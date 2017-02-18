#pragma once
#include <ossia-c/ossia-c.h>
#include <ossia/ossia.hpp>

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <ossia-c/log/tinyformat.h>
#include <ossia-c/log/ossia_log.h>

template<typename Str, typename... Args>
void DEBUG_LOG_FMT(Str fmt, Args... args)
{
  auto str = tfm::format(fmt, args...);
  ossia_log_error(str.c_str());
}
struct ossia_protocol
{
    ossia_protocol(ossia::net::protocol_base* p): protocol{p} { }
    ossia::net::protocol_base* protocol{};
};

struct ossia_device
{
    std::unique_ptr<ossia::net::device_base> device;
};

struct ossia_domain
{
    ossia::domain domain;
};

struct ossia_value_callback_index
{
    ossia::net::address_base::iterator it;
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

inline auto convert_address(ossia_address_t v)
{
  return static_cast<ossia::net::address_base*>(v);
}

inline auto convert(ossia::net::address_base* v)
{
  return static_cast<void*>(v);
}

inline auto convert_node(ossia_node_t v)
{
  return static_cast<ossia::net::node_base*>(v);
}

inline auto convert(ossia::net::node_base* v)
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

inline auto convert(ossia_value_t v)
{
  return v->value;
}


template<typename Fun>
auto safe_function(const char name[], Fun f)
-> decltype(f())
try
{
  // DEBUG_LOG_FMT("Entering %s", name);
  return f();
}
catch(const std::exception& e)
{
  DEBUG_LOG_FMT("%s: %s", name, e.what());
  return decltype(f())();
}
catch(...)
{
DEBUG_LOG_FMT("%s: Exception caught", name);
return decltype(f())();
}

inline const char* copy_string(const std::string& str)
{
  const auto n = str.size();
  auto mbuffer = new char[n + 1]();
  std::sprintf(mbuffer, "%s", str.c_str());
  mbuffer[n] = 0;
  return mbuffer;
}

