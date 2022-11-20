#pragma once

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/value_callback.hpp>
#include <ossia/network/value/value.hpp>

#include <ext.h>

namespace ossia
{
namespace max_binding
{

struct object_base;

class matcher
{
public:
  matcher(ossia::net::node_base* n, object_base* p); // constructor
  ~matcher();
  matcher(const matcher&) = delete;
  matcher(matcher&& other) = delete;
  matcher& operator=(const matcher&) = delete;
  matcher& operator=(matcher&& other) = delete;

  void output_value(ossia::value v);
  void remove_callback();

  ossia::net::node_base* get_node() const { return node; }
  object_base* get_owner() const
  {
    return owner;
  } // return the max object that holds this
  const t_atom* get_atom_addr_ptr() const { return &m_addr; }
  void set_addr_symbol();

  inline bool operator==(const matcher& rhs) { return (get_node() == rhs.node); }

  void set_dead() { m_dead = true; }
  void set_zombie() { m_zombie = true; }
  bool is_zombie() const { return m_zombie; }
  bool is_locked() const { return m_lock; }
  bool is_dead() const { return m_dead; }

  // holds the position of the matcher in the max object internal list
  // this is used to output the 'instance' number of a ossia.remote with pattern matching
  int m_index{0};

  //private:
  // TODO rename those as m_node/m_owner/m_callbackit
  ossia::net::node_base* node{};
  ossia::net::parameter_base* orig_param{};
  object_base* owner{};

  std::optional<ossia::callback_container<ossia::value_callback>::iterator> callbackit
      = std::nullopt;

  bool m_dead{};   // true when Max object is being deleted
  bool m_zombie{}; // true if node is deleted, t_matcher should be deleted asap
  bool m_lock{};
  t_atom m_addr{};
};

} // namespace max
} // namespace ossia
