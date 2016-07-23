/*!
 * \file StateElement.h
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
#include <ossia_export.h>
#include <Editor/State.h>
#include <Editor/Message.h>
#include <eggs/variant.hpp>
#include <functional>
namespace OSSIA
{
class OSSIA_EXPORT CustomState
{
    public:
        std::function<void()> func;
        void launch() const
        {
            if(func)
                func();
        }

        friend bool operator==(const CustomState& lhs, const CustomState& rhs)
        { return false; }
        friend bool operator!=(const CustomState& lhs, const CustomState& rhs)
        { return true; }
};


using StateElement = eggs::variant<Message, State, CustomState>;

struct StateExecutionVisitor
{
        template<typename T>
        void operator()(const T& m)
        { m.launch(); }
};

}
