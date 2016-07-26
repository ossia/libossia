#pragma once

namespace OSSIA
{    
struct StateExecutionVisitor
{
        template<typename T>
        void operator()(const T& m)
        { m.launch(); }
};
}
