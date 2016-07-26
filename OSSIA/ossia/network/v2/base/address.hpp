#pragma once
#include <ossia/network/v2/domain.hpp>
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/destination_index.hpp>
#include <ossia/network/common/address_properties.hpp>

#include <ossia_export.h>

#include <functional>
#include <memory>

namespace OSSIA
{
class Value;
namespace v2
{
class Node;

using ValueCallback = std::function<void(const Value&)>;

class OSSIA_EXPORT Address : public CallbackContainer<ValueCallback>
{

    public:
        Address() = default;
        Address(const Address&) = delete;
        Address(Address&&) = delete;
        Address& operator=(const Address&) = delete;
        Address& operator=(Address&&) = delete;
        using iterator = typename CallbackContainer<ValueCallback>::iterator;
        virtual ~Address();

        virtual const OSSIA::v2::Node& getNode() const = 0;

        virtual void pullValue() = 0;
        virtual Address & pushValue(const Value&) = 0;
        virtual Address & pushValue() = 0;

        virtual Value cloneValue(DestinationIndex = {}) const = 0;
        virtual Address & setValue(const Value&) = 0;

        virtual Type getValueType() const = 0;
        virtual Address & setValueType(Type) = 0;

        virtual AccessMode getAccessMode() const = 0;
        virtual Address & setAccessMode(AccessMode) = 0;

        virtual const Domain& getDomain() const = 0;
        virtual Address & setDomain(const Domain&) = 0;

        virtual BoundingMode getBoundingMode() const = 0;
        virtual Address & setBoundingMode(BoundingMode) = 0;

        virtual bool getRepetitionFilter() const = 0;
        virtual Address & setRepetitionFilter(bool = true) = 0;

        virtual const std::string& getTextualAddress() const = 0;
};

/*!
 * \brief getAddressFromNode
 * \return the textual address of a node : aDevice:/an/address
 */
OSSIA_EXPORT std::string getAddressFromNode(const OSSIA::v2::Node&);
}
}
