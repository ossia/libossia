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
class Message;
struct State;
class CustomState;
using StateElement = eggs::variant<Message, State, CustomState>;

struct OSSIA_EXPORT State
{
        void launch() const;

        std::vector<StateElement> children;
};

OSSIA_EXPORT bool operator==(const State& lhs, const State& rhs);
OSSIA_EXPORT bool operator!=(const State& lhs, const State& rhs);

/*! append each message of the state to the current state in order to eliminate address redundancy
 \param shared_ptr<State> the State to fill
 \param shared_ptr<StateElement> the StateElement to store */
void flattenAndFilter(State& state, const StateElement& element);
void flattenAndFilter(State& state, StateElement&& element);

OSSIA_EXPORT void launch(const StateElement&);
}
