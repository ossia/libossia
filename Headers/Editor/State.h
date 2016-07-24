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

#include "Misc/Container.h"
#include <ossia_export.h>
#include <eggs/variant.hpp>
namespace OSSIA
{
struct Message;
class State;
class CustomState;
using StateElement = eggs::variant<Message, State, CustomState>;

class OSSIA_EXPORT State
{
    public:
        friend bool operator==(const State& lhs, const State& rhs);
        friend bool operator!=(const State& lhs, const State& rhs);

        auto begin() { return children.begin(); }
        auto end() { return children.end(); }
        auto begin() const { return children.begin(); }
        auto end() const { return children.end(); }
        auto cbegin() const { return children.cbegin(); }
        auto cend() const { return children.cend(); }

        void launch() const;

        void add(const StateElement& e);
        void add(StateElement&& e);

        template<class Optional_T>
        auto add(Optional_T&& opt)
             -> decltype(typename Optional_T::value_type(), void())
        {
            if(opt)
                add(*std::forward<Optional_T>(opt));
        }

        void remove(const StateElement& e);

        void reserve(std::size_t);
        void clear();

    private:
        std::vector<StateElement> children;
};

inline auto begin(State& s) { return s.begin(); }
inline auto begin(const State& s) { return s.begin(); }
inline auto end(State& s) { return s.end(); }
inline auto end(const State& s) { return s.end(); }

/*! append each message of the state to the current state in order to eliminate address redundancy
 \param shared_ptr<State> the State to fill
 \param shared_ptr<StateElement> the StateElement to store */
void flattenAndFilter(State& state, const StateElement& element);
void flattenAndFilter(State& state, StateElement&& element);

OSSIA_EXPORT void launch(const StateElement&);
}
