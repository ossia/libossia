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
    extern "C" void ossia_device_setup(void);
    extern "C" void ossia_logger_setup(void);
    extern "C" void ossia_model_setup(void);
    extern "C" void ossia_parameter_setup(void);
    
    class ossia_max
    {
    public:
        static ossia_max& instance();
        
        t_class* ossia_client_class{};
        t_class* ossia_device_class{};
        t_class* ossia_parameter_class{};
        t_class* ossia_model_class{};
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
    
    /**
     * @brief register_quarantinized Try to register all quarantinized objects
     */
    void register_quarantinized();
    
    /**
     * @brief             Find the first instance of classname beside or above (in a parent patcher) context.
     * @details           The function iterate all objects at the same level or above x and return the first instance of classname found.
     * @param x           The object around which to search.
     * @param classname   The class name of the object we are looking for.
     * @param start_level Level above current object where to start. 0 for current patcher, 1 start searching in parent canvas.
     * @param level       Return level of the found object
     * @return The instance of the parent object if exists. Otherwise returns nullptr.
     */
    t_object* find_parent(t_object* x, t_symbol* classname, int start_level, int* level);
    
    /** 
     * @brief             Convenient method to get the patcher easily
     */
    t_object *get_patcher(t_object *obj);
    
    std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol);
    
} // max namespace
} // ossia namespace
