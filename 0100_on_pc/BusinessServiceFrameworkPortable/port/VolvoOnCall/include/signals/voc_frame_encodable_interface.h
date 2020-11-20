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
 *  \file     voc_frame_encodable_interface.h
 *  \brief    VOC Service Voc Frame encodable interface.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_VOC_FRAME_ENCODABLE_INTERFACE_H_
#define VOC_SIGNALS_VOC_FRAME_ENCODABLE_INTERFACE_H_

#include "voc_frame_types.h"

#include <vector>

namespace volvo_on_call
{

/**
 * \brief Any class implementing a Voc Frame type which wants to be
 *        encodable by VocFrameCodec must implement this interface.
 *        There is no decodable interface, simply add a case in the
 *        VocFrameCodec::Decode function to call the relevant creator.
 */
class VocFrameEncodableInterface
{
 public:

    /**
     * \brief Returns the VocFrameType of the frame instance.
     *
     * \return The VocFrameType of the frame instance.
     */
    virtual VocFrameType GetFrameType() const = 0;

    /**
     * \brief Provides the frame instance's payload as a byte buffer.
     *
     * \param[in,out] payload The Frames paylaod should be appended to this.
     *
     * \return True on success, false on failure.
     */
    virtual bool GetPayload(std::vector<unsigned char>& payload) const = 0;
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_VOC_FRAME_ENCODABLE_INTERFACE_H_

/** \}    end of addtogroup */
