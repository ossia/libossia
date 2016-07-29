#pragma once

namespace ossia
{    
struct StateExecutionVisitor
{
        template<typename T>
        void operator()(const T& m)
        { m.launch(); }
};
}
