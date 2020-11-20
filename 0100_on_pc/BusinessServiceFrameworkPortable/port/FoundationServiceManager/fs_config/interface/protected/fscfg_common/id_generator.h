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
 *  \file     id_generator.h
 *  \brief    Unique Id generator.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_ID_GENERATOR_H_INC_
#define FSM_ID_GENERATOR_H_INC_

// TODO: The initial intent for implementation was to also handle the situation of overflow by using an allocation
// technique. For reference one can consult the technical document at
// https://www.thinkmind.org/download.php?articleid=computation_tools_2012_1_10_80006 and adapt the idea for this
// particular use-case.
// For the current use-case purposes I identified that we can compromise with the overflow problem - still offering
// the possibility to implement the idea mentioned in the above link (interface-wise, I mean).
namespace fsm
{

/**
 * \brief Numeric identifier generator.
 *
 * \tparam T numeric type.
 */
template <typename T>
class IdGenerator
{
private:
    T max_id_;

public:
    /**
     * \brief Constructor
     */
    IdGenerator()
        : max_id_(0)
    {
    }

    /**
     * \brief Allocates a new identifier.
     *
     * \return newly allocated identifier, 0 on failure.
     */
    T AllocateId()
    {
        // TODO: Implement using an allocation/de-allocation technique. If the overflow could be a problem.
        return ++max_id_;
    }

    /**
     * \brief Deallocates a previously allocated identifier.
     *
     * \param[in] id previously allocated identifier.
     */
    void DeallocateId(T id)
    {
        // TODO: Implement using an allocation/de-allocation technique. If the overflow could be a problem.
    }
};

}

#endif // FSM_ID_GENERATOR_H_INC_

/** \}    end of addtogroup */
