#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>

#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>

#include <ossia/network/local/local.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/minuit/minuit.hpp>

namespace py = pybind11;

class python_local_device
{
  ossia::net::generic_device m_device;
  ossia::net::local_protocol& m_local_protocol;

public:
  python_local_device(std::string name):
    m_device{std::make_unique<ossia::net::local_protocol>(), std::move(name)},
    m_local_protocol{static_cast<ossia::net::local_protocol&>(m_device.getProtocol())}
  {

  }

  bool asMinuitServer(std::string ip, int remoteport, int localport)
  {
    try {
      m_local_protocol.exposeTo(
            std::make_unique<ossia::net::minuit_protocol>(
              m_device.getName(), std::move(ip), localport, remoteport));
      return true;
    } catch(...) { }
    return false;
  }

  bool asOscServer(std::string ip, int remoteport, int localport)
  {
    try {
      m_local_protocol.exposeTo(std::make_unique<ossia::net::osc_protocol>(
                         std::move(ip), localport, remoteport));
      return true;
    } catch(...) { }
    return false;
  }

  ossia::net::node_base* addNode(const std::string& address)
  {
    return &ossia::net::find_or_create_node(m_device.getRootNode(), address);
  }

  ossia::net::node_base* findNode(const std::string& address)
  {
    return ossia::net::find_node(m_device.getRootNode(), address);
  }
};

PYBIND11_PLUGIN(ossia_python) {
    py::module m("ossia_python", "ossia");

    py::class_<python_local_device>(m, "LocalDevice")
        .def(py::init<std::string>())
        .def("asMinuitServer", &python_local_device::asMinuitServer)
        .def("asOscServer", &python_local_device::asOscServer)
        .def("addNode", &python_local_device::addNode, py::return_value_policy::reference)
        .def("findNode", &python_local_device::findNode, py::return_value_policy::reference)
        ;
    py::class_<ossia::net::node_base>(m, "Node")
        .def("getAddress", &ossia::net::node_base::getAddress, py::return_value_policy::reference)
        .def("createAddress", [] (ossia::net::node_base& node, int type) { return node.createAddress((ossia::val_type) type); }, py::return_value_policy::reference)
        ;

    py::class_<ossia::value>(m, "Value")
        .def(py::init<int>())
        .def(py::init<float>())
        .def(py::init<char>())
        .def(py::init<bool>())
        .def(py::init<const std::string&>())
        .def(py::init<const std::vector<ossia::value>&>())
        .def(py::init<std::array<float, 2>>())
        .def(py::init<std::array<float, 3>>())
        .def(py::init<std::array<float, 4>>())
        .def("valid", &ossia::value::valid)
        .def("reset", &ossia::value::reset)
        .def("get_int",    [] (const ossia::value& val) { return val.get<int>(); })
        .def("get_float",  [] (const ossia::value& val) { return val.get<float>(); })
        .def("get_char",   [] (const ossia::value& val) { return val.get<char>(); })
        .def("get_string", [] (const ossia::value& val) { return val.get<std::string>(); })
        .def("get_vec2f",  [] (const ossia::value& val) { return val.get<std::array<float, 2>>(); })
        .def("get_vec3f",  [] (const ossia::value& val) { return val.get<std::array<float, 3>>(); })
        .def("get_vec4f",  [] (const ossia::value& val) { return val.get<std::array<float, 4>>(); })
        //.def(py::self + py::self)
        //.def("__repr__", [] (const ossia::value& val) -> std::string { return value_to_pretty_string(val); });
        ;

    m.def("value_to_pretty_string", &ossia::value_to_pretty_string, "A function to print an ossia value");

    py::class_<ossia::net::address_base>(m, "Address")
        .def("pullValue", &ossia::net::address_base::pullValue)
        .def("cloneValue",  [] (ossia::net::address_base& addr) -> ossia::value { return addr.cloneValue(); })
        .def("pushValue", [] (ossia::net::address_base& addr, const ossia::value& v) { addr.pushValue(v); })
        .def("addCallback", [] (ossia::net::address_base& addr, ossia::value_callback clbk) { addr.add_callback(clbk); })
        ;

    return m.ptr();
}
