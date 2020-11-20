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
 *  \file     resource_interface.h
 *  \brief    Foundation Services Resource subject interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_RESOURCE_INTERFACE_H_INC_
#define FSM_RESOURCE_INTERFACE_H_INC_

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>

#include "fscfg_types.h"

#include "source_interface.h"
#include "encoding_interface.h"

namespace fsm
{

/**
 * \brief Foundation Services Resource subject interface
 */
class ResourceInterface
{
public:
    /**
     * \brief OnExpired event parameters.
     */
    struct ExpiredEvent
    {
        std::shared_ptr<ResourceInterface> resource;    ///< resource instance that triggered the expiry notification.
        bool immediate;                                 ///< true if resource should be fetched immediately, false if just caches need to be updated.
    };

    /**
     * \brief OnUpdated event parameters.
     */
    struct UpdatedEvent
    {
        std::shared_ptr<ResourceInterface> resource;    ///< resource instance that got updated.
        std::shared_ptr<SourceInterface> source;        ///< source that triggered the update.
    };

    /**
     * \brief OnSourcesChanged event parameters.
     */
    struct SourcesChangedEvent
    {
        std::shared_ptr<ResourceInterface> resource;                ///< resource instance that got it's sources changed.
        std::vector<std::shared_ptr<SourceInterface>> sources;      ///< source that triggered the update.
    };

    /**
     * \brief OnPayloadChanged event parameters.
     */
    struct PayloadsChangedEvent
    {
        std::shared_ptr<ResourceInterface> resource;        ///< resource instance that got it's payload changed.
        std::vector<std::uint8_t> payload;                  ///< current payload.
        std::shared_ptr<SourceInterface> source;            ///< source that triggered the payload change.
        std::shared_ptr<EncodingInterface> encoding;        ///< encoding in which the changed payload is described in.
    };


    /**
     * \brief Expired Callback.
     */
    typedef Callback<ExpiredEvent> ExpiredCb;

    /**
     * \brief Updated Callback.
     */
    typedef Callback<UpdatedEvent> UpdatedCb;

    /**
     * \brief Resources Changed Callback.
     */
    typedef Callback<SourcesChangedEvent> SourcesChangedCb;

    /**
     * \brief Resources Changed Callback.
     */
    typedef Callback<PayloadsChangedEvent> PayloadsChangedCb;

    /**
     * \brief virtual Destructor.
     */
    virtual ~ResourceInterface() { }

    /**
     * \brief Registers a callback function which will get called when a resource expires.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindExpired(ExpiredCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when a resource gets updated.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindUpdated(UpdatedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when a resource sources set changes.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindSourcesChanged(SourcesChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Registers a callback function which will get called when Payloads set changed.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindPayloadsChanged(PayloadsChangedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Unbind existing callback function.
     *
     * \param[in] id binding unique identifier.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcNotFound if given id was not found.
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
     * \brief Retrieves all the sources that have payloads for the resource instance.
     *
     * \param[out] sources sources that have payloads for the resource instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetSources(std::vector<std::shared_ptr<SourceInterface>>& sources) = 0;

    /**
     * \brief Retrieves all the possible encodings in which the resource instance payload is described in.
     *
     * \param[out] encodings encodings list of available payload encodings.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetEncodings(std::vector<std::shared_ptr<EncodingInterface>>& encodings) = 0;

    /**
     * \brief Retrieves the payload for the resource instance owned by given source in given encoding.
     *
     * \param[in] encoding encoding of the retrieved payload.
     * \param[in] source source to retrieve payload from.
     * \param[out] payload retrieved payload.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcBadResource if given source and/or encoding does not match a payload.
     */
    virtual fscfg_ReturnCode GetPayload(std::shared_ptr<EncodingInterface> encoding,
                                        std::shared_ptr<SourceInterface> source,
                                        std::vector<std::uint8_t>& payload) = 0;

};

} // namespace fsm

#endif // FSM_RESOURCE_INTERFACE_H_INC_

/** \}    end of addtogroup */
