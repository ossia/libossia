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
    
    extern "C" void ossia_logger_setup(void);
    extern "C" void ossia_parameter_setup(void);
    
    class ossia_max
    {
    public:
        static ossia_max& instance();
        
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
    
    static std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol)
    {
        std::vector<std::string> tags;
        
        if (tags_symbol)
        {
            char* c = tags_symbol->s_name;
            std::string tag="";
            
            while (*c!='\0')
            {
                if (*c==' ')
                {
                    tags.push_back(tag);
                    tag = std::string("");
                }
                else tag += *c;
                
                c++;
            }
            tags.push_back(tag);
        }
        
        return tags;
    }
    
} // max namespace
} // ossia namespace
