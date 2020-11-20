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
 *  \file     car_locator_map_request_transaction.h
 *  \brief    VOC Service car locator map request transaction.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_CAR_LOCATOR_MAP_REQUEST_TRANSACTION_H_
#define VOC_TRANSACTIONS_CAR_LOCATOR_MAP_REQUEST_TRANSACTION_H_


#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/response_info_data.h"

#include "signals/bcc_cl_010_signal.h"
#include "transactions/car_position_provider.h"

#include "ipcb_IGNSSService.hpp"

#ifndef VOC_TESTS
  #include "voc_framework/signals/vehicle_comm_signal.h"
#endif


namespace volvo_on_call
{

class CarLocatorMapRequestTransaction: public CarPositionProvider
{
 public:

    /**
     * \brief Constructs a new CarLocatorMapRequestTransaction.
     */
    CarLocatorMapRequestTransaction ();

    /**
     * \brief Checks if transaction wants a given signal.
     * \param[in] signal fsm::Signal to check.
     * \return True if:
     *         fsm::Signal is a BCC-CL-010 and transaction is still in state #kNew.
     *         fsm::Signal is a VCRES_CARMODE or VCRES_CARUSAGEMODE and its transaction id is mapped.
     *         fsm::Signal is a DeadReckonedPosition or GNSSPositionData and its transaction id is mapped.
     */
    bool WantsSignal (std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle a signal.
     *        BCC-CL-010 signal will trigger a check on the car status. If car mode
     *        and car usage mode indicate a parked car, a request will be made to retrieve
     *        the current postion. The order of preference for position information is:
     *        IHU position (map matched or dead reckoned) is primary choice, if this information
     *        is not available, a position from the location manager is used. When position
     *        is established the response (BCC-CL-011) is sent)
     * \param[in] signal fsm::Signal to handle.
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleSignal (std::shared_ptr<fsm::Signal> signal);

 private:

    /**
     * The initial request.
     */
    std::shared_ptr<BccCl010Signal> request_;

    /**
     * \brief Possible states of the transaction
     */
    enum State
    {
        kNew, ///< Created,  no signals have been processed-
        kCheckPreconditions,  ///< Initial signal accepted, now checking if the operation is allowed to be performed
        kRequestSentToCar,///<  Awaiting car state information from VehicleComm.
        kRequestForPositon,///< Awaiting position information from the Ip Command broker.
        kDone, ///< Done, transaction is terminating.
    };

    /**
     * \brief Tracks the current state.
     */
    State state_ = kNew;

    /**
     * \brief Stores the information if the car mode precondition is fullfiled
     */
    bool car_mode_ = false;

    /**
     * \brief Stores the information if the car usage mode precondition is fullfiled
     */
    bool car_usage_mode_ = false;

    /**
     * \brief Handles signal initiating the transaction. If the transaction is in a right state the check
     *        of preconditions is initiated
     * \param[in] signal fsm::Signal to handle.
     */
    void HandleBccCl010Signal(std::shared_ptr<BccCl010Signal> signal);

    /**
     * \brief Handles VehicleComm signals (VCRES_CARMODE, VCRES_CARUSAGEMODE). These signals carry the car
     *        modes information used to determine if transaction preconditions are fulfilled. If the preconditions
     *        are fulfilled, a position retrieval operation is initiated, otherwise the transaction is terminated.
     * \param[in] signal fsm::Signal to handle.
     */
    void HandleVehicleCommSignal(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Checks car mode precondition. Takes car mode response (VCRES_CARMODE) as input.
     *        If the transaction is in a right state when the signal is received then the car_mode
     *        variable will be set accordingly. If preconditions are not fulfilled, the transaction
     *        will be terminated (state_ will be set to kDone)
     * \param[in] signal fsm::Signal to handle.
     */
    void CheckCarModePrecondition(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Checks car usage mode precondition. Takes car usage mode response (VCRES_CARUSAGEMODE) as input.
     *        If the transaction is in a right state when the signal is received then the car_mode
     *        variable will be set accordingly. If preconditions are not fulfilled, the transaction
     *        will be terminated (state_ will be set to kDone)
     * \param[in] signal fsm::Signal to handle.
     */
    void CheckCarUsageModePrecondition(std::shared_ptr<fsm::Signal> signal);


    /**
     * \brief Handles position information from the Ip Command broker and sends the response signal (BCC-CL-011).
     *        The order of preference for position information source is taken into account when determining
     *        the postion. Preferably the IHU originating position is used, if that is not available then the TCAM
     *        originating position is sent. If the position has not beed retrieved successfully from any of the sources
     *        then the error code is sent.
     * \param[in] signal fsm::Signal to handle.
     */
    void HandlePosition(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Sends BCC-CL-011 with error code set
     * \param[in] error_code error code to be set.
     * \param[in] message error message.
     */
    void SendError(fsm::ResponseInfoData::ResponseCodeError error_code, std::string message = std::string());

    /**
     * \brief Sends BCC-CL-011 with position retrieved from IHU
     * \param[in] dead_reckoned_position position information.
     * \return True message could be sent, false otherwise
     */
    bool SendDeadReckonedPosition(DRPositionData& dead_reckoned_position);

    /**
     * \brief Sends BCC-CL-011 with position retrieved from TCAM
     * \param[in] gnss_position position information.
     * \return True message could be sent, false otherwise
     */
    bool SendGNSSPosition(GNSSData& gnss_position);

    /**
     * \brief Sends BCC-CL-011 with position retrieved from IHU or retrieved from TCAM.
     * \param[in] dead_reckoned_position position information.
     * \param[in] gnss_data gnss position information, will only be used if
     *                           dead_reckoned_position not provided.
     * \return True message could be sent, false otherwise
     */
    bool SendPositionUpdate(DRPositionData* dead_reckoned_position, GNSSData* gnss_data);


};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_CAR_LOCATOR_TRANSACTION_H_

/** \}    end of addtogroup */
