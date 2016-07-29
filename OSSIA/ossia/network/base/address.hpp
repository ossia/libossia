#pragma once
#include <ossia/network/domain.hpp>
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/destination_index.hpp>
#include <ossia/network/common/address_properties.hpp>

#include <ossia_export.h>
#include <nano_signal_slot.hpp>
#include <functional>
#include <memory>

namespace ossia
{
class value;
namespace net
{
class Node;

using ValueCallback = std::function<void(const value&)>;

class OSSIA_EXPORT address :
    public callback_container<ValueCallback>
{
    public:
        address() = default;
        address(const address&) = delete;
        address(address&&) = delete;
        address& operator=(const address&) = delete;
        address& operator=(address&&) = delete;

        using callback_index = callback_container<ValueCallback>::iterator;
        virtual ~address();

        virtual const ossia::net::Node& getNode() const = 0;

        virtual void pullValue() = 0;
        virtual address & pushValue(const value&) = 0;
        virtual address & pushValue() = 0;

        virtual value cloneValue(destination_index = {}) const = 0;
        virtual address & setValue(const value&) = 0;

        virtual Type getValueType() const = 0;
        virtual address & setValueType(Type) = 0;

        virtual AccessMode getAccessMode() const = 0;
        virtual address & setAccessMode(AccessMode) = 0;

        virtual const Domain& getDomain() const = 0;
        virtual address & setDomain(const Domain&) = 0;

        virtual BoundingMode getBoundingMode() const = 0;
        virtual address & setBoundingMode(BoundingMode) = 0;

        virtual RepetitionFilter getRepetitionFilter() const = 0;
        virtual address & setRepetitionFilter(RepetitionFilter = RepetitionFilter::ON) = 0;

        virtual const std::string& getTextualAddress() const = 0;
};

/*!
 * \brief getAddressFromNode
 * \return the textual address of a node : aDevice:/an/address
 */
OSSIA_EXPORT std::string getAddressFromNode(const ossia::net::Node&);
}
}
