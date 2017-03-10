#pragma once
#include <ossia-max/src/ossia-max.hpp>
namespace ossia
{
namespace max
{
struct logger
{
  t_object ob;
  std::shared_ptr<spdlog::logger> log;
};
}
}

extern "C"
{
void* ossia_logger_new(t_symbol *s, long argc, t_atom *argv);
void ossia_logger_in_anything(ossia::max::logger* x, t_symbol *s, long argc, t_atom *argv);
void ossia_logger_free(ossia::max::logger* x);
}
