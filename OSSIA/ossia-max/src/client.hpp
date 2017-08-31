#pragma once

#include "ossia_object_base.hpp"

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_client structure declaration

struct t_client : t_object_base
{
  ossia::net::local_protocol m_local_proto;

  static void register_children(t_client*);
  void unregister_children();

  static void loadbang(t_client*);

  static void explore(const ossia::net::node_base& node);
};

} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_client class declaration

extern "C" {
void* ossia_client_new(t_symbol*, long, t_atom*);
void ossia_client_free(ossia::max::t_client*);
void ossia_client_dump(ossia::max::t_client*);
void ossia_client_connect(ossia::max::t_client*, t_symbol*, int, t_atom*);
}
