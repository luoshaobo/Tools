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
 *  \file     signal_factory.h
 *  \brief    VOC Service signal factory
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_SIGNAL_FACTORY_H_
#define VOC_FRAMEWORK_SIGNALS_SIGNAL_FACTORY_H_

#include <memory>
#include <string>
#include <map>

#include "fsm_ccm.h"

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_codec.h"
#include "voc_framework/signals/ccm_signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/gly_pack.h"
#include "voc_framework/signals/codec_interface.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signals/marben_codec.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/xml_codec.h"
#ifndef UNIT_TESTS
#include "voc_framework/signals/vehicle_comm_signal.h"
#endif

#include "asn.h"
#include "VDSasn.h"
#include "voc_framework/signals/signal.h"

//#include "signal_adapt_base.h"
class AppDataBase;

namespace fsm
{

class SignalFactory
{
public:
    struct t_factory_return
    {
        std::shared_ptr<Signal> ret_signal;
        std::shared_ptr<asn_wrapper::VDServiceRequest> copyed_vd_service;
        int service_id = Signal::kTypeUndefined;
        int error_code = 1;
    };

public:

    /**
     * \brief A factory function type for creating a legacy CcmDecodable Signal from ccm message.
     */
    typedef std::function<std::shared_ptr<Signal> (ccm_Message* ccm, TransactionId& transaction_id)> SignalFactoryFunction;

    typedef std::function<std::shared_ptr<Signal> (VdServiceTransactionId& transaction_id,VDServiceRequest  *  vdmsg)> SignalVdmsgFunction;

    typedef std::function<std::shared_ptr<Signal> (VdServiceTransactionId& transaction_id,asn_wrapper::VDServiceRequest  *  vdmsg)> VDSSignalVdmsgFunction;

    /////
    typedef std::function<std::shared_ptr<Signal> (VdServiceTransactionId& transaction_id, void *)> CreateSignalFunction;
    static void Register_CreateSignalFunction(long serviceType, CreateSignalFunction fun);

    /**
     * \brief A PayloadInterface factory function.
     */
    typedef std::function<std::shared_ptr<PayloadInterface>()> PayloadFactoryFunction;


    static void RegisterVdmsgFactory(long serviceType, SignalVdmsgFunction factory_function);

    static void RegisterVdmsgFactory(long serviceType, VDSSignalVdmsgFunction factory_function);

    static VDSSignalVdmsgFunction GetVdmsgFactory(long serviceType);


    /**
     * \brief Registers a factory function for a signal with the given OID.
     *
     * \deprecated Use PayloadInterfaces, CCMEncodable CCMDecodable is deprecated.
     *
     * \param[in] oid signals oid
     * \param[in] factory_function factory function of the signal
     */
    static void RegisterCcmFactory(std::string oid, SignalFactoryFunction factory_function);

    /**
     * \brief Registers a factory function for a given payload type. If one already exists
     *        it will be replaced.
     *
     * \param[in] identifer        Identifies which payload type the factory is registered for.
     * \param[in] factory_function The factory to register
     */
    static void RegisterPayloadFactory(PayloadInterface::PayloadIdentifier identifier,
                                       PayloadFactoryFunction factory_function);

    /**
     * \brief Registers a factory function for a given payload type. If one already exists
     *        it will be replaced.
     *
     * \param[in] oid        Identifies which payload type the factory is registered for.
     * \param[in] factory_function The factory to register
     */
    static void RegisterPayloadFactory(std::string oid,
                                       PayloadFactoryFunction factory_function);

    /**
     * \brief Registers a factory function for a given payload type. If one already exists
     *        it will be replaced.
     *
     * \tparam T Typename of PayloadInterface realization to be registered.
     *           Must be deafult constructable.
     *
     * \param[in] identifer Identifies which payload type the factory is registered for.
     */
    template <typename T>
    static void RegisterPayloadFactory(std::string oid)
    {
        PayloadFactoryFunction factory = std::bind(std::make_shared<T>);
        RegisterPayloadFactory(oid, factory);
    }

    /**
     * \brief Registers a factory function for a given payload type. If one already exists
     *        it will be replaced.
     *
     * \tparam T Typename of PayloadInterface realization to be registered.
     *           Must be deafult constructable.
     *
     * \param[in] identifer Identifies which payload type the factory is registered for.
     */
    template <typename T>
    static void RegisterPayloadFactory(PayloadInterface::PayloadIdentifier identifier)
    {
        PayloadFactoryFunction factory = std::bind(std::make_shared<T>);
        RegisterPayloadFactory(identifier, factory);
    }


    /**
     * \brief Convenience function to extract the payload from a Signal.
     *        Only works on signal types known by the system.
     *
     * \param [in] signal The signal to extract from.
     *
     * \return nullptr on failure or:
     *         CCM                      - signal casted to T will be returned
     *                                    as there is no concept of a contained payload
     *         CcmSignal                - PayloadInterface contained in CcmSignal
     *                                    casted to T will be returned.
     *         IpCommandBrokerSignal<T> - signal casted to T will be returned as
     *                                    payload is not pointer type and dont want to copy
     *         VehicleCommSignal        - Payload casted to T will be returned
     */
    template <typename T>
    static std::shared_ptr<T> GetSignalPayload(std::shared_ptr<Signal> signal)
    {
        std::shared_ptr<T> payload = nullptr;

        if (signal)
        {
            std::shared_ptr<CcmSignal> signal_as_ccm_signal = std::dynamic_pointer_cast<CcmSignal>(signal);

            if (signal_as_ccm_signal)
            {
                std::shared_ptr<PayloadInterface> ccm_signal_payload = signal_as_ccm_signal->GetPayload();
                payload = std::dynamic_pointer_cast<T>(ccm_signal_payload);
            }

#ifndef UNIT_TESTS
            if (!payload)
            {
                std::shared_ptr<VehicleCommSignalExt<T>> signal_as_vc = std::dynamic_pointer_cast<VehicleCommSignalExt<T>>(signal);

                if (signal_as_vc)
                {
                    payload = signal_as_vc->GetData();
                }
            }
#endif
            if (!payload)
            {
                std::shared_ptr<T> signal_as_t = std::dynamic_pointer_cast<T>(signal);

                if (signal_as_t)
                {
                    payload = signal_as_t;
                }
            }
        }

        return payload;
    }

    /**
     * \brief Get SignalFactoryFucntion registered for oid.
     *
     * \deprecated Use PayloadInterfaces, CCMEncodable CCMDecodable is deprecated.
     *
     * \param[in] oid Oid for which to find factory.
     *
     * \return Factory, or nullptr if none found.
     */
    static SignalFactoryFunction GetLegacySignalFactoryFunction(std::string oid);

    /**
     * \brief Get PayloadFactoryFunction registered for a paylaod identifer.
     *
     * \param[in] identifier Identifier for which to find factory.
     *
     * \return Factory, or nullptr if none found.
     */
    static PayloadFactoryFunction GetPayloadFactory(PayloadInterface::PayloadIdentifier identifier);

    /**
     * \brief Decode a CCM.
     *
     * \param[in] binary_data Data buffer to decode.
     * \param[in] num_bytes   Size of data buffer.
     *
     * \return Shared poiner to decoded ccm, or nullptr if decode failed.
     *         Note that returned signal is not guaranteed to be a CcmSignal.
     */
    static std::shared_ptr<Signal> DecodeCcm(const unsigned char* binary_data,
                                             size_t num_bytes);

    static std::shared_ptr<Signal>  DecodeService(const unsigned char* binary_data,
                                                 size_t num_bytes);

    static std::shared_ptr<Signal> DecodeServiceV1(const unsigned char* binary_data,
                                                 size_t num_bytes);
                                                 
    static std::shared_ptr<asn_wrapper::VDServiceRequest> DecodeVdService(const unsigned char* binary_data,
                                                 size_t num_bytes);

    static SignalFactory::t_factory_return DecodeServiceV2(const unsigned char* binary_data,
                                                 size_t num_bytes);

    /**
     * \brief Decode a Payload.
     *
     * \param[in] binary_data Data buffer to decode.
     * \param[in] num_bytes   Size of data buffer.
     * \param[in] identifier  Optional identifier for the payload under decode.
     *                        If not provided SignalFactory may attempt to decode anyway,
     *                        whether this is possible would depend on the data, and the attempt
     *                        may be costly and involve attempting codecs one by one.
     * \param[in] encoding    Optional information to SingalFactory on which encoding
     *                        data is encoded with. If not pårovided this amy cause decode to fail.
     * \param[in] version     Optional information to SignalFactory about which version of paylaod
     *                        the data contains.
     *
     * \retrun SharedPointer to PayloadInterface realization instance containing decoded data,
     *         or nullptr on failure.
     */
    static std::shared_ptr<PayloadInterface> DecodePayload(const unsigned char* binary_data,
                                                           size_t num_bytes,
                                                           const PayloadInterface::PayloadIdentifier* identifier,
                                                           const fs_Encoding* encoding = nullptr,
                                                           const fs_VersionInfo* version = nullptr);

    /**
     * \brief Encode a payload.
     *
     * \param[in]  payload              The paylaod to encode.
     * \param[out] used_encoding        If not nullptr will be set to the encoding used.
     * \param[out] applied_encode_flags If not nullptr will be set to a bitfield containing
     *                                  any encode flags applied.
     *
     * \return SharedPointer to vector containing encoded data, or nullptr on failure.
     *         Note that some codecs will accept empty payloads and return a empty buffer,
     *         this will not be considered an error and in such cases an empty vector is returned.
     */
    static std::shared_ptr<std::vector<unsigned char>> Encode(std::shared_ptr<const PayloadInterface> payload,
                                                              fs_Encoding* used_encoding = nullptr,
                                                              int* applied_encode_flags = nullptr);

    static std::shared_ptr<std::vector<unsigned char> > GeelyEncode(std::shared_ptr<fsm::SignalPack> signal, bool is_encode_to_xml = false,
                                                              fs_Encoding* used_encoding = nullptr,
                                                              int* applied_encode_flags = nullptr);

    static std::shared_ptr<std::vector<unsigned char> > GeelyEncodeV1(std::shared_ptr<fsm::SignalPack> signal, bool is_encode_to_xml = false,
                                                              fs_Encoding* used_encoding = nullptr,
                                                              int* applied_encode_flags = nullptr);

    static std::shared_ptr<std::vector<unsigned char> > GlySmsEncode(std::shared_ptr<fsm::SignalPack> signal,
                                                              fs_Encoding* used_encoding = nullptr,
                                                              int* applied_encode_flags = nullptr);

    static std::shared_ptr<std::vector<unsigned char> > GlySmsEncodeV1(std::shared_ptr<fsm::SignalPack> signal,
                                                              fs_Encoding* used_encoding = nullptr,
                                                              int* applied_encode_flags = nullptr);

    static void print_vd_service(asn_wrapper::VDServiceRequest * vd_service);

#ifdef ENABLE_SIGNAL_INJECTION
    /**
     * \brief Create a IpCommandBrokerSignal from the binary contents
     *        of a signal file created by the unittest.
     *        Only used for signal injection developer mode.
     * \param[in] file_contents The contents of the signal file.
     * \param[in] num_bytes     Size of file_contents.
     * \param[in] transaction_id_str Transaction ID as a string (will be converted into nummeric format)
     * \return Empty shared pointer if not successful, otherwise
     *         shared pointer to signal.
     */
    static std::shared_ptr<Signal> CreateSignalFromIpcb(const char* file_contents,
                                                        const size_t num_bytes,
                                                        const std::string transaction_id_str);
    /**
     * \brief Create a VehicleComm from the string representation.
     * Only used for signal injection developer mode.
     *
     * \param[in] serialized_string The serialized representation of the VehicleComm signal.
     * \param[in] num_bytes size of serialized_string.
     * \param[in] transaction_id_str to be used transaction ID as a string (will be converted into nummeric format)
     * \return Empty shared pointer if not successful, otherwise
     *         shared pointer to signal.
     */
    static std::shared_ptr<Signal> CreateSignalFromVehicleCommSerializedString(const char* serialized_string,
                                                                               const size_t num_bytes,
                                                                               const std::string& transaction_id_str);
#endif

private:

    /**
     *
     */
    static std::shared_ptr<CcmCodec> ccm_codec_;

    /**
     *
     */
    static std::shared_ptr<MarbenCodec> marben_codec_;

    /**
     *  Xml codec
     */
    static std::shared_ptr<XmlCodec> xml_codec_;

    /**
     * \brief Map between OIDs and corresponding factory functions
     */
    static std::map<std::string, SignalFactoryFunction> oid_factory_map_;

    /**
     * \brief Mutex guarding access to oid to factory map
     */
    static std::mutex oid_factory_map_mutex_;

    static std::map<long, CreateSignalFunction> signal_factory_map_;
    static std::mutex signal_factory_mutex_;

    static std::map<long, VDSSignalVdmsgFunction> serviceId_factory_map_;

    static std::mutex serviceId_factory_map_mutex_;


    /**
     * \brief Map between CodecTypes and corresponding codec implementations
     *        provided by clients.
     */
    static std::map<PayloadInterface::CodecType, std::shared_ptr<CodecInterface>> codec_map_;

    /**
     * \brief Mutex guarding access to Codec map
     */
    static std::mutex codec_map_mutex_;

    /**
     * \brief Map between PayloadIdentifiers and corresponding PayloadFactoryFunctions
     *        provided by clients.
     */
    static std::map<PayloadInterface::PayloadIdentifier, PayloadFactoryFunction> payload_factory_map_;

    /**
     * \brief Mutex guarding access to paylaod factory map
     */
    static std::mutex payload_factory_map_mutex_;

    /**
     *
     */
    static std::shared_ptr<const CodecInterface> GetCodec(PayloadInterface::CodecType codec_type);

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_SIGNAL_FACTORY_H_

/** \}    end of addtogroup */
