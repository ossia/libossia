#pragma once
#include <functional>
#include <ossia_export.h>

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
}
