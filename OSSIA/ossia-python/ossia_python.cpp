#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

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

  bool createOscQueryServer(int osc_port, int ws_port)
  {
    try {
      m_local_protocol.exposeTo(std::make_unique<ossia::oscquery::oscquery_server_protocol>(
              					osc_port, ws_port));
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

    py::object operator()(const ossia::Destination&) const { throw; }
    
    py::object operator()() { throw; }

};

PYBIND11_PLUGIN(ossia_python)
{
    py::module m("ossia_python", "ossia");

    py::class_<python_local_device>(m, "LocalDevice")
        .def(py::init<std::string>())
        .def("create_oscquery_server", &python_local_device::createOscQueryServer)
        .def("add_node", &python_local_device::addNode, py::return_value_policy::reference)
        .def("find_node", &python_local_device::findNode, py::return_value_policy::reference)
        ;

    py::class_<ossia::net::node_base>(m, "Node")
        .def("get_address", &ossia::net::node_base::getAddress, py::return_value_policy::reference)
        .def("create_address", [] (ossia::net::node_base& node, int type) { return node.createAddress((ossia::val_type) type); }, py::return_value_policy::reference)
        ;

    py::class_<ossia::net::address_base>(m, "Address")
    	.def("get_node", &ossia::net::address_base::getNode, py::return_value_policy::reference)
    	.def("get_value_type", &ossia::net::address_base::getValueType)
    	.def("set_value_type", &ossia::net::address_base::setValueType)
    	.def("get_access_mode", &ossia::net::address_base::getAccessMode)
    	.def("set_access_mode", &ossia::net::address_base::setAccessMode)
    	.def("get_bounding_mode", &ossia::net::address_base::getBoundingMode)
    	.def("set_bounding_mode", &ossia::net::address_base::setBoundingMode)
    	.def("get_domain", &ossia::net::address_base::getDomain)
    	.def("set_domain", &ossia::net::address_base::setDomain)
    	.def("get_repetition_filter", &ossia::net::address_base::getRepetitionFilter)
    	.def("set_repetition_filter", &ossia::net::address_base::setRepetitionFilter)
    	.def("get_unit", &ossia::net::address_base::getUnit)
    	.def("set_unit", &ossia::net::address_base::setUnit)
        .def("pull_value", &ossia::net::address_base::pullValue)
        .def("clone_value", [] (ossia::net::address_base& addr) -> ossia::value { return addr.cloneValue(); })
        .def("push_value", [] (ossia::net::address_base& addr, const ossia::value& v) { addr.pushValue(v); })
        .def("add_callback", [] (ossia::net::address_base& addr, ossia::value_callback clbk) { addr.add_callback(clbk); })
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
    	.value("Destination", ossia::val_type::DESTINATION)
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
   		//.def("get_min", &ossia::domain::get_min)
    	.def("set_min", &ossia::domain::set_min)
    	//.def("get_max", &ossia::domain::get_max)
    	.def("set_max", &ossia::domain::set_max)
   		;

    py::class_<ossia::value>(m, "Value")
    	.def(py::init<bool>())
        .def(py::init<int>())
        .def(py::init<float>())
        .def(py::init<char>())
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
        //.def(py::self + py::self)
        //.def("__repr__", [] (const ossia::value& val) -> std::string { return value_to_pretty_string(val); });
        ;

    return m.ptr();
}
