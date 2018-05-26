#pragma once
#include <ossia/detail/config.hpp>
#include <ossia-c/ossia-c.h>
#include <ossia/detail/config.hpp>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <fmt/format.h>
#include <boost/container/flat_map.hpp>
#include <ossia-c/log/ossia_log.h>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/domain/domain_base.hpp>

struct ossia_protocol
{
  ossia_protocol(ossia::net::protocol_base* p) : protocol{p}
  {
  }
  ossia::net::protocol_base* protocol{};
};

struct ossia_device
{
  std::shared_ptr<ossia::net::device_base> device;
};

struct ossia_domain
{
  ossia::domain domain;
};

struct ossia_value_callback_index
{
  ossia::net::parameter_base::iterator it;
};

struct ossia_value
{
  ossia::value value;
};

inline auto convert(ossia_type t)
{
  return static_cast<ossia::val_type>(t);
}

inline auto convert(ossia_access_mode t)
{
  return static_cast<ossia::access_mode>(t);
}

inline auto convert(ossia_bounding_mode t)
{
  return static_cast<ossia::bounding_mode>(t);
}

inline auto convert(ossia::val_type t)
{
  return static_cast<ossia_type>(t);
}

inline auto convert(ossia::access_mode t)
{
  return static_cast<ossia_access_mode>(t);
}

inline auto convert(ossia::bounding_mode t)
{
  return static_cast<ossia_bounding_mode>(t);
}

inline ossia::net::parameter_base* convert_parameter(ossia_parameter_t v)
{
  return static_cast<ossia::net::parameter_base*>(v);
}

inline void* convert(ossia::net::parameter_base* v)
{
  return static_cast<void*>(v);
}
inline void* convert(const ossia::net::parameter_base* v)
{
  return static_cast<void*>(const_cast<ossia::net::parameter_base*>(v));
}

inline ossia::net::node_base* convert_node(ossia_node_t v)
{
  return static_cast<ossia::net::node_base*>(v);
}

inline ossia::net::device_base* convert_device(ossia_device_t v)
{
  return v->device.get();
}

inline void* convert(const ossia::net::node_base* v)
{
    return static_cast<void*>(const_cast<ossia::net::node_base*>(v));
}
inline void* convert(ossia::net::node_base* v)
{
  return static_cast<void*>(v);
}

inline auto convert(ossia::value&& v)
{
  return new ossia_value{std::move(v)};
}

inline auto convert(const ossia::value& v)
{
  return new ossia_value{v};
}

inline auto convert(const ossia::domain& v)
{
  return new ossia_domain{v};
}
inline auto convert(ossia::domain&& v)
{
  return new ossia_domain{std::move(v)};
}

template <typename Fun>
auto safe_function(const char name[], Fun f) -> decltype(f()) try
{
  // DEBUG_LOG_FMT("Entering %s", name);
  return f();
}
catch (const std::exception& e)
{
  auto str = fmt::format("{}: {}", name, e.what());
  ossia_log_error(str.c_str());
  return decltype(f())();
}
catch (...)
{
  auto str = fmt::format("{}: Exception caught", name);
  ossia_log_error(str.c_str());
  return decltype(f())();
}

inline char* copy_string(const std::string& str)
{
  const auto n = str.size();
  auto mbuffer = new char[n + 1];
  std::memcpy(mbuffer, str.data(), n);
  mbuffer[n] = 0;
  return mbuffer;
}

inline void copy_bytes(const std::string& str, char** ptr, size_t* sz)
{
  const auto n = str.size();
  *sz = n;
  *ptr = (char*)std::malloc(sizeof(char) * (n + 1));
  std::memcpy(*ptr, str.data(), n);
  (*ptr)[n] = 0;
}

struct node_cb {
    ossia_node_callback_t m_cb{};
    void* m_ctx{};
    void operator()(const ossia::net::node_base& node) {
        m_cb(m_ctx, convert(&node));
    }
};
struct address_cb {
    ossia_parameter_callback_t m_cb{};
    void* m_ctx{};
    void operator()(const ossia::net::parameter_base& addr) {
        m_cb(m_ctx, convert(&addr));
    }
};

struct global_devices
{
    boost::container::flat_map<std::string, ossia_device_t> devices;

    global_devices() = default;
    global_devices(const global_devices&) = delete;
    global_devices(global_devices&&) = delete;
    global_devices& operator=(const global_devices&) = delete;
    global_devices& operator=(global_devices&&) = delete;
};

global_devices& static_devices();
