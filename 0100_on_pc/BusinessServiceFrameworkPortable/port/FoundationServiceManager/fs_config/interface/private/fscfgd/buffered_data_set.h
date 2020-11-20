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
 *  \file     buffered_data_set.h
 *  \brief    Artefact holding couple of arbitary data assisting change-tracking.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_BUFFERED_DATA_SET_H_INC_
#define FSM_BUFFERED_DATA_SET_H_INC_

#include <map>
#include <set>
#include <algorithm>

#include <fscfg_common/utilities.h>

namespace fsm
{

template <typename T>
/**
 * \brief Artefact holding couple of arbitary data assisting change-tracking.
 */
class BufferedDataSet
{
private:
    T front_;   ///< Current state.
    T back_;    ///< Previous state.

public:
    BufferedDataSet()
     : front_(),
       back_()
    {
    }

    /**
     * \brief Getter for the current state.
     *
     * \return reference to the current state.
     */
    T& Get()
    {
        return front_;
    }

    /**
     * \brief Getter for the old state.
     *
     * \return reference to the old state.
     */
    T& GetBack()
    {
        return back_;
    }

    /**
     * \brief Equalize states.
     */
    void Update()
    {
        back_ = front_;
    }
};

template <typename K, typename T>
/**
 * \brief Checks if a key exists by search both old and currenty data sets.
 *
 * \param[in] data_set data set to search in.
 * \param[in] key key to search for.
 *
 * \return true if key found, false otherwise.
 */
bool ExistsInBufferedDataSet(BufferedDataSet<T>& data_set, K key)
{
    return data_set.Get().count(key) == 0 && data_set.GetBack().count(key);
}

template <typename K, typename T>
/**
 * \brief Provides the diff between of a data-set (diff between current and old).
 *
 * \param[in] data_map data map to extract diff from.
 * \param[out] new_set newly added set of data.
 * \param[out] deleted_set set of data that was deleted.
 */
void BufferedDataSetDiff(BufferedDataSet<T>& data_map, std::set<K>& new_set, std::set<K>& deleted_set)
{
    std::set<K> data_set_front;
    std::set<K> data_set_back;

    MapToKeySet(data_map.Get(), data_set_front);
    MapToKeySet(data_map.GetBack(), data_set_back);

    std::set_difference(data_set_back.begin(),
                        data_set_back.end(),
                        data_set_front.begin(),
                        data_set_front.end(),
                        std::inserter(deleted_set, deleted_set.end()));

    std::set_difference(data_set_front.begin(),
                        data_set_front.end(),
                        data_set_back.begin(),
                        data_set_back.end(),
                        std::inserter(new_set, new_set.end()));
}

}

#endif // FSM_BUFFERED_DATA_SET_H_INC_

/** \}    end of addtogroup */
