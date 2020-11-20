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
 *  \file     binding_set.h
 *  \brief    Callback registry.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_BINDING_SET_H_INC_
#define FSM_BINDING_SET_H_INC_

#include <cstdint>

#include <memory>
#include <functional>
#include <map>
#include <mutex>

#include <fscfg/fscfg_types.h>

#include "id_generator.h"

namespace fsm
{

/**
 * \brief Callback binding registry.
 * \tparam T callback function prototype.
 */
template <typename T>
class BindingSet
{
public:
    ///! Generator identifier type.
    typedef std::uint32_t IdType;

    ///! Callback function type.
    typedef std::function<fscfg_ReturnCode (T)> FunctionType;

    ///! Identifier to function map type.
    typedef std::map<IdType, FunctionType> MapType;

private:
    MapType bindings_;                                  ///< Identifier to function mapping.
    std::shared_ptr<IdGenerator<IdType>> id_generator_; ///< Unique numeric identifier generator.
    std::shared_ptr<std::mutex> sync_;                  ///< Synchronization primitive.

public:

    /**
     * \brief BindingSet Default Constructor.
     */
    BindingSet()
        : bindings_(),
          id_generator_(std::make_shared<IdGenerator<IdType>>()),
          sync_(std::make_shared<std::mutex>())
    {

    }

    /**
     * \brief BindingSet Constructor with existing numeric identifier generator.
     */
    BindingSet(std::shared_ptr<IdGenerator<IdType>> id_generator)
        : bindings_(),
          id_generator_(id_generator),
          sync_(std::make_shared<std::mutex>())
    {

    }
    /**
     * \brief BindingSet numeric indentifier generator setter.
     *
     * \param[in] id_generator numeric identifier generator.
     */
    void SetGenerator(std::shared_ptr<IdGenerator<IdType>> id_generator)
    {
        if (id_generator)
        {
            id_generator_ = id_generator;
        }
    }

    /**
     * \brief Retrieves all bindings held by this registry.
     *
     * \return Bindings held by this registry.
     */
    MapType& GetBindings()
    {
        return bindings_;
    }

    IdType Register(FunctionType func)
    {
        IdType id = 0;

        if (id_generator_)
        {
            sync_->lock();

            id = id_generator_->AllocateId();
            bindings_[id] = func;

            sync_->unlock();
        }

        return id;
    }

    /**
     * \brief Unregister an existing binding.
     *
     * \param[in] id previously registered binding numeric identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    fscfg_ReturnCode Unregister(IdType id)
    {
        fscfg_ReturnCode rc = fscfg_kRcSuccess;
        typename MapType::iterator it;

        if (!id_generator_)
        {
            rc = fscfg_kRcBadState;
        }
        else if ((it = bindings_.find(id)) == bindings_.end())
        {
            rc = fscfg_kRcNotFound; // key doesn't exist.
        }
        else
        {
            bindings_.erase(id);
            id_generator_->DeallocateId(id);
        }

        return rc;
    }

    /**
     * \brief Deduces if a given binding id is held by the registry.
     *
     * \param[in] id previously registered binding numeric identifier.
     *
     * \return fscfg_kRcSuccess on success.
     */
    bool HasBindingId(IdType id)
    {
        bool has_id = false;

        if (bindings_.count(id))
        {
            has_id = true;
        }

        return has_id;
    }

    /**
     * \brief Retrieves the synchronization primitive.
     *
     * \return synchronization primitive.
     */
    std::shared_ptr<std::mutex> GetSync()
    {
        return sync_;
    };

};

} // namespace fsm

#endif // FSM_BINDING_SET_H_INC_

/** \}    end of addtogroup */
