#pragma once
#include <functional>
#include <ossia_export.h>

namespace ossia
{
class OSSIA_EXPORT custom_state
{
    public:
        std::function<void()> func;
        void launch() const
        {
            if(func)
                func();
        }

        friend bool operator==(const custom_state& lhs, const custom_state& rhs)
        { return false; }
        friend bool operator!=(const custom_state& lhs, const custom_state& rhs)
        { return true; }
};
}
