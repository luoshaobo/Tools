/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     feature_processor.h
 *  \brief    Foundation Services Config additional per-feature processing logic interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_FEATURE_PROCESSOR_H_INC_
#define FSM_FEATURE_PROCESSOR_H_INC_


#include <fscfgd/vsdpsource.h>
#include <fscfgd/feature.h>
#include <fscfgd/resource_binding_handler.h>

namespace fsm
{

/**
 * \brief Tracks and processes feature data in order to determine parameters related to child resources. (such as
 * resource existence, URI etc).
 * Each trackable feature has a processing method associated inside the FeatureProcessor to deduce the child resources.
 * The processing method is also called when a change notification has been received from the feature - this will cause
 * the processing method to deduce the child resources again and check for their existence in order to update the
 * provisioning container.
 * A trackable feature's processing method is associated when FeatureProcessor constructor is called.
 */
class FeatureProcessor : public ResourceBindingHandler
{
public:
    /**
     * \brief FeatureProcessor constructor.
     *
     * \param[in] vsdp_source VSDP data source used for URI mapping when tracking child resources.
     * \param[in] config config container.
     */
    FeatureProcessor(std::shared_ptr<VsdpSource> vsdp_source, std::shared_ptr<Config> config);

    /**
     * \brief Adds a given feature to the internal house-keeping list. House-keeping involves processing the feature payload
     * in order to deduce the existence of the child resources as well as their URI. Additionally, change notification are handled
     * to re-process the resource and check the child resources for existence, when necessary.
     *
     * \param[in] feature feature to be tracked
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcNotPermitted if feature is not trackable (no processing method associated),\n
     *         fscfg_kRcBadParam if feature pointer is null.
     */
    fscfg_ReturnCode TrackFeature(std::shared_ptr<Feature> feature);

    /**
     * \brief Removes the given feature from the internal house-keeping list.
     *
     * \param[in] feature feature to remove from internal house-keeping list.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if feature is not actually being tracked,\n
     *         fscfg_kRcBadParam if feature pointer is null.
     *
     */
    fscfg_ReturnCode UntrackFeature(std::shared_ptr<Feature> feature);

    /**
     * \brief Retrives the list of feature names that are trackable. A feature is trackable if there is a processing
     * method associated to it.
     *
     * \param[out] vector of feature names that will get appended with the processing method associated (thus, considered trackable).
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode GetTrackableFeatures(std::vector<std::string>& trackable_features);

private:
    /**
     * \brief Pointer to private method assigned to perform additional processing for the parameter-provided feature.
     *
     * \param[in] feature that is to be processed.
     * \param[in] payload payload to be processed (optional, if empty vector the payload will be retrieved).

     */
    typedef fscfg_ReturnCode (FeatureProcessor::*ProcessingMethod)(std::shared_ptr<Feature> feature,
                                                                   std::vector<std::uint8_t> &payload);

    ///! Disabled copy-constructor.
    FeatureProcessor(const FeatureProcessor& other);

    ///! Disabled assignment operator.
    FeatureProcessor& operator=(const FeatureProcessor& other);

    /**
     * \brief Maps all known feature to their respective private method for performing additional processing.
     */
    void PerformProcessingMethodMapping();

    /**
     * \brief Call the processing method for the parameter provided feature name.
     *
     * \param[in] feature_name the feature name to call the processing method for.
     * \param[in] payload payload, if not available put empty vector and payload will be retrieved.

     */
    void CallProcessor(const std::string& feature_name, std::vector<std::uint8_t> &payload);

    /**
     * \copydoc ResourceBindingHandler::OnBoundResourcePayloadChanged
     */
    fscfg_ReturnCode OnBoundResourcePayloadChanged(ResourceInterface::PayloadsChangedEvent payloads_changed_event);

    /**
     * \brief Processing method for the External Diagnostics feature.
     * During processing, the additional child resources (Remote Session) related to External
     * Diagnostics are uri-mapped inside the VSDP source and added inside the provisioning container.
     *
     * \param[in] External Diagnostics feature.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode ProcessExternalDiagnostics(std::shared_ptr<Feature> feature, std::vector<std::uint8_t> &payload);

    /**
     * \brief Processing method for the Assistance Call feature.
     * During processing, the additional child resources (Call) related to External
     * Diagnostics are uri-mapped inside the VSDP source and added inside the provisioning container.
     *
     * \param[in] External Diagnostics feature.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode ProcessAssistanceCall(std::shared_ptr<Feature> feature, std::vector<std::uint8_t>& payload);

    std::map<std::string, std::shared_ptr<Feature>> tracked_features_;  ///< Internal tracked resources that are under house-keeping.
    std::map<std::string, ProcessingMethod> feature_process_method_;    ///< Feature name to process method mapping.
    std::shared_ptr<VsdpSource> vsdp_source_;                           ///< Vsdp source used for URI-Mapping.
};

}

#endif // FSM_FEATURE_PROCESSOR_H_INC_

/** \}    end of addtogroup */
