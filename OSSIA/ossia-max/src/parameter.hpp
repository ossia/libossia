#pragma once
#include <ossia-max/src/ossia-max.hpp>

namespace ossia {
namespace max {
struct parameter
{
  t_object ob; // Always first
  void* outlet{};
  ossia::net::node_base* node{};
};
} }

extern "C" {
void* ossia_parameter_new(t_symbol *s, long argc, t_atom *argv);
void ossia_parameter_free(ossia::max::parameter* x);

void ossia_parameter_assist(ossia::max::parameter *x, void *b, long m, long a, char *s);
void ossia_parameter_in_float(ossia::max::parameter* x, double f);
void ossia_parameter_in_int(ossia::max::parameter* x, long int f);
void ossia_parameter_in_bang(ossia::max::parameter* x);
void ossia_parameter_in_symbol(ossia::max::parameter* x, t_symbol* f);
void ossia_parameter_in_char(ossia::max::parameter* x, char f);
void ossia_parameter_in_anything(ossia::max::parameter* x, t_symbol *s, long argc, t_atom *argv);
}
