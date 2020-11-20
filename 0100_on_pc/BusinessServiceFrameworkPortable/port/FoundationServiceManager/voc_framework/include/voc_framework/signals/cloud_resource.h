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
 *  \file     cloud_resource.h
 *  \brief    Cloud Resource
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */


#ifndef VOC_FRAMEWORK_SIGNALS_CLOUD_RESOURCE_H_
#define VOC_FRAMEWORK_SIGNALS_CLOUD_RESOURCE_H_

#include "fscfg/bus_names.h"
#include "fscfg/fscfg.h"
#include "fscfg/provisionedresource_interface.h"

#include "voc_framework/signals/car_access_signal.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/xml_payload.h"


#include <algorithm>
#include <string>
#include <functional>
#include <future>
#include <memory>

#include "dlt/dlt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

const std::string kCarAccessResourceName = std::string(fscfg_kCarAccessResource);

/**
 * \brief CloudResource class provides means for accessing the cloud configuration.
 * */
template <class T>
class CloudResource
{
 private:

    /**
     * \brief value used for undefined callbacks
     */
    const std::uint32_t kUndefinedCallbackId = 0;

 public:
    /**
     * \brief Constructs CloudResource instance
     * \param[in] resource_name the name of the cloud resource
     */
     CloudResource(std::string resource_name) : resource_name_(resource_name),
                                                provisioned_resource_(nullptr),
                                                provisioning_iface_(nullptr),
                                                payload_changed_id_(kUndefinedCallbackId),
                                                resources_changed_id_(kUndefinedCallbackId),
                                                init_succeeded_(false),
                                                binding_thread_active_(false)
     {
         Init();
     }

     /**
      * \brief Can be used to check if initialization has been successful. Note that initialization will be re-tried
      *        so the state may change with time
      *  \return true if successful, false otherwise.
      */
     bool IsInitialized()
     {
         return init_succeeded_;
     }

     /**
      * \brief Provides the current configuration
      * \return signal containing latest configuration.
      *         nullptr in case valid configuration cannot be retrieved for any reason.
      */
     std::shared_ptr<T> GetPayload()
     {
         std::vector<std::uint8_t> payload;  //payload from resource interface
         std::shared_ptr<T> response_typed; //response

         if (IsInitialized())
         {
             std::shared_ptr<EncodingInterface> encoding;

             fscfg_ReturnCode result_payload = provisioned_resource_->GetPayload(payload);

             fscfg_ReturnCode result_encoding = provisioned_resource_->GetEncoding(encoding);


             if (result_payload == fscfg_kRcSuccess && result_encoding == fscfg_kRcSuccess && payload.size() != 0)
             {
                 std::string encoding_name;
                 fscfg_ReturnCode result_encoding_name = encoding->GetName(encoding_name);

                 if (result_encoding_name == fscfg_kRcSuccess)
                 {
                     if (encoding_name == std::string(fscfg_kCcmEncoding))
                     {


                         std::shared_ptr<Signal> response_signal =
                                 SignalFactory::DecodeCcm(reinterpret_cast<unsigned char*>(payload.data()),
                                                          payload.size());

                         if (response_signal)
                         {
                             response_typed = std::dynamic_pointer_cast<T>(response_signal);
                         }
                         else
                         {
                             DLT_LOG_STRINGF(dlt_libfsm,
                                             DLT_LOG_ERROR,
                                             "Failed to decode payload (CCM) for : %s.",
                                             resource_name_.c_str());
                         }
                     }
                     else if (encoding_name == std::string(fscfg_kXmlEncoding))
                     {
                         std::shared_ptr<fsm::XmlPayload> xml_payload =
                             std::make_shared<fsm::XmlPayload>();

                        if(xml_payload)//modify klocwork warning,yangjun add 20181107
                        {
                            fsm::PayloadInterface::PayloadIdentifier xml_identifier =
                             xml_payload->GetIdentifier().payload_identifier;

                             std::shared_ptr<fsm::PayloadInterface> decoded_payload =
                                 fsm::SignalFactory::DecodePayload(reinterpret_cast<unsigned char*>(payload.data()),
                                                                   payload.size(),
                                                                   &xml_identifier);
                             if (decoded_payload)
                             {
                                 response_typed = std::dynamic_pointer_cast<T>(decoded_payload);
                             }
                             else
                             {
                                 DLT_LOG_STRINGF(dlt_libfsm,
                                                 DLT_LOG_ERROR,
                                                 "Failed to decode payload (XML) for : %s.",
                                                 resource_name_.c_str());
                             }
                        }
                     }
                     else
                     {
                         DLT_LOG_STRINGF(dlt_libfsm,
                                         DLT_LOG_ERROR,
                                         "Unsupported encoding for : %s.Encoding: %s.",
                                         resource_name_.c_str(), encoding_name);
                     }
                 }
             }
             else
             {
                 DLT_LOG_STRINGF(dlt_libfsm,
                                 DLT_LOG_ERROR,
                                 "Failed to retrieve data for payload : %s. Payload result: %d, Encoding result: %d,  Payload size: %d",
                                 resource_name_.c_str(), result_payload, result_encoding, payload.size());
             }
         }
         else
         {
             DLT_LOG_STRINGF(dlt_libfsm,
                             DLT_LOG_ERROR,
                             "No interface for resource: %s. Resource will be unavailable.",
                             resource_name_.c_str());
         }

         return response_typed;
     }


     /**
      * \brief Provides the current payload in a binary form, not decoded
      * \return undecoded signal containing latest configuration.
      *         empty vector in case valid configuration cannot be retrieved for any reason.
      */
     std::vector<std::uint8_t> GetRawPayload()
     {
         std::vector<std::uint8_t> payload;  //payload from resource interface

         if (IsInitialized())
         {

             fscfg_ReturnCode result_payload = provisioned_resource_->GetPayload(payload);

             if (result_payload != fscfg_kRcSuccess)
             {
                 DLT_LOG_STRINGF(dlt_libfsm,
                                 DLT_LOG_ERROR,
                                 "Failed to retrieve data for payload : %s. Payload result: %d, Payload size: %d",
                                 resource_name_.c_str(), result_payload, payload.size());

                 //defensive, as there is no guarantee in the documentation that the input
                 //parameter to GetPayload won't be changed in case error is returned
                 payload.clear();
             }
         }
         else
         {
             DLT_LOG_STRINGF(dlt_libfsm,
                             DLT_LOG_ERROR,
                             "No interface for resource: %s. Resource is unavailable.",
                             resource_name_.c_str());
         }

         return payload;
     }


     /**
      * \brief Registers change handler. Only one handler is registered at the time, registering new handler invalidates
      *        previous registered handler.
      * \param[in] resource_changed_cb callback function that will be called when the payload change has been detected.
      *            Events triggering re-check of the configuration are ExpiryNotification or re-read of the cloud resource.
      *            No active monitoring of the cache expiry is performed. New data will be provided in the callback. Do not
      *            call other CloudResource calls (e.g. GetPayload) from inside the change handler as it may lead to a deadlock
      */
     void RegisterChangeHandler(std::function<void(std::shared_ptr<T> new_payload)> resource_changed_cb)
     {
         //remember the callback
         change_callback_ = resource_changed_cb;
         //call init, it will perform payload binding if necessary
         Init();
     }


     /**
      * \brief Unregisters the callback. Make sure to call this prior to destroying
      *        either the CloudResource object or anything holding the client callback.
      */
     void UnregisterCallback()
     {

             //unregister callbacks for payload change and expiry
             if (payload_changed_id_ != kUndefinedCallbackId)
             {
                 provisioned_resource_->Unbind(payload_changed_id_);
             }

             if (resources_changed_id_ != kUndefinedCallbackId)
             {
                 provisioning_iface_->Unbind(resources_changed_id_);
             }

             //remove client registered callback
             change_callback_ = nullptr;
             payload_changed_id_ = kUndefinedCallbackId;
             resources_changed_id_ = kUndefinedCallbackId;

     };

     /**
      * \brief Destroys CloudResource object
      */
     ~CloudResource()
     {
         //call unregister callback (safety, in case someone has forgotten to unregister this will
         //minimize the risk of crashes). It is not an equivalent to calling this method properly
         UnregisterCallback();
     };

private:
     /**
      * \brief Resource name
      */
     std::string resource_name_;

     /**
      * \brief Provisioned resource interface
      */
     std::shared_ptr<ProvisionedResourceInterface> provisioned_resource_;

     /**
      * \brief Provisioning interface
      */
     std::shared_ptr<ProvisioningInterface> provisioning_iface_;

     /**
      * \brief payload change callback registered by a client
      */
     std::function<void(std::shared_ptr<T> new_payload)> change_callback_;

     /**
      * \brief payload change callback id, for unbinding
      */
     std::uint32_t payload_changed_id_;

     /**
      * \brief resources changed callback id, for unbinding
      */
     std::uint32_t resources_changed_id_;

     /**
      * \brief keeps track if init has been successful
      */
     bool init_succeeded_;

     /** \brief Mutex synchronizing access to the initialization procedure */
     std::mutex initialization_mutex_;

     /** \brief Keeps track of the information if we have already tried to create a binding thread
      *         for delayed binding.
      */
     bool binding_thread_active_;

     /** \brief Mutex synchronizing creation of the binding thread. Used to assure that only one such thread is created*/
     std::mutex binding_thread_mutex_;

     /** \brief Handle to a thread handling deferred binding. Only one such thread is created. The reason
      *         for having it in the scope of a class is not to block return from callback that indicates that
      *         the requested resource has become available (as this may lead to deadlock)
      */
     std::future<bool> async_handle_;

     /**
      * \brief Provides information if initialization towards fsconfig has been successful. If it has not
      *        it first attempts to initialize before returning the status. Will also attempt to bind
      *        previously requested binding
      * \return true if initialization has been successful, false otherwise (note that binding status is not
      *              reflected in the return value, as it is internal retry-mechanism)
      */
     bool Init()
     {
         //make sure init does not happen in parallel
         std::lock_guard<std::mutex> lock(initialization_mutex_);

         //Get provisioning interface
         if (!provisioning_iface_)
         {
             provisioning_iface_ = fsm::GetProvisioningInterface();
             if (!provisioning_iface_)
             {
                 DLT_LOG_STRINGF(dlt_libfsm,
                                 DLT_LOG_ERROR,
                                 "Failed to fetch provisioning interface for: %s.",
                                 resource_name_.c_str());
                 //TODO: consider shutdown, this should not happen
             }
         }

         //check if not already iniitalized, then try to initialize the provisioned resource
         if (!init_succeeded_)
         {
             if (provisioning_iface_)
             {
                 //first sign up for resources changes callback before attempting to connect.
                 //This is to make sure that callback is not missed if resource change happens
                 //while executing this init method
                 if (resources_changed_id_ == kUndefinedCallbackId)
                 {
                     std::function<fscfg_ReturnCode (ProvisioningInterface::ResourcesChangedEvent)> resources_cb
                             = std::bind(&CloudResource<T>::ResourcesChangedCb, this, std::placeholders::_1);

                     fscfg_ReturnCode result = provisioning_iface_->BindResourcesChanged(resources_cb, resources_changed_id_);
                     if (result != fscfg_kRcSuccess)
                     {
                         DLT_LOG_STRINGF(dlt_libfsm,
                                         DLT_LOG_ERROR,
                                         "Failed to register binding on resources changed for: %s.",
                                         resource_name_.c_str());
                         //TODO: this should not happen ever, consider shutdown
                     }
                 }

                 //initialize provisioned_resource
                 fscfg_ReturnCode result = provisioning_iface_->Get(resource_name_, provisioned_resource_);

                 if (result != fscfg_kRcSuccess)
                 {
                     DLT_LOG_STRINGF(dlt_libfsm,
                                     DLT_LOG_INFO,
                                     "Failed to retrieve provisioned resource: %s,"
                                     " error: %d\n.",
                                     resource_name_.c_str(),
                                     result);
                 }
                 else
                 {
                     init_succeeded_ = true;
                 }
             }
         }

         //if we have callback request but we have not registered yet to the config_fs, register.
         //this should succeed if init has succeeded
         if (init_succeeded_ && change_callback_ && (payload_changed_id_ == kUndefinedCallbackId))
         {
             //create payload change callback
             std::function<fscfg_ReturnCode (ProvisionedResourceInterface::PayloadChangedEvent)> payload_cb
                     = std::bind(&CloudResource<T>::PayloadChangedCb, this, std::placeholders::_1);
             //bind payload change callback
             if (provisioned_resource_->BindPayloadChanged(payload_cb, payload_changed_id_) != fscfg_kRcSuccess)
             {
                 DLT_LOG_STRINGF(dlt_libfsm,
                                 DLT_LOG_ERROR,
                                 "Failed to bind payload change for resource: %s.",
                                 resource_name_.c_str());
                 //TODO: this should never happen, consider shutdown
             }

         }

         return init_succeeded_;
     };

     /**
      * \brief Calls Init to perform initialization. If initialization is successful, and there is a callback
      *        registered, then the callback will be executed to notify the client about a change
      */
     bool PerformInitializationAndNotifyOnChange()
     {
         //Call Init to attempt initialization
         bool init_successful = Init();

         //If initialization has been successful and we have a callback registered, it should be called
         if (init_successful && change_callback_)
         {
             std::shared_ptr<T> payload = GetPayload();
             if (payload)
             {
                change_callback_(payload);
             }
         }
         binding_thread_active_ = false;

         return init_successful;

     }

     /**
      * \brief payload change callback
      * \param[in] event callback structure containing the new payload
      */
     fscfg_ReturnCode PayloadChangedCb(ProvisionedResourceInterface::PayloadChangedEvent event)
     {
         if (change_callback_)
         {
             //create signal out of the new payload
             std::shared_ptr<Signal> response_signal =
                 SignalFactory::DecodeCcm(reinterpret_cast<unsigned char*>(event.payload.data()),
                                          event.payload.size());

             if (response_signal)
             {
                 //if signal successfully created, cast it to desired signal type
                 //if casting fails we get nullptr which is a valid output in such case
                 std::shared_ptr<T> response_typed; //response
                 response_typed = std::dynamic_pointer_cast<T>(response_signal);
                 change_callback_(response_typed);
             }
             else
             {
                 DLT_LOG_STRINGF(dlt_libfsm,
                                 DLT_LOG_ERROR,
                                 "In callback failed to decode payload for : %s.",
                                 resource_name_.c_str());
             }
         }
         return fscfg_kRcSuccess;
     };


     /**
     * \brief resources changed callback, used to bind on resource changes to discover
     *        when the resource becomes available
     * \param[in] event callback structure containing the new resource list
     */
     fscfg_ReturnCode ResourcesChangedCb(ProvisioningInterface::ResourcesChangedEvent event)
     {
         //check if the resource in question is available by comparing the name
         auto iterator = std::find_if(event.resources.begin(), event.resources.end(),
                                     [&](std::shared_ptr<ProvisionedResourceInterface> resource)
                                        {
                                            std::string name;
                                            bool result = false;
                                            if (resource->GetName(name) == fscfg_kRcSuccess)
                                            {
                                                result = (name.compare(resource_name_) == 0);
                                            }
                                            return result;
                                        });

         bool resource_available_in_cloud = (iterator != event.resources.end());

         //if requested resource is available and we are not bound then try to initialize
         if (resource_available_in_cloud && (!IsInitialized()) )
         {
             //make sure we do not risk creating two binding threads
             std::lock_guard<std::mutex> lock(binding_thread_mutex_);

             //when the resource becomes available we start Init in a new thread to perform binding
             if (binding_thread_active_ == false)
             {
                 //launch binding procedure in a separate thread
                 //since async_handle is in scope of the class we are not blocking
                 //callback return and async thread will execute in parallel
                 //this is to avoid deadlock when calling other provisionedresource operation
                 //from the registered callback
                 binding_thread_active_ = true;
                 async_handle_ = std::async(std::launch::async, &CloudResource<T>::PerformInitializationAndNotifyOnChange, this);
             }
         }
         //if we are bound but the resource has been removed, disconnect
         else if ((!resource_available_in_cloud) && IsInitialized())
         {
             if (payload_changed_id_ != kUndefinedCallbackId)
             {
                 provisioned_resource_->Unbind(payload_changed_id_);
                 payload_changed_id_ = kUndefinedCallbackId;
             }
             init_succeeded_ = false;
             provisioned_resource_ = nullptr;

             //notify the user if callback registered
             if (change_callback_)
             {
                 change_callback_(nullptr);
             }
         }

         return fscfg_kRcSuccess;
     };
};


/**
 * \brief CaFunc002CloudResource class is a specialized helper class for accessing
 *        the car access configuration.
 * */
class CaFunc002CloudResource: public CloudResource<CarAccessSignal>
{
public:
    /**
     * \brief Constructs CaFunc002CloudResource instance.
     */
    CaFunc002CloudResource();
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_CLOUD_RESOURCE_H_

/** \}    end of addtogroup */
