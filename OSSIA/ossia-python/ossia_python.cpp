// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
namespace pybind11
{
  namespace py = pybind11;
}

#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>

#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/midi/midi.hpp>

#include <ossia/network/common/network_logger.hpp>
#include <ossia/detail/logger.hpp>
#include <spdlog/spdlog.h>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/network/base/node_attributes.hpp>

#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>

#include <Python.h>

namespace py = pybind11;

namespace ossia {
  namespace python {

/**
 * @brief To cast python value into OSSIA value
 *
 */
struct to_python_value
{
  template <typename T>
  py::object operator()(const T& t) const
  {
    return py::cast(t);
  }

  py::object operator()(const std::vector<ossia::value>& v) const
  {
    std::vector<py::object> vec;
    vec.reserve(v.size());

    for (const auto& i : v)
      vec.push_back(i.apply(to_python_value{}));

    return py::cast(vec);
  }

  py::object operator()()
  {
    return py::none{};
  }
};

ossia::value from_python_value(PyObject* source)
{
  ossia::value returned_value;

  PyObject *tmp = nullptr;
  if (PyNumber_Check(source))
  {
    if (PyBool_Check(source))
      returned_value = (source == Py_True);
#if PY_MAJOR_VERSION < 3
    else if (PyInt_Check(source))
        returned_value = (int)PyInt_AsLong(source);
#endif
    else if (PyLong_Check(source))
      returned_value = (int)PyLong_AsLong(source);
    else if (PyFloat_Check(source))
      returned_value = (float)PyFloat_AsDouble(source);
  }
#if PY_MAJOR_VERSION >= 3
  else if (PyUnicode_Check(source))
    returned_value = (std::string)PyUnicode_AsUTF8(source);
#endif
  else if (PyBytes_Check(source))
    returned_value = (std::string)PyBytes_AsString(source);
  else if (PyList_Check(source))
  {
    std::vector<ossia::value> vec;
    int n = PyList_Size(source);
    vec.reserve(n);

    PyObject *iterator = PyObject_GetIter(source);
    PyObject *item;

    while ((item = PyIter_Next(iterator)))
    {
      vec.push_back(from_python_value(item));
      Py_DECREF(item);
    }

    returned_value = std::move(vec);
  }
  else if ((tmp = PyByteArray_FromObject(source)))
    returned_value = (std::string)PyByteArray_AsString(tmp);

  if (tmp)
    Py_DECREF(tmp);

  return returned_value;
}

} }

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
  ossia_local_device(std::string name)
      : m_device{std::make_unique<ossia::net::local_protocol>(),
                 std::move(name)}
      , m_local_protocol{
            static_cast<ossia::net::local_protocol&>(m_device.get_protocol())}
  {
  }


  operator ossia::net::generic_device&() { return m_device; }
  /** get local device name
  \return std::string */
  std::string get_name()
  {
    return m_device.get_name();
  }

  /** Make the local device able to handle oscquery request
  \param int port where OSC requests have to be sent by any remote client to
  deal with the local device
  \param int port where WebSocket requests have to be sent by any remote client
  to deal with the local device
  \param bool enable protocol logging
  \return bool */
  bool create_oscquery_server(int osc_port, int ws_port, bool log=false)
  {
    try
    {
      m_local_protocol.expose_to(
          std::make_unique<ossia::oscquery::oscquery_server_protocol>(
              osc_port, ws_port));

      if (log)
      {
        ossia::net::network_logger logger;

        logger.inbound_logger = spdlog::stderr_logger_mt("oscquery input");
        logger.inbound_logger->set_pattern("oscquery input: %v");
        logger.inbound_logger->set_level(spdlog::level::info);

        logger.outbound_logger = spdlog::stderr_logger_mt("oscquery output");
        logger.outbound_logger->set_pattern("oscquery output: %v");
        logger.outbound_logger->set_level(spdlog::level::info);

        // attach the logger to the OSCQuery Server protocol only
        for (const auto& p : m_local_protocol.get_protocols())
        {
          try
          {
            ossia::oscquery::oscquery_server_protocol& oscquery_server = dynamic_cast<ossia::oscquery::oscquery_server_protocol&> (*p);

            oscquery_server.set_logger(std::move(logger));
            break;
          }
          catch (std::exception& e)
          {
            continue;
          }
        }
      }

      return true;
    }
    catch (std::exception& e)
    {
      ossia::logger().error(
          "ossia_local_device: error when creating OSCQuery server: {}",
          e.what());
    }
    catch (...)
    {
      ossia::logger().error(
          "ossia_local_device: error when creating OSCQuery server: {}");
    }
    return false;
  }

  /** Make the local device able to handle osc request and emit osc message
  \param int port where osc messages have to be sent to be catch by a remote
  client to listen to the local device
  \param int port where OSC requests have to be sent by any remote client to
  deal with the local device
  \param bool enable protocol logging
  \return bool */
  bool create_osc_server(std::string ip, int remote_port, int local_port, bool log=false)
  {
    try
    {
      m_local_protocol.expose_to(
          std::make_unique<ossia::net::osc_protocol>(
              ip, remote_port, local_port, m_device.get_name()));

      if (log)
      {
        ossia::net::network_logger logger;

        logger.inbound_logger = spdlog::stderr_logger_mt("osc input");
        logger.inbound_logger->set_pattern("osc input: %v");
        logger.inbound_logger->set_level(spdlog::level::info);

        logger.outbound_logger = spdlog::stderr_logger_mt("osc output");
        logger.outbound_logger->set_pattern("osc output: %v");
        logger.outbound_logger->set_level(spdlog::level::info);

        // attach the logger to the OSC protocol only
        for (const auto& p : m_local_protocol.get_protocols())
        {
          try
          {
            ossia::net::osc_protocol& osc_server = dynamic_cast<ossia::net::osc_protocol&> (*p);

            osc_server.set_logger(std::move(logger));
            break;
          }
          catch (std::exception& e)
          {
            continue;
          }
        }
      }

      return true;
    }
    catch (std::exception& e)
    {
      ossia::logger().error(
          "ossia_local_device: error when creating OSC server: {}",
          e.what());
    }
    catch (...)
    {
      ossia::logger().error(
          "ossia_local_device: error when creating OSC server: {}");
    }
    return false;
  }


  bool create_minuit_server(std::string local_name, std::string ip, int remote_port, int local_port, bool log=false)
  {
    try
    {
      auto proto = std::make_unique<ossia::net::minuit_protocol>(
              local_name, ip, remote_port, local_port);

      if (log)
      {
        ossia::net::network_logger logger;

        logger.inbound_logger = spdlog::stderr_logger_mt("minuit input");
        logger.inbound_logger->set_pattern("minuit input: %v");
        logger.inbound_logger->set_level(spdlog::level::info);

        logger.outbound_logger = spdlog::stderr_logger_mt("minuit output");
        logger.outbound_logger->set_pattern("minuit output: %v");
        logger.outbound_logger->set_level(spdlog::level::info);

        proto->set_logger(std::move(logger));
      }

      m_local_protocol.expose_to(std::move(proto));
      return true;
    }
    catch (std::exception& e)
    {
      ossia::logger().error(
          "ossia_local_device: error when creating Minuit client: {}",
          e.what());
    }
    catch (...)
    {
      ossia::logger().error(
          "ossia_local_device: error when creating Minuit client: {}");
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
 * An OSCQuery device is required to deal with a remote application using
 * OSCQuery protocol
 */
class ossia_oscquery_device
{
  ossia::net::generic_device m_device;
  ossia::oscquery::oscquery_mirror_protocol& m_oscquery_protocol;

public:
  ossia_oscquery_device(
      std::string name, std::string host, uint16_t local_osc_port)
      : m_device{std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
                     host, local_osc_port),
                 std::move(name)}
      , m_oscquery_protocol{
            static_cast<ossia::oscquery::oscquery_mirror_protocol&>(
                m_device.get_protocol())}
  {
  }

  operator ossia::net::generic_device&() { return m_device; }

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
 * @brief Minuit device class
 *
 * A Minuit device is required to deal with a remote application using
 * Minuit protocol
 */
class ossia_minuit_device
{
  ossia::net::generic_device m_device;
  ossia::net::minuit_protocol& m_protocol;

public:
  ossia_minuit_device(
      std::string name, std::string host, uint16_t remote_port, uint16_t local_port)
      : m_device{std::make_unique<ossia::net::minuit_protocol>(
                     name, host, remote_port, local_port),
                 name}
      , m_protocol{
            static_cast<ossia::net::minuit_protocol&>(
                m_device.get_protocol())}
  {
  }

  operator ossia::net::generic_device&() { return m_device; }

  bool update()
  {
    return m_protocol.update(m_device.get_root_node());
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
 * @brief OSC device class
 *
 * An OSC device is required to deal with a remote application using
 * OSC protocol
 */
class ossia_osc_device
{
  ossia::net::generic_device m_device;
  ossia::net::osc_protocol& m_osc_protocol;

public:
  ossia_osc_device(
      std::string name, std::string ip, uint16_t remote_port, uint16_t local_port)
      : m_device{std::make_unique<ossia::net::osc_protocol>(
                     ip, remote_port, local_port),
                 std::move(name)}
      , m_osc_protocol{
            static_cast<ossia::net::osc_protocol&>(
                m_device.get_protocol())}
  {
  }

  operator ossia::net::generic_device&() { return m_device; }

  bool get_learning()
  {
    return m_osc_protocol.learning();
  }

  void set_learning(bool l)
  {
    m_osc_protocol.set_learning(l);
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
 * @brief MIDI device class
 *
 * A MIDI device is required to deal with a controller using
 * MIDI protocol
 */
std::vector<ossia::net::midi::midi_info> list_midi_devices()
{
  ossia::net::midi::midi_protocol midi_protocol{};
  return midi_protocol.scan();
}

class ossia_midi_device
{
  ossia::net::midi::midi_device m_device;
  ossia::net::midi::midi_protocol& m_protocol;

public:
    ossia_midi_device(std::string name, ossia::net::midi::midi_info d)
    : m_device{ std::make_unique<ossia::net::midi::midi_protocol>(d) },
    m_protocol{ static_cast<ossia::net::midi::midi_protocol&>(m_device.get_protocol()) }
  {
      m_device.set_name(name);
      m_device.update_namespace();
  }

  operator ossia::net::midi::midi_device&() { return m_device; }

  ossia::net::node_base* find_node(const std::string& address)
  {
    return ossia::net::find_node(m_device.get_root_node(), address);
  }

  ossia::net::node_base* get_root_node()
  {
    return &m_device.get_root_node();
  }
};

// TODO : add explorations tools functions
// get_nodes
// get_params
// .. cf __init__ pyossia

// to get children of a node
PYBIND11_MAKE_OPAQUE(std::vector<ossia::net::node_base*>);

PYBIND11_MODULE(ossia_python, m)
{
  m.doc() = "python binding of ossia library";

  py::class_<ossia_local_device>(m, "LocalDevice")
      .def(py::init<std::string>())
      .def_property_readonly(
          "name", &ossia_local_device::get_name,
          py::return_value_policy::reference)
      .def_property_readonly(
          "root_node", &ossia_local_device::get_root_node,
          py::return_value_policy::reference)
      .def(
          "create_oscquery_server",
          &ossia_local_device::create_oscquery_server)
      .def(
          "create_osc_server",
          &ossia_local_device::create_osc_server)
      .def(
          "create_minuit_server",
          &ossia_local_device::create_minuit_server)
      .def(
          "add_node", &ossia_local_device::add_node,
          py::return_value_policy::reference)
      .def(
          "find_node", &ossia_local_device::find_node,
          py::return_value_policy::reference)
      ;

  py::class_<ossia_minuit_device>(m, "MinuitDevice")
      .def(py::init<std::string, std::string, uint16_t, uint16_t>())
      .def("update", &ossia_minuit_device::update)
      .def(
          "find_node", &ossia_minuit_device::find_node,
          py::return_value_policy::reference)
      .def_property_readonly(
          "root_node", &ossia_minuit_device::get_root_node,
          py::return_value_policy::reference);

  py::class_<ossia_oscquery_device>(m, "OSCQueryDevice")
      .def(py::init<std::string, std::string, uint16_t>())
      .def("update", &ossia_oscquery_device::update)
      .def(
          "find_node", &ossia_oscquery_device::find_node,
          py::return_value_policy::reference)
      .def_property_readonly(
          "root_node", &ossia_oscquery_device::get_root_node,
          py::return_value_policy::reference);

  py::class_<ossia::net::oscquery_connection_data>(m, "OSCQueryConnectionData")
      .def_readwrite("name", &ossia::net::oscquery_connection_data::name)
      .def_readwrite("host", &ossia::net::oscquery_connection_data::host)
      .def_readwrite("port", &ossia::net::oscquery_connection_data::port);

  m.def("list_oscquery_devices", &ossia::net::list_oscquery_devices);

  py::class_<ossia_osc_device>(m, "OSCDevice")
      .def(py::init<std::string, std::string, uint16_t, uint16_t>())
      .def_property("learning", &ossia_osc_device::get_learning,
          &ossia_osc_device::set_learning)
      .def(
          "add_node", &ossia_osc_device::add_node,
          py::return_value_policy::reference)
      .def(
          "find_node", &ossia_osc_device::find_node,
          py::return_value_policy::reference)
      .def_property_readonly(
          "root_node", &ossia_osc_device::get_root_node,
          py::return_value_policy::reference);

  m.def("list_midi_devices", &list_midi_devices);

  py::class_<ossia_midi_device>(m, "MidiDevice")
      .def(py::init<std::string, ossia::net::midi::midi_info>())
      .def("find_node", &ossia_midi_device::find_node,
          py::return_value_policy::reference)
      .def_property_readonly(
          "root_node", &ossia_midi_device::get_root_node,
          py::return_value_policy::reference);

  py::class_<ossia::net::midi::midi_info>(m, "MidiInfo")
      .def(py::init())
      .def_readonly("type", &ossia::net::midi::midi_info::type)
      .def_readonly("device", &ossia::net::midi::midi_info::device)
      .def_readonly("port", &ossia::net::midi::midi_info::port);

  py::enum_<ossia::net::midi::midi_info::Type>(m, "MidiDeviceType", py::arithmetic())
      .value("RemoteInput", ossia::net::midi::midi_info::Type::RemoteInput)
      .value("RemoteOutput", ossia::net::midi::midi_info::Type::RemoteOutput)
      .export_values();

  py::class_<std::vector<ossia::net::node_base*>>(m, "NodeVector")
      .def(py::init<>())
      .def("clear", &std::vector<ossia::net::node_base*>::clear)
      .def("pop_back", &std::vector<ossia::net::node_base*>::pop_back)
      .def(
          "__len__",
          [](const std::vector<ossia::net::node_base*>& v) {
            return v.size();
          })
      .def(
          "__iter__",
          [](std::vector<ossia::net::node_base*>& v) {
            return py::make_iterator(v.begin(), v.end());
          },
          py::keep_alive<0, 1>()) // Keep vector alive while iterator is used
      ;

  py::class_<ossia::net::node_base>(m, "Node")
      .def_property_readonly(
          "parameter", &ossia::net::node_base::get_parameter,
          py::return_value_policy::reference)
      .def_property("description", 
        [](ossia::net::node_base& node) -> ossia::net::description {
          ossia::net::description empty{};
          return ossia::net::get_description(node).value_or(empty);
        },
        [](ossia::net::node_base& node, const ossia::net::description v) {
          ossia::net::set_description(node, v);
        })
      .def_property("tags", 
        [](ossia::net::node_base& node) -> ossia::net::tags {
          ossia::net::tags empty{};
          return ossia::net::get_tags(node).value_or(empty);
        },
        [](ossia::net::node_base& node, const ossia::net::tags v) {
          ossia::net::set_tags(node, v);
        })
      .def_property("priority", 
        [](ossia::net::node_base& node) -> ossia::net::priority {
          ossia::net::priority empty{};
          return ossia::net::get_priority(node).value_or(empty);
        },
        [](ossia::net::node_base& node, const ossia::net::priority v) {
          ossia::net::set_priority(node, v);
        })
      .def_property("refresh_rate", 
        [](ossia::net::node_base& node) -> ossia::net::refresh_rate {
          ossia::net::refresh_rate empty{};
          return ossia::net::get_refresh_rate(node).value_or(empty);
        },
        [](ossia::net::node_base& node, const ossia::net::refresh_rate v) {
          ossia::net::set_refresh_rate(node, v);
        })
      .def_property("value_step_size", 
        [](ossia::net::node_base& node) -> ossia::net::value_step_size {
          ossia::net::value_step_size empty{};
          return ossia::net::get_value_step_size(node).value_or(empty);
        },
        [](ossia::net::node_base& node, const ossia::net::value_step_size v) {
          ossia::net::set_value_step_size(node, v);
        })
      .def_property("default_value",
          [](ossia::net::node_base& node) -> py::object {
            ossia::value empty{};
            return ossia::net::get_default_value(node).value_or(empty).apply(ossia::python::to_python_value{});
          },
          [](ossia::net::node_base& node, const py::object& v) {
            ossia::net::set_default_value(node, ossia::python::from_python_value(v.ptr()));
          })
      .def_property("extended_type", 
        [](ossia::net::node_base& node) -> ossia::extended_type {
          ossia::extended_type empty{};
          return ossia::net::get_extended_type(node).value_or(empty);
        },
        [](ossia::net::node_base& node, const ossia::extended_type v) {
          ossia::net::set_extended_type(node, v);
        })
      .def_property("instance_bounds", 
        [](ossia::net::node_base& node) -> ossia::net::instance_bounds {
          ossia::net::instance_bounds empty{};
          return ossia::net::get_instance_bounds(node).value_or(empty);
        },
        [](ossia::net::node_base& node, const ossia::net::instance_bounds v) {
          ossia::net::set_instance_bounds(node, v);
        })
      .def_property_readonly("zombie", 
        [](ossia::net::node_base& node) -> ossia::net::zombie {
          return ossia::net::get_zombie(node);
        })
      .def_property("critical", 
        [](ossia::net::node_base& node) -> ossia::net::critical {
          return ossia::net::get_critical(node);
        },
        [](ossia::net::node_base& node, const ossia::net::critical v) {
          ossia::net::set_critical(node, v);
        })
      .def_property("disabled", 
        [](ossia::net::node_base& node) -> ossia::net::disabled {
          return ossia::net::get_disabled(node);
        },
        [](ossia::net::node_base& node, const ossia::net::disabled v) {
          ossia::net::set_disabled(node, v);
        })
      .def_property("hidden", 
        [](ossia::net::node_base& node) -> ossia::net::hidden {
          return ossia::net::get_hidden(node);
        },
        [](ossia::net::node_base& node, const ossia::net::hidden v) {
          ossia::net::set_hidden(node, v);
        })
      .def_property("muted", 
        [](ossia::net::node_base& node) -> ossia::net::muted {
          return ossia::net::get_muted(node);
        },
        [](ossia::net::node_base& node, const ossia::net::muted v) {
          ossia::net::set_muted(node, v);
        })
      .def("add_node",
          [](ossia::net::node_base& node,
             const std::string& adrs) -> ossia::net::node_base& {
            return ossia::net::find_or_create_node(node, adrs);
          },
          py::return_value_policy::reference)
      .def("create_parameter",
          [](ossia::net::node_base& node, int type) {
            return node.create_parameter((ossia::val_type)type);
          },
          py::return_value_policy::reference)
      .def("children", &ossia::net::node_base::children_copy)
      .def("__str__", [](ossia::net::node_base& node) -> std::string {
        return ossia::net::osc_parameter_string(node);
      });

  py::class_<ossia::net::parameter_base>(m, "Parameter")
      .def_property_readonly(
          "node", &ossia::net::parameter_base::get_node,
          py::return_value_policy::reference)
      .def_property_readonly(
          "callback_count", [](ossia::net::parameter_base& addr) -> int {
            return addr.callback_count();
          })
      .def_property(
          "value",
          [](ossia::net::parameter_base& addr) -> py::object {
            return addr.fetch_value().apply(ossia::python::to_python_value{});
          },
          [](ossia::net::parameter_base& addr, const py::object& v) {
            addr.push_value(ossia::python::from_python_value(v.ptr()));
          })
      .def_property(
          "value_type", &ossia::net::parameter_base::get_value_type,
          &ossia::net::parameter_base::set_value_type)
      .def_property(
          "access_mode", &ossia::net::parameter_base::get_access,
          &ossia::net::parameter_base::set_access)
      .def_property(
          "bounding_mode", &ossia::net::parameter_base::get_bounding,
          &ossia::net::parameter_base::set_bounding)
      .def_property(
          "repetition_filter",
          &ossia::net::parameter_base::get_repetition_filter,
          &ossia::net::parameter_base::set_repetition_filter)
      .def_property(
          "unit",
          [](ossia::net::parameter_base& addr) -> std::string { 
            return ossia::get_pretty_unit_text(addr.get_unit()); },
          [](ossia::net::parameter_base& addr, std::string u) {
            addr.set_unit(ossia::parse_pretty_unit(u));
          })
      .def_property_readonly(
          "domain", &ossia::net::parameter_base::get_domain,
          py::return_value_policy::reference)
      .def(
          "have_domain",
          [](ossia::net::parameter_base& addr) -> bool {
            return bool(addr.get_domain());
          })
      .def(
          "make_domain",
          [](ossia::net::parameter_base& addr, const py::object& min, const py::object& max) {
            addr.set_domain(
                  ossia::make_domain(
                    ossia::python::from_python_value(min.ptr())
                  , ossia::python::from_python_value(max.ptr())));
          })
      .def(
          "make_domain",
          [](ossia::net::parameter_base& addr, const std::vector<py::object>& values) {
            auto dom = ossia::init_domain(addr.get_value_type());
            
            std::vector<ossia::value> vec;
            vec.reserve(values.size());

            for (auto& v : values)
              vec.push_back(ossia::python::from_python_value(v.ptr()));

            ossia::set_values(dom, vec);
            addr.set_domain(dom);
          })
      .def(
          "apply_domain",
          [](ossia::net::parameter_base& addr) {
            addr.push_value(ossia::apply_domain(
                addr.get_domain(), addr.get_bounding(), addr.fetch_value()));
          })
      .def("pull_value", &ossia::net::parameter_base::pull_value)
      .def(
          "clone_value",
          [](ossia::net::parameter_base& addr) -> py::object {
            return addr.value().apply(ossia::python::to_python_value{});
          })
      .def(
          "fetch_value",
          [](ossia::net::parameter_base& addr) -> py::object {
            return addr.fetch_value().apply(ossia::python::to_python_value{});
          })
      .def(
          "push_value", [](ossia::net::parameter_base& addr,
                           const py::object& v) { addr.push_value(ossia::python::from_python_value(v.ptr())); })
      .def(
          "add_callback",
          [](ossia::net::parameter_base& addr,
             std::function<void(const py::object&)> clbk) {
            addr.add_callback([=] (const auto& val) {
              clbk(val.apply(ossia::python::to_python_value{}));
            });
          })
      .def(
         "add_callback_param",
          [](ossia::net::parameter_base& addr,
             std::function<void(ossia::net::node_base&, const py::object&)> clbk) {
             addr.add_callback([clbk,&addr] (const ossia::value& val) {
               clbk(addr.get_node(), val.apply(ossia::python::to_python_value{}));
             });
          })
      .def("__str__", [](ossia::net::parameter_base& addr) -> std::string {
        return ossia::value_to_pretty_string(addr.value());
      });

  py::enum_<ossia::val_type>(m, "ValueType", py::arithmetic())
      .value("Float", ossia::val_type::FLOAT)
      .value("Int", ossia::val_type::INT)
      .value("Vec2f", ossia::val_type::VEC2F)
      .value("Vec3f", ossia::val_type::VEC3F)
      .value("Vec4f", ossia::val_type::VEC4F)
      .value("Impulse", ossia::val_type::IMPULSE)
      .value("Bool", ossia::val_type::BOOL)
      .value("String", ossia::val_type::STRING)
      .value("List", ossia::val_type::LIST)
      .value("Char", ossia::val_type::CHAR)
      .export_values();

  py::enum_<ossia::access_mode>(m, "AccessMode", py::arithmetic())
      .value("Bi", ossia::access_mode::BI)
      .value("Get", ossia::access_mode::GET)
      .value("Set", ossia::access_mode::SET)
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
      .def_property(
          "min",
          [](ossia::domain& d) -> py::object { return ossia::get_min(d).apply(ossia::python::to_python_value{}); },
          [](ossia::domain& d, const py::object& v) {
            ossia::set_min(d, ossia::python::from_python_value(v.ptr()));
          })
      .def_property(
          "max",
          [](ossia::domain& d) -> py::object { return ossia::get_max(d).apply(ossia::python::to_python_value{}); },
          [](ossia::domain& d, const py::object& v) {
            ossia::set_max(d, ossia::python::from_python_value(v.ptr()));
          });

  py::class_<ossia::net::instance_bounds>(m, "InstanceBounds")
      .def(py::init<int32_t, int32_t>())
      .def_readwrite("min", &ossia::net::instance_bounds::min_instances)
      .def_readwrite("max", &ossia::net::instance_bounds::max_instances);

  py::class_<ossia::message_queue>(m, "MessageQueue")
      .def(py::init<ossia_local_device&>())
      .def(py::init<ossia_osc_device&>())
      .def(py::init<ossia_oscquery_device&>())
      .def(py::init<ossia_minuit_device&>())
      .def(py::init<ossia_midi_device&>())
      .def("register", [] (ossia::message_queue& mq, ossia::net::parameter_base& p) {
    mq.reg(p);
  })
      .def("unregister", [] (ossia::message_queue& mq, ossia::net::parameter_base& p) {
    mq.unreg(p);
  })
      .def("pop", [] (ossia::message_queue& mq) -> py::object {
     ossia::received_value v;
     bool res = mq.try_dequeue(v);
     if(res)
     {
       return py::make_tuple(py::cast(v.address), v.value.apply(ossia::python::to_python_value{}));
     }
     return py::none{};
  });

  py::class_<ossia::global_message_queue>(m, "GlobalMessageQueue")
      .def(py::init<ossia_local_device&>())
      .def(py::init<ossia_osc_device&>())
      .def(py::init<ossia_oscquery_device&>())
      .def(py::init<ossia_minuit_device&>())
      .def(py::init<ossia_midi_device&>())
      .def("pop", [] (ossia::global_message_queue& mq) -> py::object {
     ossia::received_value v;
     bool res = mq.try_dequeue(v);
     if(res)
     {
       return py::make_tuple(py::cast(v.address), v.value.apply(ossia::python::to_python_value{}));
     }
     return py::none{};
  });
}
