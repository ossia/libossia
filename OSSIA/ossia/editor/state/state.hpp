/*!
 * \file State.h
 *
 * \defgroup Editor
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>

#include <ossia/editor/state/state_element_fwd.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia_export.h>
namespace ossia
{
class OSSIA_EXPORT state
{
    public:
        friend bool operator==(const state& lhs, const state& rhs);
        friend bool operator!=(const state& lhs, const state& rhs);

        auto begin() { return children.begin(); }
        auto end() { return children.end(); }
        auto begin() const { return children.begin(); }
        auto end() const { return children.end(); }
        auto cbegin() const { return children.cbegin(); }
        auto cend() const { return children.cend(); }
        std::size_t size() const;

        void launch() const;

        void add(const state_element& e);
        void add(state_element&& e);

        template<class Optional_T>
        auto add(Optional_T&& opt)
             -> decltype(std::declval<typename Optional_T::value_type>(), void())
        {
            if(opt)
                add(*std::forward<Optional_T>(opt));
        }

        void remove(const state_element& e);

        void reserve(std::size_t);
        void clear();

    private:
        std::vector<state_element> children;
};

inline auto begin(state& s) { return s.begin(); }
inline auto begin(const state& s) { return s.begin(); }
inline auto end(state& s) { return s.end(); }
inline auto end(const state& s) { return s.end(); }

/*! append each message of the state to the current state in order to eliminate address redundancy
 \param std::shared_ptr<State> the State to fill
 \param std::shared_ptr<StateElement> the StateElement to store */
OSSIA_EXPORT void flattenAndFilter(state&, const state_element& element);
OSSIA_EXPORT void flattenAndFilter(state&, state_element&& element);
}
