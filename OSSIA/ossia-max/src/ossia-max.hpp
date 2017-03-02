#pragma once
#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post

#include <ossia/ossia.hpp>
#include <ossia/context.hpp>
namespace ossia
{
namespace max
{

// Return a singleton local device that can be used if the user has not made any device.
class singleton
{
public:
  static singleton& instance();

  static ossia::net::device_base& device_instance();

  t_class* ossia_parameter_class{};
  t_class* ossia_logger_class{};

private:
  singleton();

  ossia::context ctx;
  ossia::net::local_protocol* m_localProtocol{};
  ossia::net::generic_device m_device;
};

}
}
