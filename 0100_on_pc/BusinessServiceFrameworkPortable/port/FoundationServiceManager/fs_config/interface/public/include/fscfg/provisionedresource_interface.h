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
 *  \file     provisionedresource_interface.h
 *  \brief    Foundation Services ProvisionedResource subject interface.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_PROVISIONEDRESOURCE_INTERFACE_H_INC_
#define FSM_PROVISIONEDRESOURCE_INTERFACE_H_INC_

#include <memory>
#include <functional>

#include "fscfg_types.h"
#include "source_interface.h"
#include "resource_interface.h"

namespace fsm
{

/**
 * \brief Foundation Services ProvisionedResource subject interface.
*/
class ProvisionedResourceInterface
{
public:

    /**
     * \brief OnPayloadChanged event parameters.
     */
    struct PayloadChangedEvent
    {
        std::shared_ptr<ProvisionedResourceInterface> resource;     ///< resource instance that triggered the payload change.
        std::vector<std::uint8_t> payload;                          ///< resource current payload.
    };

    /**
     * \brief OnSourceChanged event parameters.
     */
    struct SourceChangedEvent
    {
        std::shared_ptr<ProvisionedResourceInterface> resource;     ///< resource instance that triggered the source change.
        std::shared_ptr<SourceInterface> source;                    ///< resource highest priority source.
    };

    /**
     * \brief Payload Changed Callback.
     */
    typedef Callback<PayloadChangedEvent> PayloadChangedCb;

    /**
     * \brief Source Changed Callback.
     */
    typedef Callback<SourceChangedEvent> SourceChangedCb;

    /**
     * \brief virtual Destructor.
     */
    virtual ~ProvisionedResourceInterface() { }

    /**
     * \brief Registers a callback function which will get called when the resource payload changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindPayloadChanged(PayloadChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when the source payload changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindSourceChanged(SourceChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Unbind existing callback function.
     *
     * \param[in] id binding unique identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode Unbind(std::uint32_t id) = 0;

    /**
     * \brief Retrieves the name of the resource instance.
     *
     * \param[out] name name of the resource instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetName(std::string& name) = 0;

    /**
     * \brief Retrieves the highest priority encoding of the resource instance.
     *
     * \param[out] encoding highest priority encoding of the resource instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetEncoding(std::shared_ptr<EncodingInterface>& encoding) = 0;

    /**
     * \brief Retrieves the highest priority source of the resource instance.
     *
     * \param[out] source highest priority source of the resource instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetSource(std::shared_ptr<SourceInterface>& source) = 0;

    /**
     * \brief Retrieves the payload of the resource from the highest priority source.
     *
     * \param[out] payload payload from highest priority source.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetPayload(std::vector<std::uint8_t>& payload) = 0;

};

} // namespace fsm

#endif // FSM_PROVISIONEDRESOURCE_INTERFACE_H_INC_

/** \}    end of addtogroup */
