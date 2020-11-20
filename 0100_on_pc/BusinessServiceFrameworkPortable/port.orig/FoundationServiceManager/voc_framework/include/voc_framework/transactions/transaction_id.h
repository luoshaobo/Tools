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
 *  \file     transaction_id.h
 *  \brief    VOC Service transaction id class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_TRANSACTIONS_TRANSACTION_ID_H_
#define VOC_FRAMEWORK_TRANSACTIONS_TRANSACTION_ID_H_

#include <atomic>
#include <cstdint>
#include <string>
#include <string.h>
#include <memory>
#include <mutex>


namespace fsm
{

class TransactionId
{

 public:

    enum TransactionIdType
    {
        kTransactionIdTypeCCM,
        kTransactionIdTypeVC,  //*< Vehicle Comm transaction ID (= session_id)
        kTransactionIdTypeIpCommandBroker,
        kTransactionIdTypeVpom,
        kTransactionIdTypeTimeout,
        kTransactionIdTypeInternalSignal,
        kTransactionIdTypeVdService,
        kTransactionIdTypeUndefined
    };

    TransactionIdType GetType() const;

    virtual bool IsDefined () const;

    bool operator== (const TransactionId& other) const;

    bool operator!= (const TransactionId& other) const;

    /*!
     * \brief Create a copy of this TransactionId and return
     *        a shared pointer to it.
     *
     * \return Shared ptr to copy of this transaction id.
     */
    virtual std::shared_ptr<TransactionId> GetSharedCopy() const = 0;

 protected:

    TransactionIdType my_type;

    virtual bool Compare (const TransactionId& other) const = 0;

    TransactionId ();

};



class VdServiceTransactionId : public TransactionId
{
 public:

    static const uint64_t kUndefinedTransactionId = 0;

    /*!
     * \brief Constructs a VdServiceTransactionId
     *        with a unique internal id.
     */
    VdServiceTransactionId ();

    /*!
     * \brief Constructs a VdServiceTransactionId
     *        with a specific value.
     * \param[in] id The id value.
     */
    VdServiceTransactionId (uint64_t id);

    /*!
     * \brief Constructs a VdServiceTransactionId
     *        with a specific value given as a string.
     *        note: error handling is done like in std::stoull defined, i.e. throw invalid_argument exception
     *
     * \param[in] transaction_id_str  the input string
     */
    VdServiceTransactionId(const std::string transaction_id_str);

    /*!
     * \brief Get the contained id.
     * \return The internal id of this IpCommandBrokerTransactionId.
     */
    uint64_t GetId();

    /*!
     * \fn     IsDefined
     * \brief  checkes whether the object contains a valid transaction ID
     *
     * \return  true if valid
     * \return  false if invalid
    */
    bool IsDefined() const;

    /*!
     * \brief Create a copy of this TransactionId and return
     *        a shared pointer to it.
     *
     * \return Shared ptr to copy of this transaction id.
     */
    std::shared_ptr<TransactionId> GetSharedCopy() const;

 protected:

    bool Compare (const TransactionId& other) const;

 private:

    uint64_t my_id = kUndefinedTransactionId;

    static uint64_t counter_;

    /*!
     * \brief Get a numeric id of same type as internal id representation.
     *        Guaranteed to be unique to ids previously returned by this
     *        function.
     * \return Numeric id of same type as internal id representation.
     */
    static uint64_t GetNextId();
};




class CCMTransactionId : public TransactionId
{
 public:

    static const std::string kUndefinedCCMTransactionId;

    CCMTransactionId ();

    CCMTransactionId (const std::string& id);

    CCMTransactionId (char* id);

    /*!
     * \brief Writes this CCMTransactionId
     *        into a c style character sequence.
     * \param[in/out] c_string The character sequence to write to.
     *                must be atleast CCM_LONG_TRANSACTION_ID_LEN
     *                bytes large.
     */
    void WriteToCString(char* c_string) const;

    bool IsDefined() const;

    /*!
     * \brief Create a copy of this TransactionId and return
     *        a shared pointer to it.
     *
     * \return Shared ptr to copy of this transaction id.
     */
    std::shared_ptr<TransactionId> GetSharedCopy() const;

 protected:

    bool Compare (const TransactionId& other) const;

 private:

    std::string my_id;
};

/*!
* \class VehicleCommTransactionId
* \brief class to create and manage transaction IDs for communication with Vehicle Comm
*
* \author Florian Schindler
*/
class VehicleCommTransactionId : public TransactionId
{
 public:

    static const long kUndefinedVCTransactionId = -1;  //!< define an invalid value
    //!< should match default value in vc_common.hpp vc::MessageBase declaration.

    /*!
     * \fn     VehicleCommTransactionId
     * \brief  constructor
     * initialises all variables; creates unique transaction ID
     *
     * \author Florian Schindler
    */
    VehicleCommTransactionId ();

    /*!
     * \fn     VehicleCommTransactionId
     * \brief  constructor
     * initialises all variables; uses provided ID
     *
     * \author Florian Schindler
     *
     * \param[in]  id  transaction ID to be used
    */
    VehicleCommTransactionId (long id);  // initialise with given ID

    /*!
     * \fn     IsDefined
     * \brief  checkes whether the object contains a valid transaction ID
     *
     * \author Florian Schindler
     *
     * \return  true if valid
     * \return  false if invalid
    */
    bool IsDefined() const;

    /*!
     * \fn     GetId
     * \brief  returns the internal transaction ID
     *
     * \author Florian Schindler
     *
     * \return  transaction ID
    */
    long GetId() const;

    /*!
     * \brief Create a copy of this TransactionId and return
     *        a shared pointer to it.
     *
     * \return Shared ptr to copy of this transaction id.
     */
    std::shared_ptr<TransactionId> GetSharedCopy() const;

    static const long kUndefinedVehicleCommTransactionId_ = 0;  //!< define an invalid value
      //!< because vehicle_comm_client.hpp does not define it (it takes what we send) we define this

 protected:

    /*!
     * \fn     Compare
     * \brief  compares two instances of this object whether they have the same transaction ID
     *
     * \author Florian Schindler
     *
     * \param[in]  other  object to compare with
     *
     * \return  true if valid
     * \return  false if invalid
    */
    bool Compare (const TransactionId& other) const;

 private:

    long my_id_ = kUndefinedVehicleCommTransactionId_;    //!< our transaction ID
      //!< use the same definition like session_id in vehicle_comm_client.hpp

    static long last_used_id_;  //!< stores the last used transaction ID.
      // FlSc: I don't like the concept of generating random numbers, because from my point of view the value should
      // be unigue. Therefore we increase the last_used_id by one and have 4 billion values available, which are
      // hopefully sufficient for one lifecycle. Otherwise we need a more sophisticated bookkeeping

    static std::mutex vc_transaction_id_mutex_;  //!< to protect last_used_id
};

class IpCommandBrokerTransactionId : public TransactionId
{
 public:

    static const uint64_t kUndefinedTransactionId = 0;

    /*!
     * \brief Constructs a IpCommandBrokerTransactionId
     *        with a unique internal id.
     */
    IpCommandBrokerTransactionId ();

    /*!
     * \brief Constructs a IpCommandBrokerTransactionId
     *        with a specific value.
     * \param[in] id The id value.
     */
    IpCommandBrokerTransactionId (uint64_t id);

    /*!
     * \brief Constructs a IpCommandBrokerTransactionId
     *        with a specific value given as a string.
     *        note: error handling is done like in std::stoull defined, i.e. throw invalid_argument exception
     *
     * \param[in] transaction_id_str  the input string
     */
    IpCommandBrokerTransactionId(const std::string transaction_id_str);

    /*!
     * \brief Get the contained id.
     * \return The internal id of this IpCommandBrokerTransactionId.
     */
    uint64_t GetId();

    /*!
     * \fn     IsDefined
     * \brief  checkes whether the object contains a valid transaction ID
     *
     * \return  true if valid
     * \return  false if invalid
    */
    bool IsDefined() const;

    /*!
     * \brief Create a copy of this TransactionId and return
     *        a shared pointer to it.
     *
     * \return Shared ptr to copy of this transaction id.
     */
    std::shared_ptr<TransactionId> GetSharedCopy() const;

 protected:

    bool Compare (const TransactionId& other) const;

 private:

    uint64_t my_id = kUndefinedTransactionId;

    static uint64_t counter_;

    /*!
     * \brief Get a numeric id of same type as internal id representation.
     *        Guaranteed to be unique to ids previously returned by this
     *        function.
     * \return Numeric id of same type as internal id representation.
     */
    static uint64_t GetNextId();
};


class VpomTransactionId : public TransactionId
{
 public:

    static const uint64_t kUndefinedTransactionId = 0;

    /*!
     * \brief Constructs a VpomTransactionId
     *        with a unique internal id.
     */
    VpomTransactionId ();

    /*!
     * \brief Constructs a VpomTransactionId
     *        with a specific value.
     * \param[in] id The id value.
     */
    VpomTransactionId (uint64_t id);

    /*!
     * \brief Constructs a VpomTransactionId
     *        with a specific value given as a string.
     *        note: error handling is done like in std::stoull defined, i.e. throw invalid_argument exception
     *
     * \param[in] transaction_id_str  the input string
     */
    VpomTransactionId(const std::string transaction_id_str);

    /*!
     * \brief Get the contained id.
     * \return The internal id of this VpomTransactionId.
     */
    uint64_t GetId();

    /*!
     * \fn     IsDefined
     * \brief  checkes whether the object contains a valid transaction ID
     *
     * \return  true if valid
     * \return  false if invalid
    */
    bool IsDefined() const;

    /*!
     * \brief Create a copy of this TransactionId and return
     *        a shared pointer to it.
     *
     * \return Shared ptr to copy of this transaction id.
     */
    std::shared_ptr<TransactionId> GetSharedCopy() const;

 protected:

    bool Compare (const TransactionId& other) const;

 private:

    uint64_t my_id = kUndefinedTransactionId;

    static uint64_t counter_;

    /*!
     * \brief Get a numeric id of same type as internal id representation.
     *        Guaranteed to be unique to ids previously returned by this
     *        function.
     * \return Numeric id of same type as internal id representation.
     */
    static uint64_t GetNextId();
};

class TimeoutTransactionId : public TransactionId
{
 public:

    /*!
     * \brief Constructs a TimeoutTransactionId
     *        with a specific value.
     */
    TimeoutTransactionId ();

    /*!
     * \brief Get the contained id.
     * \return The internal id of this TimeoutTransactionId.
     */
    uint64_t GetId();

    /*!
     * \fn     IsDefined
     * \brief  checkes whether the object contains a valid transaction ID
     *
     * \return  true if valid
     * \return  false if invalid
    */
    bool IsDefined() const;

    /*!
     * \brief Create a copy of this TransactionId and return
     *        a shared pointer to it.
     *
     * \return Shared ptr to copy of this transaction id.
     */
    std::shared_ptr<TransactionId> GetSharedCopy() const;

 protected:

    /*!
     * \brief Compare id with another Transaction Id
     * \param[in] other transaction id to compare with
     * \return True if transaction ids are identical, false otherwise
     */
    bool Compare (const TransactionId& other) const;

 private:

    uint64_t my_id = 0;

    //static counter, used to generate new ids
    static uint64_t counter_;

    /*!
     * \brief Get a numeric id of same type as internal id representation.
     *        Guaranteed to be unique to ids previously returned by this
     *        function.
     * \return Numeric id of same type as internal id representation.
     */
    static uint64_t GetNextId();
};

class InternalSignalTransactionId : public TransactionId
{

public:

    /*!
     * \brief Constructs an InternalSignalTransactionId
     *        with id_ set to next available value.
     */
    InternalSignalTransactionId();

    /*!
     * \brief Get the contained id.
     * \return The internal id of this TimeoutTransactionId.
     */
    uint64_t GetId() const;

    /*!
     * \brief Create a copy of this TransactionId and return
     *        a shared pointer to it.
     *
     * \return Shared ptr to copy of this transaction id.
     */
    std::shared_ptr<TransactionId> GetSharedCopy() const;

protected:

    /*!
     * \brief Compare id with another Transaction Id
     * \param[in] other transaction id to compare with
     * \return True if transaction ids are identical, false otherwise
     */
    bool Compare(const TransactionId& other) const;

private:

    uint64_t id_ = 0;

    /*!
     * Static counter, used to generate new ids.
     */
    static std::atomic<uint64_t> counter_;
};


} // namespace fsm

#endif //VOC_FRAMEWORK_TRANSACTIONS_TRANSACTION_ID_H_

/** \}    end of addtogroup */
