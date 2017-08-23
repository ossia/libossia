#pragma once
#include "ossia_object_base.hpp"
#include <boost/optional.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_remote structure declaration

struct t_remote;

class t_matcher
{
public:
  t_matcher(ossia::net::node_base* n, t_remote* p); // constructor
  ~t_matcher();
  t_matcher(const t_matcher&) = delete;
  t_matcher(t_matcher&& other);
  t_matcher& operator=(const t_matcher&) = delete;
  t_matcher& operator=(t_matcher&& other);

  void set_value(const ossia::value& v);
  auto get_node() const { return node; }

  inline bool operator==(const t_matcher& rhs)
  { return (get_node() == rhs.node); }


  private:
    ossia::net::node_base* node{};
    t_remote* parent{};

    ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
        callbackit = ossia::none;

};

struct t_remote : t_object_base
{
  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  std::vector<boost::optional<ossia::callback_container<ossia::value_callback>::iterator> >
      m_callbackits;

  std::vector<t_matcher> m_matchers{};

  bool m_is_pattern{};
  ossia::net::device_base* m_dev{};

  void is_deleted(const ossia::net::node_base& n);
  static void remote_bind(t_remote* x, t_symbol* address);

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);

  static ossia::safe_vector<t_remote*>& quarantine()
  {
    static ossia::safe_vector<t_remote*> quarantine;
    return quarantine;
  }
};

} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_remote class declaration

extern "C" {
void* ossia_remote_new(t_symbol*, long, t_atom*);
void ossia_remote_free(ossia::max::t_remote*);

//    void ossia_remote_click(t_remote *x, t_floatarg xpos, t_floatarg ypos,
//    t_floatarg shift, t_floatarg ctrl, t_floatarg alt);
void ossia_remote_assist(ossia::max::t_remote*, void*, long, long, char*);

void ossia_remote_in_float(ossia::max::t_remote*, double f);
void ossia_remote_in_int(ossia::max::t_remote*, long int f);
void ossia_remote_in_bang(ossia::max::t_remote*);
void ossia_remote_in_symbol(ossia::max::t_remote*, t_symbol*);
void ossia_remote_in_char(ossia::max::t_remote*, char);
void ossia_remote_in_anything(ossia::max::t_remote*, t_symbol*, long, t_atom*);

}
