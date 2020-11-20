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
 *  \file     response_info_data.h
 *  \brief    VOC Service ResponseInfoData class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_RESPONSE_INFO_DATA_H_
#define VOC_FRAMEWORK_SIGNALS_RESPONSE_INFO_DATA_H_

// asn1c include
#include "ResponseInfo.h"

#include <string>

namespace fsm
{

class ResponseInfoData
{

 public:

    /**
     * \brief Known success codes.
     */
    enum ResponseCodeSuccess
    {
        kInvalidSuccessCode,
        kUnspecified, //used when no special success code specified
        kCreated,
        kDeleted,
        kValid,
        kChanged,
        kContent,
        kNumberOfSuccessCodes //keep this code last
    };

    /**
     * \brief Known error codes.
     */
    enum ResponseCodeError
    {
        kInvalidErrorCode,
        kBadRequest,
        kUnauthorized,
        kBadOption,
        kForbidden,
        kNotFound,
        kMethodNotAllowed,
        kNotAcceptable,
        kPreconditionFailed,
        kRequestEntityTooLarge,
        kUnsupportedContentFormat,
        kInternalError,
        kBadGateway,
        kServiceUnavailable,
        kGatewayTimeout,
        kProxyingNotSupported,
        kNumberOfErrorCodes  //keep this code last
    };

    /**
     * \brief Sets result to successful
     * \param[in] result_code  success code to set, defaults to Unspecified (legal setting)
     * \param[in] success_info  optional success info to set, defaults to none
     * \return Returns true if code specified correctly, otherwise false
     */
    bool SetSuccess(ResponseCodeSuccess result_code = ResponseCodeSuccess::kUnspecified,
                    const std::string& success_info = std::string() );

    /**
     * \brief Sets result in the response as error
     * \param[in] result_code  error code to set (mandatory parameter)
     * \param[in] error_info  optional error info to set, defaults to none
     * \param[in] size_info  size info, required when resultCode set to RequestEntityTooLarge.
     *                       This parameter is ignored for other response codes.
     * \return Returns true if input specified correctly, otherwise false
     */
    bool SetError(ResponseCodeError result_code,
                  const std::string& error_info = std::string(), const long size_info = 0);

    /**
     * \brief Check whether the response contained in the ResponseInfoData
     *        was a success.
     *
     * \return True if the response was success, false if it was error.
     */
    bool IsSuccess();

    /**
     * \brief Check whether the response contained in the ResponseInfoData
     *        was an error.
     *
     * \return True if the response was error, false if it was success.
     */
    bool IsError();

    /**
     * \brief Get the contained success code.
     *
     * \return The contained success code, or kInvalidSuccessCode if not
     *         a success response.
     */
    ResponseCodeSuccess GetSuccessCode();

    /**
     * \brief Get the contained error code.
     *
     * \return The contained error code, or kInvalidErrorCode if not
     *         a error response.
     */
    ResponseCodeError GetErrorCode();

    /**
     * \brief Get the size info, only valid if the response is an error with
     *        code kRequestEntityTooLarge.
     *
     * \return The size info, or 0 if not error with code kRequestEntityTooLarge.
     */
    long GetSizeInfo();

    /**
     * \brief Get the info message contained in the response.
     *
     * \return Returns the error info, or an empty string if it was not set.
     */
    const std::string& GetInfoMessage();

    /**
     * \brief Compare a ResponseInfoData to this for equality.
     * \param[in] other The ResponseInfoData to comapre to.
     * \return True if the ResponseInfoDatas hold the same data,
     *         Otherwise false.
     */
    bool operator== (const ResponseInfoData& other);

 protected:

    /**
     * \brief Packs the device pairing id into the provided
     *        ASN1C struct.
     * \param[out] asn1c_device_pairing_id The asn1c struct to pack the pairing id into.
     *                                     Caller must ensure this parameter is
     *                                     "deep freed" even if this call fails.
     *
     * \return True if succesfull, false otherwise. Parameters
     *         are not guaranteed to be unaffected in case of failure.
     */
    bool PackData(ResponseInfo_t* asn1c_device_pairing_id);

    /**
     * \brief Populates the device pairing id from the provided
     *        ASN1C struct.
     * \param[int] asn1c_device_pairing_id The asn1c struct to load the pairing id from.
     *
     * \return True if succesfull, false otherwise.
     */
    bool UnpackData(ResponseInfo_t* asn1c_device_pairing_id);

 private:

    /**
     * \brief Response can be success, error or may be undefined
     */
    enum ResponseStatus
    {
        kResultUnspecified,
        kResultSuccess,
        kResultError
    };

    ResponseStatus status_ = kResultUnspecified;

    /**
     * \brief Response code
     */
    long code_ = kInvalidErrorCode;

    /**
     * \brief Response info message
     */
    std::string info_message_;

    /**
     * \brief Error size info, used when EntityTooLarge error is reported
     */
    long error_size_info_ = 0;
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_RESPONSE_INFO_H_

/** \}    end of addtogroup */
