/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     transfer_manager.cpp
 *  \brief    Transfer Manager implementation.
 *  \author   Iulian Sirghi & Florian Schindler
 *
 *  \addtogroup transfermanager
 *  \{
 */


// **** INCLUDES ***************************************************************
#include "transfer_manager.h"

#include <functional>
#include <string>
#include <cinttypes>

#include "dlt/dlt.h"

#include "icache.h"
#include "default_cache.h"
#include "fsm_persist_data_mgr.h"
#include "transfer_response.h"

DLT_DECLARE_CONTEXT(dlt_libfsmtm);

namespace fsm
{

namespace transfermanager
{

void FSM_TransferManager_Initialise()
{
    DLT_REGISTER_CONTEXT_LL_TS(dlt_libfsmtm, "FSTM","libfsm_transfermanager", DLT_LOG_INFO, DLT_TRACE_STATUS_ON);  // for debugging purpose enabe also DLT_LOG_VERBOSE
    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_INFO, "Initializing libfsm_transfermanager");
}


void FSM_TransferManager_Terminate()
{
    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_INFO, "Terminating libfsm_transfermanager");
    DLT_UNREGISTER_CONTEXT(dlt_libfsmtm);
}


const std::string TransferManager::kDefaultHostKey = "fsm_default_host";

std::mutex TransferManager::global_sync_;
std::condition_variable TransferManager::coma_response_;


ICommunicationManagerProxy* TransferManager::coma_com_mgr_ = nullptr;
std::uint32_t TransferManager::coma_ref_count_ = 0;

TransferManager::ComaState TransferManager::coma_client_state_ = TransferManager::ComaState::kUninitialized;
static const uint32_t kComaTimeout = 10000;  ///< Number of milliseconds to wait until coma activation time-out.

TransferManager::TransferManager(const std::string& host, std::shared_ptr<ICache> cache)
    : host_(host),
      cache_(cache),
      coma_com_proxy_state_(ComaState::kUninitialized),
      coma_com_data_(nullptr),
      coma_com_(nullptr)
{
    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_DEBUG,__func__);
    if (!cache)
    {
        // no cache implementation supplied, fallback to our default.
        cache_.reset(new DefaultCache());
    }

    // Creates the CoMa client-side instance if there is none globally-available.
    if (CreateComaClient() != ErrorCode::kNone)
    {
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s(): Failed to init COMA", __func__);
#ifndef VOC_TESTS
        raise(SIGTERM);
#endif
    }
    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_DEBUG,__func__);
}

TransferManager::TransferManager()
    : TransferManager("", nullptr)
{
}

TransferManager::~TransferManager()
{

    if (coma_com_)
    {
        coma_com_mgr_->releaseCommunicator(coma_com_->getCommunicatorId());

        coma_com_ = 0;
        coma_com_data_ = 0;
    }


    // Decrease the CoMa instance reference count.
    // We will only release the CoMa instance once all local TransferManager instances are destroyed.
    // TODO:Iulian: We are actually not context-aware to release the client-side instance. This should be fixed -
    // We ought to have the Application responsible for the lifetime of the CoMa client-side instance as CoMa
    // provides mutiple services. (Passing the CommunicationManagerProxy instance to the TransferManager instance seems
    // a way-better idea!).
    global_sync_.lock();
    --coma_ref_count_;

    if (coma_ref_count_ == 0)
    {
        destroyCommunicationManagerProxy();
        coma_com_mgr_ = 0;
    }
    global_sync_.unlock();
}


std::shared_ptr<ICache> TransferManager::GetCache()
{
    return cache_;
}


ErrorCode TransferManager::Read(Request& request, std::shared_ptr<Response>& response)
{
    ErrorCode err_code = ErrorCode::kNone;
    bool request_has_abs_uri = false;

    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_DEBUG,__func__);
    // if the request is found to be relative, we need to form another request object as
    // cache cannot work with relative requests.
    err_code = request.HasAbsoluteUri(request_has_abs_uri);

    if (err_code == ErrorCode::kNone)
    {
        Request request_copy(request);

        std::string final_uri;
        request.GetUri(final_uri);
        if (!request_has_abs_uri)
        {
            err_code = MakeAbsoluteUri(final_uri, final_uri);
            err_code = err_code == ErrorCode::kNone ? request_copy.SetUri(final_uri) : err_code;
        }

        if (err_code == ErrorCode::kNone)
        {
            Request& final_request = request_has_abs_uri ? request : request_copy;
            std::shared_ptr<TransferResponse> transfer_response
                    = std::make_shared<TransferResponse>();
            if (transfer_response.get() != nullptr)
            {
                transfer_response->SetUri(final_uri);  // save URI, needed later to save response in cache
                bool must_get_data_from_internet = true;
                must_get_data_from_internet = ! cache_->HasValidCache(final_uri);
                if (!must_get_data_from_internet)
                {
                    transfer_response->SetState(Response::State::kRequestQueued);
                    bool result = cache_->FillResponseFromCache(transfer_response, Response::StatusCode::kNotModified);
                    if (result)
                    {
                        response = transfer_response;
                        transfer_response->Done();  // finalise and notify waiting users
                    }
                    else
                    {
                        must_get_data_from_internet = true;  // if cache handling fails, then get data from Internet
                    }
                }

                if (must_get_data_from_internet)
                {
                    // Cache-miss or cache error, use CoMa to retrieve the response.
                    cache_->PrepareGetRequestFromCache(final_request);
                    // pass request to CoMa
                    transfer_response->SetState(Response::State::kRequestQueued);
                    err_code = SendGetRequest(final_request, transfer_response);

                    // form the stream and update the response.
                    if (err_code == ErrorCode::kNone)
                    {
                        response = transfer_response;
                    }
                    else
                    {
                        // try to return stale cache in case of COMA issues:
                        bool result = cache_->FillResponseFromCache(transfer_response, Response::StatusCode::kStaleCache);
                        if (result)
                        {
                            response = transfer_response;
                            transfer_response->Done();  // finalise and notify waiting users
                            err_code = ErrorCode::kNone;
                        }
                        else
                        {
                            // return kTimeout to mark transport-layer failure.
                            err_code = ErrorCode::kTimeout;
                        }
                    }
                }
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s no memory",__func__);
                err_code = ErrorCode::kInternalError;
            }
        }
    }
    return err_code;
}


ErrorCode TransferManager::SendGetRequest(const Request& request,
                                          std::shared_ptr<TransferResponse> response)
{
    ErrorCode err_code = ErrorCode::kNone;
    std::string uri;

    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_DEBUG,__func__);
    if (CheckState() != ErrorCode::kNone)
    {
        // It's all bad, the CoMa CommunicationManagerProxy was not initialized.
        err_code = ErrorCode::kBadState;
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s CommunicationManagerProxy was not initialized",__func__);
    }
    else
    {
        global_sync_.lock();
        // try to determine if we should create CoMa Communicator or if it was previously done.
        if (coma_com_data_ == 0)
        {
            // CoMa Communicator is to be activated. Attempt activation and if it fails report the error code.
            err_code = CreateComaCommunicator();
        }
        global_sync_.unlock();

        // CoMa Communicator is created and activated.
        if (err_code == ErrorCode::kNone)
        {
            std::vector<std::string> head_lines;

            err_code = request.GetUri(uri);
            err_code = err_code == ErrorCode::kNone ? request.GetHeader(head_lines) : err_code;

            if (err_code == ErrorCode::kNone)
            {
                // Perform the httpsGet via CoMa interface.
                RequestStatus req_status;

                coma_com_data_->httpsGet(req_status,
                                        uri,
                                        head_lines,
                                        std::bind(&TransferManager::ComaHttpsResponseCallback,
                                                  this,
                                                  response,
                                                  std::placeholders::_1));
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s request.GetUri(%s) failed", __func__, uri);
            }
        }
    }

    return err_code;
}


ErrorCode TransferManager::SetDefaultHost(const std::string& host)
{
    // new default host, lets save it!
    fsm::fsmpersistdatamgr::FsmPersistDataMgr persistant_data_manager;
    bool persistance_success = persistant_data_manager.FsmPersistDataInit();

    persistance_success = persistance_success
                          ? persistant_data_manager.FsmPersistDataSet(kDefaultHostKey, host)
                          : persistance_success;

    return  persistance_success ? ErrorCode::kNone : ErrorCode::kBadState;
}

ErrorCode TransferManager::GetDefaultHost(std::string& host)
{
    ErrorCode err_code = ErrorCode::kNone;

    fsm::fsmpersistdatamgr::FsmPersistDataMgr persistant_data_manager;

    if (persistant_data_manager.FsmPersistDataInit())
    {
        const std::string persistent_host = persistant_data_manager.FsmPersistDataGet(kDefaultHostKey);
        host.append(persistent_host);
    }
    else
    {
        err_code = ErrorCode::kBadState;
    }

    return err_code;
}


ErrorCode TransferManager::SetValidCacheFlag(const std::string uri, const bool valid)
{
    ErrorCode err_code = ErrorCode::kBadResource;
    bool request_has_abs_uri = false;
    std::string full_uri;
    Request request(uri);

    err_code = request.HasAbsoluteUri(request_has_abs_uri);
    if (err_code == ErrorCode::kNone)
    {
        if  (!request_has_abs_uri)
        {
            MakeAbsoluteUri(uri, full_uri);
        }
        else
        {
            full_uri = uri;
        }
        err_code = GetCache()->SetValidFlag(full_uri, valid);
    }
    else
    {
        // do nothing, just return error code
    }
    return err_code;
}


ErrorCode TransferManager::GetValidCacheFlag(const std::string uri, bool &valid)
{
    ErrorCode err_code = ErrorCode::kBadResource;
    bool request_has_abs_uri = false;
    std::string full_uri;
    Request request(uri);

    err_code = request.HasAbsoluteUri(request_has_abs_uri);
    if (err_code == ErrorCode::kNone)
    {
        if  (!request_has_abs_uri)
        {
            MakeAbsoluteUri(uri, full_uri);
        }
        else
        {
            full_uri = uri;
        }
        err_code = GetCache()->GetValidFlag(full_uri, valid);
    }
    else
    {
        // do nothing, just return error code
    }
    return err_code;
}


ErrorCode TransferManager::CreateComaClient()
{

    ErrorCode err_code = ErrorCode::kNone;

    // Create the CoMa CommunicationManagerProxy client-side instance and initialize it.
    global_sync_.lock();

    if (!coma_com_mgr_)
    {
        coma_com_mgr_ = ::createCommunicationManagerProxy("TransferManager", "libcomaclient.so");

        if (!coma_com_mgr_)
        {
            // Failed to retrieve CoMa instance.
            err_code = ErrorCode::kBadValue;
        }
        else
        {

            ICommunicationManagerProxy::InitialisationStatus init_status = coma_com_mgr_->getInitStatus();

            switch (init_status)
            {
                case ICommunicationManagerProxy::INITIALISATIONSTATUS_NOTSTARTED:
                case ICommunicationManagerProxy::INITIALISATIONSTATUS_FAILED:
                {
                    err_code = InitializeComaClient();

                    if (err_code == ErrorCode::kNone)
                    {
                        // Increase our manual reference count.
                        ++coma_ref_count_;
                    }
                    else
                    {
                        // Initialization of the CoMa CommunicationManagerProxy client-side instance failed.
                        // Mark the client state as error the destroy the previously-acquired instance.
                        coma_client_state_ = ComaState::kError;
                        destroyCommunicationManagerProxy();
                        coma_com_mgr_ = 0;
                    }

                    break;
                }

                case ICommunicationManagerProxy::INITIALISATIONSTATUS_SUCCESS:
                {
                    // CoMa already initialized, simple set the state.
                    coma_client_state_ = ComaState::kClientInitialized;

                    // Also increase the ref-count (as we are actually holding the instance).
                    // Increase our manual reference count.
                    ++coma_ref_count_;
                    break;
                }

                // TODO:Iulian:Try to handle INITIALISATIONSTATUS_STARTING
                // rather than failing. Personally, I currently don't know how
                // We need to sync with Elmar and maybe get an idea if he can internally
                // handle this attempt to retrieve multiple client-side instances.

                default:
                {
                    coma_client_state_ = ComaState::kError;
                    break;
                }
            }
        }

    }

    global_sync_.unlock();

    return err_code;
}


ErrorCode TransferManager::InitializeComaClient()
{
    RequestStatus rq_status;
    ErrorCode err_code = ErrorCode::kNone;

    bool timeout = false;

    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_DEBUG,__func__);
    static std::function<void (ResponseComaInitClient&)>
            callback = std::bind(&TransferManager::ComaInitializationCallback, std::placeholders::_1);

    coma_client_state_ = ComaState::kClientInitializing;
    bool init_ok = coma_com_mgr_->init(rq_status, callback);

    if (!init_ok)
    {
        err_code = ErrorCode::kTimeout;
        coma_client_state_ = ComaState::kError;
    }
    else
    {
        // Our global mutex is already taken by the caller.
        std::unique_lock<std::mutex> lk(global_sync_, std::defer_lock_t());

        timeout = !coma_response_.wait_for(lk,
                                 std::chrono::milliseconds(kComaTimeout),
                                 []{return coma_client_state_ != ComaState::kClientInitializing;});
        if (timeout)
        {
            // Allocated time for initialization has elapsed.
            // We are marking error code timeout and we transition CoMa state to error-case.
            err_code = ErrorCode::kTimeout;
            coma_client_state_ = ComaState::kError;
        }
    }

    return err_code;
}

ErrorCode TransferManager::CreateComaCommunicator()
{
    ErrorCode err_code = ErrorCode::kNone;

    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_DEBUG,__func__);
    if (coma_com_mgr_ != 0
        && coma_client_state_ != ComaState::kError)
    {
        ResourceDescriptor rd;
        rd.m_type = ResourceDescriptor::RES_TYPE_CALL; //< TODO:is this the right thing to use? (Elmar mentions this is not used yet).

        bool timeout = false;

        {
            std::unique_lock<std::mutex> lk(global_sync_, std::defer_lock_t());

            if (!coma_com_)
            {
                std::function<void (ICommunicatorProxy::EventCommunicatorActivationChanged&)>
                        callback = std::bind(&TransferManager::ComaActivationCallback, this, std::placeholders::_1);

                coma_com_proxy_state_ = ComaState::kCommunicatorActivating;

                coma_com_mgr_->getCommunicator(rd,
                                               coma_com_,
                                               callback);

                TransferManager* tm = this;
                timeout = !coma_response_.wait_for(lk,
                                         std::chrono::milliseconds(kComaTimeout),
                                         [tm]{return tm->coma_com_proxy_state_ != ComaState::kCommunicatorActivating;});
            }
        }

        {
            if (timeout)
            {
                // Allocated time for activation has elapsed.
                // We are marking error code timeout and we transition CoMa state to error-case.
                err_code = ErrorCode::kTimeout;
                coma_com_proxy_state_ = ComaState::kError;
                DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s getCommunicator timed out",__func__);
            }
            else
            {
                coma_com_data_ = coma_com_->getICommunicatorData();
                err_code = coma_com_data_ == 0 ? ErrorCode::kBadState : err_code;
            }
        }
    }
    else
    {
        err_code = ErrorCode::kBadState;
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s coma_com_mgr_already initialized",__func__);
    }

    return err_code;
}


void TransferManager::ComaInitializationCallback(ResponseComaInitClient& init_response)
{
    std::lock_guard<std::mutex> lk(global_sync_);

    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_DEBUG,__func__);
    if (init_response.status() == ::com::contiautomotive::communicationmanager::messaging::internal::STATUS_SUCCESS)
    {
        coma_client_state_ = ComaState::kClientInitialized;
    }
    else
    {
        coma_client_state_ = ComaState::kError;
        DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_ERROR, "%s returned %i",\
          __func__, static_cast<int>(init_response.status()) );
    }

    coma_response_.notify_all(); // Coma client-side instance is initialized.
}


void TransferManager::ComaActivationCallback(ICommunicatorProxy::EventCommunicatorActivationChanged& event)
{
    DLT_LOG_STRING(dlt_libfsmtm, DLT_LOG_DEBUG,__func__);
    if (event.activated == true)
    {
        std::lock_guard<std::mutex> lk(global_sync_);

        coma_com_proxy_state_ = ComaState::kCommunicatorActivated;
        coma_response_.notify_all(); // CoMa CommunicatorProxy is activated.
    }
}


void TransferManager::ComaHttpsResponseCallback(std::shared_ptr<TransferResponse> response_1,
                                                ResponseCommunicatorDataHttpsGet& callback)
{
    Response::StatusCode http_response_code = static_cast<Response::StatusCode>(callback.httpcode());
    DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_INFO, "%s(%s): http_response_code=%i",\
         __func__, response_1->GetUri().c_str(), static_cast<int>(http_response_code));
    std::shared_ptr<TransferResponse> response;
    std::shared_ptr<TransferResponse> response_from_cache;

    if (http_response_code == Response::StatusCode::kOk)
    {
        response = response_1;
    }
    else
    {
        response_from_cache = response_1;
        response = std::make_shared<TransferResponse>();  // Create new temporary object, which will receive response from Coma
        response->SetUri(response_from_cache->GetUri());
        response->SetState(Response::State::kRequestQueued);  // Don't know if this is really necessary
    }
    std::stringstream response_stream;

    if ( (http_response_code == Response::StatusCode::kOk) || // 200 OK, i.e. use response
         (http_response_code == Response::StatusCode::kNotModified) )  // 304 not modified; here we need the updated header strings
    {
        const std::uint32_t header_line_count = callback.header_size();
        for (std::uint32_t header_line_index = 0; header_line_index < header_line_count; ++header_line_index)
        {
            const std::string kHeaderLine = callback.header(header_line_index);
            DLT_LOG_STRINGF(dlt_libfsmtm, DLT_LOG_VERBOSE, "rx header(%i) = %s",\
                header_line_index, kHeaderLine.c_str());
            response_stream << std::noskipws << kHeaderLine << std::noskipws << "\r\n";
        }
        response_stream << std::noskipws << "\r\n";
        // Add the payload in the response stream
        response_stream << std::noskipws << callback.payload();

        // Update and parse everything.
        // Coma decompresses compressed content, which makes the
        // ContentLength header value invalid for the purpose of
        // processing the response, so we override it.
        response->SetContentLength(callback.payload().size());
        // Coma does not put status line in headers so set it explicitly.
        // If coma or tsm ever start assigning non standard values to http code enums this will break.
        response->SetStatusCode(http_response_code);
        response->Update(response_stream);  // this parses the header fields too, which are needed later
        if (http_response_code == Response::StatusCode::kOk)
        {
            cache_->HandleOkResponseInCache(response);
            response->Done();
        }  // StatusCode::kOk (200 OK)
        else  // Response::StatusCode::kNotModified
        {
            // caution: http_response_code codes must be set before FillResponseFromCache is called, because with the
            // first byte written to response_from_cache the waiting user is notified.
            cache_->FillResponseFromCache(response_from_cache, Response::StatusCode::kNotModified);
            // must be done before HandleNotModifiedInCache, which might delete the cache record
            cache_->HandleNotModifiedInCache(response);
            response_from_cache->Done();  // users are alread notified in FillResponseFromCache, because of asynchronous handling
        }  // not StatusCode::kOk (200 OK)
    }
    else  // neither kOk nor kNotModified,
    {  //  according to requirements we should deliver the cache entry in case of any errors
        // caution: http_response_code codes must be set before FillResponseFromCache is called, because with the
        // first byte written to response_from_cache the waiting user is notified.
        bool result = cache_->FillResponseFromCache(response_from_cache, Response::StatusCode::kStaleCache);
        if (!result)  // if there is notihing in the database, return error code provided by COMA
        {
            response_from_cache->SetStatusCode(http_response_code);
        }
        response_from_cache->Done();  // users are alread notified in FillResponseFromCache, because of asynchronous handling
    }  // neither kOk nor kNotModified
    // release our reference.
}


ErrorCode TransferManager::CheckState()
{
    std::lock_guard<std::mutex> lk(global_sync_);

    ErrorCode err_code = ErrorCode::kNone;

    if (coma_client_state_ == ComaState::kError
        || coma_com_mgr_ == 0)
    {
        err_code = ErrorCode::kBadState;
    }

    return err_code;
}


ErrorCode TransferManager::MakeAbsoluteUri(const std::string uri_in, std::string &uri_out)
{
    ErrorCode err_code = ErrorCode::kNone;

    // we can deal with a relative URI in two ways:
    // prepend the per-instance host configuration
    // if per-instance host is empty, we prepend the global host configuration.
    std::string final_uri;

    if (host_ == "")  // if passed host_ is empty then get default_host from persistency
    {
        std::string default_host;
        err_code = GetDefaultHost(default_host);
        final_uri = default_host;
    }
    else
    {
        final_uri = host_;
    }
    if (err_code == ErrorCode::kNone)
    {
        TrimUri(final_uri, final_uri);
        final_uri.append(uri_in);
        uri_out = final_uri;
    }
    return err_code;
}


void TrimUri(const std::string uri_in, std::string &uri_out)
{
    std::string whitespaces(" \n\r\t\f\v");  // space, CR, LF, tab, form feed, horizontal tab

    // find first occurance of leading (left) non-whitespace:
    std::string::size_type left_index = uri_in.find_first_not_of(whitespaces);
    left_index = left_index == std::string::npos ? 0 : left_index;

    // find first occurance of trailing (right) non-whitespace:
    std::string::size_type right_index = uri_in.find_last_not_of(whitespaces);
    right_index = right_index == std::string::npos ? 0 : right_index;  // if std::string::npos then the entire string consists of whitespaces

    // copy the trimmed string to output:
    std::string::size_type len = right_index - left_index +1;
    uri_out = uri_in.substr(left_index, len);
}



} // namespace transfermanager
} // namespace fsm

/** \}    end of addtogroup */
