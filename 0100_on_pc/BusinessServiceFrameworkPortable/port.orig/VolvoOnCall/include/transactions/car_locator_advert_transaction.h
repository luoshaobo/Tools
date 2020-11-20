/**
 * Copyright (C) 2016 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     car_locator_advert_transaction.h
 *  \brief    VOC Service car locator advert transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_CAR_LOCATOR_TRANSACTION_H_
#define VOC_TRANSACTIONS_CAR_LOCATOR_TRANSACTION_H_

#include "voc_framework/signals/response_info_data.h"

#include "signals/bcc_cl_001_signal.h"


#ifndef VOC_TESTS
#include "voc_framework/signals/vehicle_comm_signal.h"
#endif
#include "voc_framework/transactions/transaction.h"


namespace volvo_on_call
{

class CarLocatorAdvertTransaction: public fsm::Transaction
{
 public:

    /**
     * \brief Constructs a new CarLocatorTransaction.
     */
    CarLocatorAdvertTransaction ();

    /**
     * \brief Checks if transaction wants a given signal.
     * \param[in] signal fsm::Signal to check.
     * \return True if:
     *         fsm::Signal is a BCC-CL-001 and transaction is still in state #kNew.
     *         Singal is a VCRES_HORNNLIGHT and its transaction id is mapped.
     */
    bool WantsSignal (std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle a signal.
     *        If signal is a BCC-CL-001 and transaction is still in state #kNew
     *        it will be propagated to VehicleComm.
     *        If signal is a VCRES_HORNNLIGHT and its transaction id is mapped
     *        a response will be sent to the original requester.
     * \param[in] signal fsm::Signal to handle.
     * \return True if transaction is not yet finished, false if it is.
     */

    bool HandleSignal (std::shared_ptr<fsm::Signal> signal);

 private:

    /**
     * \brief Possible states of the transaction
     */
    enum State
    {
        kNew, ///< Created but no singals have been processed-
        kRequestSentToCar, ///< Reuqest received and sent to VehicleComm, awaiting response.
        kDone, ///< Done, no new singals should be handeld.
    };

    /**
     * \brief Tracks the current state.
     */
    State state_ = kNew;

    /**
     * The initial request.
     */
    std::shared_ptr<BccCl001Signal> request_;

    /**
     * \brief Session id of the intial request. Will be empty if it was not provided.
     *        Should be used when responding if not empty.
     */
    std::string request_session_id_;

#ifndef VOC_TESTS
    /**
     * \brief Handle an incomming BCC-CL-002.
     * \param[in] bcc_cl001_signal The incomming signal.
     * \return True if tranasction should terminate,
     *            otherwise false.
     */
    bool HandleBccCl001 (std::shared_ptr<BccCl001Signal> bcc_cl001_signal);

    /**
     * \brief Handle an incomming vc::RES_HORNNLIGHT.
     * \param[in] vc_return_value internal VehicleComm return value for the request.
     * \param[in] horn_and_light_return Response to thr request.
     * \return True if tranasction should terminate,
     *         otherwise false.
     */
    bool HandleHornAndLightResponse (vc::ReturnValue vc_return_value,
                                     vc::HNLReturnCode horn_and_light_return_value);
#endif
    /**
     * \brief Sends a BCC-CL-002 response with success code.
     * \return None
     */
    void SendSuccess();


    /**
     * \brief sends a BCC-CL-002 response with error code.
     * \param[in] error_code Error code, default to internal error,
     * \param[in] message    Optional message to attach.
     * \return None
     */
    void SendError(fsm::ResponseInfoData::ResponseCodeError error_code =
                       fsm::ResponseInfoData::ResponseCodeError::kInternalError,
                   const std::string& message = std::string());

};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_CAR_LOCATOR_TRANSACTION_H_

/** \}    end of addtogroup */
