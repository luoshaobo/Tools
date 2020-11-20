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
 *  \file     signal_types.h
 *  \brief    VOC Service signal base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNAL_TYPES_SIGNAL_H_
#define VOC_SIGNAL_TYPES_SIGNAL_H_


namespace volvo_on_call
{
enum VocSignalTypes
{
    kBasicCarControl,
    kBccCl001Signal,
    kBccCl002Signal,
    kBccCl010Signal,
    kBccCl011Signal,
    kBccFunc001Signal,
    kBasicCarControlSignal,
    kBccCl020Signal,
    kCaCat001Signal,
    kCaCat002Signal,
    kCaDel001Signal,
    kAesBleDaKeyVocFrame, //*< AES encrypted voc frame containing BDAK
    kAesCertActorVocFrame, //*< AES encrypted voc frame containing actor Cert.
    kAesCertNearfieldVocFrame, //*< AES encrypted voc frame containing nearfield Cert.
    kAesCaVocFrame, //*< AES encrypted voc frame containing vehicle CA.
    kAesCarActorCertVocFrame, //*< AES encrypted voc frame containing vehicle actor.
    kAesCsrActorVocFrame, //*< AES encrypted voc frame containing actor CSR
    kAesCsrNearfieldVocFrame, //*< AES encrypted voc frame containing nearfield CSR
    kEcdheApprovalVocFrame, //*< voc frame containing ECDHE Approval
    kEcdheRequestVocFrame, //*< voc frame containing ECDHE Request
    kVehicleCommSignal,  //*< signal issued by Vehicle Comm
    kPrivacyNotificationSignal,  //*< signal issued by IPCommandBroker
    kDeadReckonedPositionSignal,  //*< signal issued by IPCommandBroker
    kGNSSPositionDataSignal,  //*< signal issued by IPCommandBroker
    kCatalogueSignal,//*< signal triggering uploading of certificates
    kCaDel002Signal,
    kCaDel010Signal,
    kCaDpa001Signal,
    kCaDpa002Signal,
    kCaDpa020Signal,
    kCaDpa021Signal,
    kBleConnectionSignal,
    // Here start internal signal types
    kPositionUpdateInternalSignal,
    // End of internal signal types
#ifdef VOC_TESTS
    kTestSignalId0,
    kTestSignalId1,
    kTestSignalId2,
    kTestSignalId3,
    kTestSignalId4,
    kTestSignalId5,
    kTestSignalId6,
    kTestSignalId7,
    kTestSignalId8,
    kTestSignalId9,
#endif

    kNumberOfVocSignals //keep this one last
};

enum VocInternalSignalTypes
{
    //svt internal signal types
    kSvtReStart = 20000,
    kSvtStart,
    kSvtPeriod,
    kSvtStop,

    // remote control internal signal types
    kRmcStart,
    kRmtCancel,

    // rvdc internal signal types
    kRvdcNewReq,
    kRvdcExeReq,

    kAppStartEngineRequestSignal, //*< signal of start engine request by applications
    kAppStartEngineResponseSignal, //*< signal of start engine response, that request comes from other applications

    kNumberOfVocInternalSignals //keep this one last
};
} // namespace volvo_on_call

#endif //VOC_SIGNAL_TYPES_SIGNAL_H_

/** \}    end of addtogroup */
