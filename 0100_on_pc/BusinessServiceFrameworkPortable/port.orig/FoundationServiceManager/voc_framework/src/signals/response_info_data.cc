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
 *  \file     response_info_data.cc
 *  \brief    VOC Service ResponseInfoData class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/response_info_data.h"

#include "dlt/dlt.h"
#include "fsm_ccm.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

bool ResponseInfoData::SetSuccess(ResponseCodeSuccess result_code, const std::string& success_info)
{
    bool return_value = true;

    if ((result_code == ResponseCodeSuccess::kInvalidSuccessCode) ||
        (result_code >= ResponseCodeSuccess::kNumberOfSuccessCodes))
    {
        return_value = false;
    }
    else
    {
        //set the values
        code_ = result_code;
        info_message_ = success_info;
        status_ = kResultSuccess;
    }

    return return_value;
}

bool ResponseInfoData::SetError(ResponseCodeError result_code, const std::string& error_info, const long size_info)
{
    bool return_value = true;

    if ((result_code == ResponseCodeError::kInvalidErrorCode) ||
        (result_code >= ResponseCodeError::kNumberOfErrorCodes ))
    {
        return_value = false;
    }
    else
    {
        //For RequestEntityTooLarge size1Info must be additionally specified
        if (result_code == kRequestEntityTooLarge )
        {
            if (size_info == 0)
            {
                return_value = false;
            }
            else
            {
                error_size_info_ = size_info;
            }
        }
        else
        {
            error_size_info_ = 0;
        }
    }

    if (return_value)
    {
        //set the remaining values
        code_ = result_code;
        info_message_ = error_info;
        status_ = kResultError;
    }

    return return_value;
}

bool ResponseInfoData::IsSuccess()
{
    return status_ == ResponseStatus::kResultSuccess;
}

bool ResponseInfoData::IsError()
{
    return status_ == ResponseStatus::kResultError;
}

ResponseInfoData::ResponseCodeSuccess ResponseInfoData::GetSuccessCode()
{
    return static_cast<ResponseCodeSuccess>(code_);
}

ResponseInfoData::ResponseCodeError ResponseInfoData::GetErrorCode()
{
    return static_cast<ResponseCodeError>(code_);
}

long ResponseInfoData::GetSizeInfo()
{
    return error_size_info_;
}

const std::string& ResponseInfoData::GetInfoMessage()
{
    return info_message_;
}

bool ResponseInfoData::operator== (const ResponseInfoData& other)
{
    return status_ == other.status_ &&
           code_ == other.code_ &&
           info_message_ == other.info_message_ &&
           error_size_info_ == other.error_size_info_;
}

bool ResponseInfoData::PackData(ResponseInfo_t* response)
{
    bool return_value = true;

    if (response)
    {
        switch(status_)
        {
        case kResultSuccess :
        {
            //set the type of response (mandatory)
            response->present = ResponseInfo_PR_success;

            //if code is set to Unspecified then is should not be set
            if (code_ != kUnspecified)
            {
                response->choice.success.code = static_cast<long*>(calloc(1, sizeof(long)));
                if (response->choice.success.code == NULL)
                {
                    return_value = false;
                }
                else
                {
                    switch (code_)
                    {
                    case kCreated :
                        *(response->choice.success.code) = SuccessCode_created;
                        break;
                    case kDeleted :
                        *(response->choice.success.code) = SuccessCode_deleted;
                        break;
                    case kValid :
                        *(response->choice.success.code) = SuccessCode_valid;
                        break;
                    case kChanged :
                        *(response->choice.success.code) = SuccessCode_changed;
                        break;
                    case kContent :
                        *(response->choice.success.code) = SuccessCode_content;
                        break;
                    default:
                        //we should never end up here, if we do, it means that new success codes
                        //have been added that this method is not supporting
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                        "Unsupported success code received: %d\n", code_);
                        return_value = false;
                    }
                }
            }

            //set success message (optional)
            if (return_value && info_message_.size() != 0)
            {
                response->choice.success.info = static_cast<OCTET_STRING_t*>(calloc(1, sizeof(OCTET_STRING_t)));
                if (response->choice.success.info == NULL)
                {
                    return_value = false;
                }

                //set message, size -1 means that everything until endl is taken
                if (return_value &&
                    (0 != OCTET_STRING_fromBuf(response->choice.success.info, info_message_.c_str(), -1)))
                {
                    return_value = false;
                }
            }
            break;
        }
        case kResultError :
        {
            //set the type of response (mandatory)
            response->present = ResponseInfo_PR_error;

            //set the error code (mandatory)
 /*
            switch (code_)
            {

            case kBadRequest :
                response->choice.error.code = ErrorCode_badRequest;
                break;
            case kUnauthorized :
                response->choice.error.code = ErrorCode_unauthorized;
                break;
            case kBadOption :
                response->choice.error.code = ErrorCode_badOption;
                break;
            case kForbidden :
                response->choice.error.code = ErrorCode_forbidden;
                break;
            case kNotFound :
                response->choice.error.code = ErrorCode_notFound;
                break;
            case kMethodNotAllowed :
                response->choice.error.code = ErrorCode_methodNotAllowed;
                break;
            case kNotAcceptable :
                response->choice.error.code = ErrorCode_notAcceptable;
                break;
            case kPreconditionFailed :
                response->choice.error.code = ErrorCode_preconditionFailed;
                break;
            case kRequestEntityTooLarge :
                response->choice.error.code = ErrorCode_requestEntityTooLarge;
                break;
            case kUnsupportedContentFormat :
                response->choice.error.code = ErrorCode_unsupportedContentFormat;
                break;
            case kInternalError :
                response->choice.error.code = ErrorCode_internalError;
                break;
            case kBadGateway :
                response->choice.error.code = ErrorCode_badGateway;
                break;
            case kServiceUnavailable :
                response->choice.error.code = ErrorCode_serviceUnavailable;
                break;
            case kGatewayTimeout :
                response->choice.error.code = ErrorCode_gatewayTimeout;
                break;
            case kProxyingNotSupported :
                response->choice.error.code = ErrorCode_proxyingNotSupported;
                break;
            default:
                //we should never end up here, if we do, it means that new error codes
                //have been added that this method is not supporting
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "Unsupported error code received: %d\n", code_);
                return_value = false;

            }

*/

            //set error message (optional)
            if (return_value && info_message_.size() != 0)
            {
                //set message
                response->choice.error.info = static_cast<OCTET_STRING_t*>(calloc(1, sizeof(OCTET_STRING_t)));
                if (response->choice.error.info == NULL)
                {
                    return_value = false;
                }
                //set message, size -1 means that everything until '\0' is taken
                if (return_value &&
                    (0 != OCTET_STRING_fromBuf(response->choice.error.info, info_message_.c_str(), -1)))
                {
                    return false;
                }
            }

            //set size parameter (optional)
            if (return_value && (error_size_info_ > 0) && (code_ == kRequestEntityTooLarge))
            {
                //set size1Info
                response->choice.error.size1Info = static_cast<long*>(calloc(1, sizeof(long)));
                if (response->choice.error.size1Info != NULL)
                {
                    *response->choice.error.size1Info = error_size_info_;
                }
                else
                {
                    return_value = false;
                }
            }
            break;
        }
        default :
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                           "Asked to pack uninitialized ResponseInfoData.");
            return_value = false;
        }
    }
    else
    {
        return_value = false;
    }

    return return_value;
}

bool ResponseInfoData::UnpackData(ResponseInfo_t* response)
{
    bool return_value = true;

    if (response)
    {
        switch (response->present)
        {
        case ResponseInfo_PR::ResponseInfo_PR_success:
        {
            status_ = kResultSuccess;

            //code and message are optional

            //code
            SuccessCode_t *codeptr = response->choice.success.code;
            if (codeptr != NULL)
            {
                SuccessCode code = static_cast<SuccessCode>(*codeptr);

                switch (code)
                {
                case SuccessCode_created:
                    code_ = kCreated;
                    break;
                case SuccessCode_deleted:
                    code_ = kDeleted;
                    break;
                case SuccessCode_valid:
                    code_ = kValid;
                    break;
                case SuccessCode_changed:
                    code_ = kChanged;
                    break;
                case SuccessCode_content:
                    code_ = kContent;
                    break;
                default:
                    code_ = kInvalidSuccessCode;
                    return_value = false;
                }
            }
            else
            {
                code_ = kUnspecified; //valid value
            }

            //message
            UTF8String_t *info = response->choice.success.info;
            if (return_value && info)
            {
                long size = info->size;

                // text must be filled with zeros
                char *text = new char[size+1]();
                if (!ASN1C2OctetString(info, text, size))
                {
                    //failed to decode the string (e.g. incorrect length)
                    return_value = false;
                }

                info_message_.assign(text, size);
                delete[] text;
            }
            break;
        }
        case ResponseInfo_PR::ResponseInfo_PR_error:
        {
            status_ = kResultError;

            //set code
/*            ErrorCode code = static_cast<ErrorCode>(response->choice.error.code);

            switch (code)
            {
            case ErrorCode_badRequest :
                code_ = kBadRequest;
                break;
            case ErrorCode_unauthorized :
                code_ = kUnauthorized;
                break;
            case ErrorCode_badOption :
                code_ = kBadOption;
                break;
            case ErrorCode_forbidden :
                code_ = kForbidden;
                break;
            case ErrorCode_notFound :
                code_ = kNotFound;
                break;
            case ErrorCode_methodNotAllowed :
                code_ = kMethodNotAllowed;
                break;
            case ErrorCode_notAcceptable :
                code_ = kNotAcceptable;
                break;
            case ErrorCode_preconditionFailed :
                code_ = kPreconditionFailed;
                break;
            case ErrorCode_requestEntityTooLarge :
                code_ = kRequestEntityTooLarge;
                break;
            case ErrorCode_unsupportedContentFormat :
                code_ = kUnsupportedContentFormat;
                break;
            case ErrorCode_internalError :
                code_ = kInternalError;
                break;
            case ErrorCode_badGateway :
                code_ = kBadGateway;
                break;
            case ErrorCode_serviceUnavailable :
                code_ = kServiceUnavailable;
                break;
            case ErrorCode_gatewayTimeout :
                code_ = kGatewayTimeout;
                break;
            case ErrorCode_proxyingNotSupported :
                code_ = kProxyingNotSupported;
                break;
            default:
                code_ = kInvalidErrorCode;
                return_value = false;
            }

*/

            //if error message available, set it
            UTF8String_t* info = response->choice.error.info;
            if (return_value && info != NULL)
            {
                long size = info->size;

                // text must be filled with zeros
                char *text = new char[size+1]();
                if (!ASN1C2OctetString(info, text, size))
                {
                    //failed to decode the string (e.g. incorrect length)
                    return_value = true;
                }

                info_message_.assign(text, size);
                delete[] text;
            }

            long* size1Info = response->choice.error.size1Info;
            if (return_value && size1Info != NULL)
            {
                error_size_info_ = *size1Info;
            }

            break;
        }
        default:
            status_ = kResultUnspecified;
            return_value = false;
        }
    }
    else
    {
        return_value = false;
    }

    return return_value;
}

} // namespace fsm
/** \}    end of addtogroup */
