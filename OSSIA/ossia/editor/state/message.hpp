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

#include <ossia/editor/value/value.hpp>
#include <ossia_export.h>
#include <utility>

namespace ossia
{
namespace net {
class address;
}
struct OSSIA_EXPORT Message
{
        std::reference_wrapper<ossia::net::address> address;
        ossia::value value;

        void launch() const;

        friend bool operator==(const Message& lhs, const Message& rhs)
        { return &lhs.address == &rhs.address && &lhs.value == &rhs.value; }

        friend bool operator!=(const Message& lhs, const Message& rhs)
        { return &lhs.address != &rhs.address || &lhs.value != &rhs.value; }
};


}
