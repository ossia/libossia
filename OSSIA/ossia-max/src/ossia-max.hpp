#pragma once
#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post

#include <ossia/ossia.hpp>

namespace ossia
{
namespace max
{
    
# pragma mark -
# pragma mark Library
    
    extern "C" void ossia_client_setup(void);
    extern "C" void ossia_logger_setup(void);
    extern "C" void ossia_parameter_setup(void);
    
    class ossia_max
    {
    public:
        static ossia_max& instance();
        
        t_class* ossia_client_class{};
        t_class* ossia_parameter_class{};
        t_class* ossia_logger_class{};
        
    private:
        ossia_max();
    };
    
# pragma mark -
# pragma mark Templates
    
    // we can't have virtual methods with C linkage so we need a bunch a template instead...
    
    template<typename T> extern bool object_register(T*);
    
    template<typename T> extern std::string object_path(T*);
    
    template<typename T> extern void object_quarantining(T*);
    
    template<typename T> extern void object_dequarantining(T*);
    
    template<typename T> extern bool object_is_quarantined(T*);
    
    template<typename T> extern void object_dump(T*);
    
# pragma mark -
# pragma mark Utilities
    
    std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol);
    
} // max namespace
} // ossia namespace
