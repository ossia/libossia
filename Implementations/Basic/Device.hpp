#pragma once
#include "Node.hpp"

#include <ossia_export.h>


namespace OSSIA
{
namespace v2
{
class Protocol;

class OSSIA_EXPORT Device : public virtual Node
{

    public:
        Device() = default;
        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        virtual ~Device();

        virtual OSSIA::v2::Protocol& getProtocol() const = 0;
        virtual bool updateNamespace() = 0;
};
}
}
