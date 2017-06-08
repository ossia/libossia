#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/logger.hpp>

using namespace ossia::max;

# pragma mark -
# pragma mark library

// ossia-max library loading
extern "C"
void ext_main(void *r)
{
    ossia_logger_setup();
    ossia_parameter_setup();
    
    post("OSSIA library for Max is loaded");
}

// ossia-max library constructor
ossia_max::ossia_max()
{}

// ossia-max library instance
ossia_max& ossia_max::instance()
{
    static ossia_max library_instance;
    return library_instance;
}

# pragma mark -
# pragma mark registration

template<typename T> bool object_register(T *x)
{
    post("OSSIA library : object_register TODO");
    /*
    if (x->m_node) return true; // already registered
    if (x->m_dead) return false; // object will be removed soon
    
    int l;
    t_device *device = (t_device*) find_parent(&x->m_object,"ossia.device", 0, &l);
    t_client *client = (t_client*) find_parent(&x->m_object,"ossia.client", 0, &l);
    
    // first try to locate a ossia.device in the parent hierarchy...
    if (!device && !client)
        return false; // not ready to register : if there is no device, nothing could be registered
    
    t_model *model = nullptr;
    t_view *view = nullptr;
    int view_level=0, model_level=0;
    
    if (!x->m_absolute)
    {
        // then try to locate a parent view or model
        if (std::is_same<T,t_view>::value || std::is_same<T,t_remote>::value)
            view = find_parent_alive<t_view>(&x->m_obj,"ossia.view", 0, &view_level);
        else
            model = find_parent_alive<t_model>(&x->m_obj,"ossia.model", 0, &model_level);
    }
    
    ossia::net::node_base*  node = nullptr;
    
    if (view)
        node = view->m_node;
    else if (model)
        node = model->m_node;
    else if (client)
        node = client->m_node;
    else
        node = device->m_node;
    
    return x->register_node(node);
     */
    
    return false;
}

//template bool object_register<t_parameter> (t_parameter *x);
//template bool object_register<t_remote>(t_remote *x);
//template bool object_register<t_model> (t_model *x);
//template bool object_register<t_view>  (t_view *x);

template<typename T> void object_quarantining(T* x)
{
    if ( !object_isQuarantined<T>(x) ) x->quarantine().push_back(x);
}

//template void object_quarantining<t_parameter> (t_parameter  *x);
//template void object_quarantining<t_model> (t_model  *x);
//template void object_quarantining<t_remote>(t_remote *x);
//template void object_quarantining<t_view>  (t_view   *x);

template<typename T> void object_dequarantining(T* x)
{
    x->quarantine().erase(std::remove(x->quarantine().begin(), x->quarantine().end(), x), x->quarantine().end());
}

//template void object_dequarantining<t_parameter> (t_parameter  *x);
//template void object_dequarantining<t_model> (t_model  *x);
//template void object_dequarantining<t_remote>(t_remote *x);
//template void objct_dequarantining<t_view>  (t_view   *x);

template<typename T> bool object_isQuarantined(T* x)
{
    return ossia::contains(x->quarantine(),x);
}

//template bool object_isQuarantined<t_parameter> (t_parameter  *x);
//template bool object_isQuarantined<t_model> (t_model  *x);
//template bool object_isQuarantined<t_remote>(t_remote *x);
//template bool object_isQuarantined<t_view>  (t_view   *x);

