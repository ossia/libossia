#pragma once
#include <ossia-max/src/ossia-max.hpp>

namespace ossia
{
namespace max
{
    
# pragma mark -
# pragma mark t_logger structure declaration
    
    struct t_logger
    {
        t_object m_object;
        std::shared_ptr<spdlog::logger> m_log;
    };
} // max namespace
} // ossia namespace

# pragma mark -
# pragma mark ossia_logger class declaration

extern "C"
{
    void* ossia_logger_new(t_symbol *s, long argc, t_atom *argv);
    void ossia_logger_in_anything(ossia::max::t_logger* x, t_symbol *s, long argc, t_atom *argv);
    void ossia_logger_free(ossia::max::t_logger* x);
}
