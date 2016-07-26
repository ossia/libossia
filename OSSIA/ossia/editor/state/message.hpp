/*!
 * \file Message.h
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

#include <Editor/Value/Value.h>
#include <ossia_export.h>

namespace OSSIA
{
class Address;

struct OSSIA_EXPORT Message
{
        std::shared_ptr<Address> address;
        Value value;

        void launch() const;

        friend bool operator==(const Message& lhs, const Message& rhs)
        { return lhs.address == rhs.address && lhs.value == rhs.value; }

        friend bool operator!=(const Message& lhs, const Message& rhs)
        { return lhs.address != rhs.address || lhs.value != rhs.value; }
};


}
