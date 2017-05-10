namespace pybind11 { namespace py = pybind11; }

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>

namespace py = pybind11;

/**
 * @brief Local device class
 *
 * A local device is required to build any OSSIA network
 */
class ossia_local_device
{
    ossia::net::generic_device m_device;
    ossia::net::local_protocol& m_local_protocol;

public:

    /** Constructor
    \param std::string name of the local device */
    ossia_local_device(std::string name):
        m_device{std::make_unique<ossia::net::local_protocol>(), std::move(name)},
        m_local_protocol{static_cast<ossia::net::local_protocol&>(m_device.get_protocol())}
    {

    }

    /** Make the local device able to handle oscquery request
    \param int port where OSC requests have to be sent by any remote client to deal with the local device
    \param int port where WebSocket requests have to be sent by any remote client to deal with the local device
    \return bool */
    bool create_oscquery_server(int osc_port, int ws_port)
    {
        try {
            m_local_protocol.expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>(
                                          osc_port, ws_port));
            return true;
        } catch(std::exception& e) {
            ossia::logger().error("ossia_local_device: error when creating OSCQuery server: {}", e.what());
        } catch(...) {
            ossia::logger().error("ossia_local_device: error when creating OSCQuery server: {}");
        }
        return false;
    }

    ossia::net::node_base* add_node(const std::string& address)
    {
        return &ossia::net::find_or_create_node(m_device.get_root_node(), address);
    }

    ossia::net::node_base* find_node(const std::string& address)
    {
        return ossia::net::find_node(m_device.get_root_node(), address);
    }

    ossia::net::node_base* get_root_node()
    {
        return &m_device.get_root_node();
    }
};

/**
 * @brief OSCQuery device class
 *
 * An OSCQuery device is required to deal with a remote application using OSCQuery protocol 
 */
class ossia_oscquery_device
{
    ossia::net::generic_device m_device;
    ossia::oscquery::oscquery_mirror_protocol& m_oscquery_protocol;

public:

    ossia_oscquery_device(std::string name, std::string host, uint16_t local_osc_port):
        m_device{std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(host, local_osc_port), std::move(name)},
        m_oscquery_protocol{static_cast<ossia::oscquery::oscquery_mirror_protocol&>(m_device.get_protocol())}
    {

    }

    bool update()
    {
        return m_oscquery_protocol.update(m_device.get_root_node());
    }

    ossia::net::node_base* find_node(const std::string& address)
    {
        return ossia::net::find_node(m_device.get_root_node(), address);
    }

    ossia::net::node_base* get_root_node()
    {
        return &m_device.get_root_node();
    }
};

/**
 * @brief To cast python value into OSSIA value
 *
 */
struct to_python_value
{
    template<typename T>
    py::object operator()(const T& t) const
    {
        return py::cast(t);
    }

    py::object operator()(const std::vector<ossia::value>& v) const
    {
        std::vector<py::object> vec;

        for (auto i : v)
            vec.push_back(i.apply(to_python_value{}));

        return py::cast(vec);
    }

    template<int N>
    py::object operator()(const std::array<float, N>&) const { throw; }

    py::object operator()() { throw; }

};

// TODO : add explorations tools functions
// get_nodes
// get_params
// .. cf __init__ pyossia

// to get children of a node
PYBIND11_MAKE_OPAQUE(std::vector<ossia::net::node_base*>);

PYBIND11_PLUGIN(ossia_python)
{
    py::module m("ossia_python", "python binding of ossia library");

    py::class_<ossia_local_device>(m, "LocalDevice")
            .def(py::init<std::string>())
            .def("create_oscquery_server", &ossia_local_device::create_oscquery_server)
            .def("add_node", &ossia_local_device::add_node, py::return_value_policy::reference)
            .def("find_node", &ossia_local_device::find_node, py::return_value_policy::reference)
            .def("get_root_node", &ossia_local_device::get_root_node, py::return_value_policy::reference)
            ;

    py::class_<ossia_oscquery_device>(m, "OSCQueryDevice")
            .def(py::init<std::string, std::string, uint16_t>())
            .def("update", &ossia_oscquery_device::update)
            .def("find_node", &ossia_oscquery_device::find_node, py::return_value_policy::reference)
            .def("get_root_node", &ossia_oscquery_device::get_root_node, py::return_value_policy::reference)
            ;

    py::class_<std::vector<ossia::net::node_base*>>(m, "NodeVector")
            .def(py::init<>())
            .def("clear", &std::vector<ossia::net::node_base*>::clear)
            .def("pop_back", &std::vector<ossia::net::node_base*>::pop_back)
            .def("__len__", [](const std::vector<ossia::net::node_base*> &v) { return v.size(); })
            .def("__iter__", [](std::vector<ossia::net::node_base*> &v) {return py::make_iterator(v.begin(), v.end()); }, py::keep_alive<0, 1>()) // Keep vector alive while iterator is used
            ;

    py::class_<ossia::net::node_base>(m, "Node")
            .def("add_node", [] (ossia::net::node_base& node, const std::string& adrs) -> ossia::net::node_base& { return ossia::net::find_or_create_node(node, adrs); }, py::return_value_policy::reference)
            .def("get_address", &ossia::net::node_base::get_address, py::return_value_policy::reference)
            .def("create_address", [] (ossia::net::node_base& node, int type) { return node.create_address((ossia::val_type) type); }, py::return_value_policy::reference)
            .def("children", &ossia::net::node_base::children_copy)
            .def("__str__", [] (ossia::net::node_base& node) -> std::string { return ossia::net::osc_address_string(node); })
    ;

    py::class_<ossia::net::address_base>(m, "Address")
            .def("get_node", &ossia::net::address_base::getNode, py::return_value_policy::reference)
            .def("get_value_type", &ossia::net::address_base::get_value_type)
            .def("set_value_type", &ossia::net::address_base::set_value_type)
            .def("get_access_mode", &ossia::net::address_base::get_access)
            .def("set_access_mode", &ossia::net::address_base::set_access)
            .def("get_bounding_mode", &ossia::net::address_base::get_bounding)
            .def("set_bounding_mode", &ossia::net::address_base::set_bounding)
            .def("make_domain", [] (ossia::net::address_base &addr, const ossia::value& min, const ossia::value& max) { addr.set_domain(ossia::make_domain(min, max)); })
            .def("make_domain", [] (ossia::net::address_base &addr, const ossia::value& min, const ossia::value& max, const std::vector<ossia::value>& vals) {addr.set_domain(ossia::make_domain(min, max, vals)); })
            .def("have_domain", [] (ossia::net::address_base &addr) -> bool { return bool(addr.get_domain()); })
            .def("get_domain", &ossia::net::address_base::get_domain)
            .def("apply_domain", [] (ossia::net::address_base &addr) { addr.push_value(ossia::apply_domain(addr.get_domain(), addr.get_bounding(), addr.fetch_value())); })
            .def("get_repetition_filter", &ossia::net::address_base::get_repetition_filter)
            .def("set_repetition_filter", &ossia::net::address_base::set_repetition_filter)
            .def("get_unit", &ossia::net::address_base::get_unit)
            .def("set_unit", &ossia::net::address_base::set_unit)
            .def("pull_value", &ossia::net::address_base::pull_value)
            .def("clone_value", [] (ossia::net::address_base& addr) -> ossia::value { return addr.value(); })
            .def("fetch_value", [] (ossia::net::address_base& addr) -> ossia::value { return addr.fetch_value(); })
            .def("push_value", [] (ossia::net::address_base& addr, const ossia::value& v) { addr.push_value(v); })
            .def("add_callback", [] (ossia::net::address_base& addr, ossia::value_callback clbk) { addr.add_callback(clbk); })
            .def("__str__", [] (ossia::net::address_base& addr) -> std::string { return ossia::value_to_pretty_string(addr.value()); })
    ;

    py::enum_<ossia::val_type>(m, "ValueType", py::arithmetic())
            .value("Float", ossia::val_type::FLOAT)
            .value("Int", ossia::val_type::INT)
            .value("Vec2f", ossia::val_type::VEC2F)
            .value("Vec3f", ossia::val_type::VEC3F)
            .value("Vec4f", ossia::val_type::VEC4F)
            .value("Impulse", ossia::val_type::IMPULSE)
            .value("Bool", ossia::val_type::BOOL)
            .value("String", ossia::val_type::STRING)
            .value("Tuple", ossia::val_type::TUPLE)
            .value("Char", ossia::val_type::CHAR)
            .export_values();

    py::enum_<ossia::access_mode>(m, "AccessMode", py::arithmetic())
            .value("Get", ossia::access_mode::GET)
            .value("Set", ossia::access_mode::SET)
            .value("Bi", ossia::access_mode::BI)
            .export_values();

    py::enum_<ossia::bounding_mode>(m, "BoundingMode", py::arithmetic())
            .value("Free", ossia::bounding_mode::FREE)
            .value("Clip", ossia::bounding_mode::CLIP)
            .value("Wrap", ossia::bounding_mode::WRAP)
            .value("Fold", ossia::bounding_mode::FOLD)
            .value("Low", ossia::bounding_mode::LOW)
            .value("High", ossia::bounding_mode::HIGH)
            .export_values();

    py::enum_<ossia::repetition_filter>(m, "RepetitionFilter", py::arithmetic())
            .value("Off", ossia::repetition_filter::OFF)
            .value("On", ossia::repetition_filter::ON)
            .export_values();

    py::class_<ossia::domain>(m, "Domain")
            .def(py::init())
            .def("get_min", [] (ossia::domain& d) -> ossia::value { return ossia::get_min(d); })
            .def("get_max", [] (ossia::domain& d) -> ossia::value { return ossia::get_max(d); })
            ;

    py::class_<ossia::value>(m, "Value")
            .def(py::init<bool>())
            .def(py::init<int>())
            .def(py::init<float>())
            .def(py::init<const std::string&>())
            .def(py::init<const std::vector<ossia::value>&>())
            .def(py::init<std::array<float, 2>>())
            .def(py::init<std::array<float, 3>>())
            .def(py::init<std::array<float, 4>>())
            .def("valid", &ossia::value::valid)
            .def("reset", &ossia::value::reset)
            .def("get", [] (const ossia::value& val) { return val.apply(to_python_value{}); })
            .def("get_bool",   [] (const ossia::value& val) { return val.get<bool>(); })
            .def("get_int",    [] (const ossia::value& val) { return val.get<int>(); })
            .def("get_float",  [] (const ossia::value& val) { return val.get<float>(); })
            .def("get_char",   [] (const ossia::value& val) { return val.get<char>(); })
            .def("get_string", [] (const ossia::value& val) { return val.get<std::string>(); })
            .def("get_vec2f",  [] (const ossia::value& val) { return val.get<std::array<float, 2>>(); })
            .def("get_vec3f",  [] (const ossia::value& val) { return val.get<std::array<float, 3>>(); })
            .def("get_vec4f",  [] (const ossia::value& val) { return val.get<std::array<float, 4>>(); })
            ;

    return m.ptr();
}
