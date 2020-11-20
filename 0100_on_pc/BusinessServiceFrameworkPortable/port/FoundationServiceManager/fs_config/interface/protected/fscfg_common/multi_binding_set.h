/**
 * Copyright (C) 2017, 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     multi_binding_set.h
 *  \brief    Container of Callback registries.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup fsm
 *  \{
 */

#ifndef FSM_MULTI_BINDING_SET_H_INC_
#define FSM_MULTI_BINDING_SET_H_INC_

#include <tuple>

#include "binding_set.h"

namespace fsm
{

// artefacts necessary to construct for_each in std::tuple (compile-time sequence generator).
namespace detail
{
    template<int... Is>
    struct seq { };

    template<int N, int... Is>
    struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

    template<int... Is>
    struct gen_seq<0, Is...> : seq<Is...> { };

    template<typename T, typename F, int... Is>
    void for_each(T&& t, F f, seq<Is...>)
    {
        auto l = { (f(std::get<Is>(t)), 0)... };
        (void)(l);
    }

    template<typename... Ts, typename F>
    void for_each_in_tuple(std::tuple<Ts...>& t, F f)
    {
        detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
    }
}

/**
 * \brief Container of callback registries.
 * \tparam Types variadic sequence of callback registries.
 */
template <class... Types>
struct MultiBindingSet
{
    typedef IdGenerator<std::uint32_t> GeneratorType;
    std::shared_ptr<GeneratorType> id_generator_;           ///< numeric identifier generator.
    std::tuple<BindingSet<Types>...> tup_;                  ///< nuple of callback registries.

    /**
     * \brief numeric id generator nuple setter functor.
     */
    struct SetupGeneratorFunctor
    {
         std::shared_ptr<GeneratorType> id_generator_;

         SetupGeneratorFunctor(std::shared_ptr<GeneratorType> id_generator)
             : id_generator_(id_generator)
         {
         }

        template <typename T>
        void operator () (T& t)
        {
            t.SetGenerator(id_generator_);
        }
    };

    /**
     * \brief numeric id generator nuple unbind functor.
     *
     * Functor will run for all BindingSets in the tuple, so the functor logic applies to all BindingSets within the tuple.
     */
    struct UnbindFunctor
    {
         std::uint32_t id;      ///< ID that must be unbound.
         fscfg_ReturnCode& rc;  ///< Output return code.
         bool found_id;         ///< true if we didn't yet find a BindingSet that had the binding id.

         UnbindFunctor(std::uint32_t id, fscfg_ReturnCode& rc)
             : id(id),
               rc(rc),
               found_id(false)
         {
         }

        template <typename T>
        void operator () (T& t)
        {
            // As the numeric id space is shared across multiple binding sets, we need to try to unregister the id
            // until we find a binding set that did manage to do so. If no binding set was able to unregister the given id,
            // than we report an error.
            if (!found_id)
            {
                fscfg_ReturnCode unregister_rc = t.Unregister(id);

                if (unregister_rc == fscfg_kRcSuccess)
                {
                    rc = unregister_rc;
                    found_id = true;
                }
            }
        }
    };

    /**
     * \brief MultiBindingSet Constructor.
     */
    MultiBindingSet()
        : id_generator_(std::make_shared<GeneratorType>()),
          tup_()
    {
        SetGenerator(id_generator_);
    }

    /**
     * \brief MultiBindingSet Constructor with existing numeric identifier generator.
     *
     * \param[in] id_generator existing numeric identifier generator.
     */
    MultiBindingSet(std::shared_ptr<GeneratorType> id_generator)
        : id_generator_(),
          tup_()
    {
        SetGenerator(id_generator);
    }

    /**
     * \brief Dispatches a given generator to all binding sets.
     *
     * \param[in] id_generator existing numeric identifier generator.
     */
    void SetGenerator(std::shared_ptr<GeneratorType> id_generator)
    {
        id_generator_ = id_generator;
        detail::for_each_in_tuple(tup_, SetupGeneratorFunctor(id_generator_));
    }

    /**
     * \brief Unbind a an identifier from the first binding set that has it.
     * \param[in] id previously registered identifier.
     *
     * \return fscfg_kRcSuccess on success,\n
     *         fscfg_kRcNotFound if given id was not found.
     */
    fscfg_ReturnCode Unbind(std::uint32_t id)
    {
        fscfg_ReturnCode rc = fscfg_kRcNotFound;
        detail::for_each_in_tuple(tup_, UnbindFunctor(id, rc));

        return rc;
    }
};

} // namespace fsm

#endif // FSM_MULTI_BINDING_SET_H_INC_
