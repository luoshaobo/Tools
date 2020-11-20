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
 *  \file     source_interface.h
 *  \brief    Foundation Services Source subject interface
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_SOURCE_INTERFACE_H_INC_
#define FSM_SOURCE_INTERFACE_H_INC_

#include <cstdint>
#include <memory>
#include <vector>

#include "fscfg_types.h"


namespace fsm
{

class EncodingInterface;
class ResourceInterface;

/**
 * \brief Foundation Services Source subject interface
*/
class SourceInterface
{
public:

    /**
     * \brief BindUpdated event parameters.
     */
    struct UpdatedEvent
    {
        std::shared_ptr<SourceInterface> source;                     ///< source that generated the event.
        std::vector<std::shared_ptr<ResourceInterface>> resources;   ///< list of current resources.
    };

    /**
     * \brief Updated Callback.
     */
    typedef Callback<UpdatedEvent> UpdatedCb;

    /**
     * \brief Virtual Destructor.
     */
    virtual ~SourceInterface() { }

    /**
     * \brief Registers a callback function which will get called when a set of owned resources get updated.
     *
     * \param[in] func function object acting as callback.
     * \param[out] id unique binding identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode BindUpdated(UpdatedCb func, std::uint32_t& id) = 0;

    /**
     * \brief Unbind existing callback function.
     *
     * \param[in] id binding unique identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode Unbind(std::uint32_t id) = 0;

    /**
     * \brief Retrieves the name of the source instance.
     *
     * \param[out] name name of the source instance.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetName(std::string& name) = 0;

    /**
     * \brief Retrieves all the known encodings for a given, owned, resource.
     *
     * \param[in] resource a source-owned resource.
     * \param[out] encodings set of known payload encodings for the given resource.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetEncodings(std::shared_ptr<ResourceInterface> resource,
                                          std::vector<std::shared_ptr<EncodingInterface>>& encodings) = 0;

    /**
     * \brief Retrieve if the source instance holds any payloads for a given resource.
     *
     * \param[in] resource resource instance.
     * \param[out] has_resource true if source instance has payloads for the given resource, false otherwise.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode HasResource(std::shared_ptr<ResourceInterface> resource, bool& has_resource) = 0;

    /**
     * \brief Retrieves the payload for an owned resource and given encoding.
     *
     * \param[in] resource a source-owned resource.
     * \param[in] encoding payload encoding.
     * \param[out] payload actual payload.
     *
     * \return fscfg_kRcSuccess on success.
     */
    virtual fscfg_ReturnCode GetPayload(std::shared_ptr<ResourceInterface> resource,
                                        std::shared_ptr<EncodingInterface> encoding,
                                        std::vector<std::uint8_t>& payload) = 0;
};

} // namespace fsm

#endif // FSM_SOURCE_INTERFACE_H_INC_

/** \}    end of addtogroup */
