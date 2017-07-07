#include <ossia-cpp/ossia-cpp98.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
namespace opp
{

value::value(): impl{new ossia::value} { }
value::~value() { delete impl; }
value::value(const value& v): impl{new ossia::value(*v.impl)} { }
value::value(int v): impl{new ossia::value(v)} { }
value::value(bool v): impl{new ossia::value(v)} { }
value::value(float v): impl{new ossia::value(v)} { }
value::value(std::vector<opp::value> v): impl{new ossia::value()}
{
  *this = std::move(v);
}
value::value(std::string v): impl{new ossia::value(std::move(v))} { }

bool value::is_impulse() const { return impl->getType() == ossia::val_type::IMPULSE; }
bool value::is_int() const { return impl->getType() == ossia::val_type::INT; }
bool value::is_float() const { return impl->getType() == ossia::val_type::FLOAT; }
bool value::is_bool() const { return impl->getType() == ossia::val_type::BOOL; }
bool value::is_list() const {
  const auto t = impl->getType();
  return t == ossia::val_type::TUPLE
      || t == ossia::val_type::VEC2F
      || t == ossia::val_type::VEC3F
      || t == ossia::val_type::VEC4F
      ;
}

bool value::is_string() const { return impl->getType() == ossia::val_type::STRING; }
int value::to_int() const { return ossia::convert<int>(*impl); }
float value::to_float() const { return ossia::convert<float>(*impl); }
bool value::to_bool() const { return ossia::convert<bool>(*impl); }
std::vector<value> value::to_list() const {
  std::vector<opp::value> res;
  auto vec = ossia::convert<std::vector<ossia::value>>(*impl);
  for(auto v : vec)
  {
    res.push_back(std::move(v));
  }
  return res;
}

std::string value::to_string() const { return ossia::convert<std::string>(*impl); }

value&value::operator=(const value& v) { *impl = *v.impl; return *this; }
value&value::operator=(int v) { *impl = v; return *this; }
value&value::operator=(float v) { *impl = v; return *this; }
value&value::operator=(bool v) { *impl = v; return *this; }
value&value::operator=(std::vector<value> v) {
  std::vector<ossia::value> res;
  for(auto& val : v)
    res.push_back(std::move(*val.impl));
  *impl = std::move(res);
  return *this;
}

value&value::operator=(std::string v) { *impl = v; return *this; }
void value::set_impulse() { *impl = ossia::impulse{}; }
void value::set_int(int v) { *this = std::move(v); }
void value::set_float(float v) { *this = std::move(v); }
void value::set_bool(bool v) { *this = std::move(v); }
void value::set_list(std::vector<value> v) { *this = std::move(v); }
void value::set_string(std::string v) { *this = std::move(v); }

value::value(value&& v): impl{new ossia::value{std::move(*v.impl)}}
{

}

value& value::operator=(value&& v)
{
  *impl = std::move(*v.impl);
  return *this;
}

value::value(const ossia::value& v): impl{new ossia::value(v)} { }

node::node()
  : node_impl{}
  , addr_impl{}
{

}

node::node(const node& other)
  : node_impl{other.node_impl}
  , addr_impl{other.addr_impl}
{
  init();
}

node&node::operator=(const node& other)
{
  if(node_impl)
    cleanup(*node_impl);

  node_impl = other.node_impl;
  addr_impl = other.addr_impl;

  init();

  return *this;
}

std::string node::get_name() const { return node_impl ? node_impl->get_name() : ""; }

void node::set_name(std::string s) { if(node_impl) node_impl->set_name(std::move(s)); }

std::string node::get_address() const { return ossia::net::osc_address_string(*addr_impl); }

std::vector<node> node::get_children() const {
  std::vector<node> res;
  auto copy = node_impl->children_copy();
  for(auto node : copy)
  {
    res.push_back(node);
  }
  return res;
}

node node::find_child(std::string addr)
{
  return node_impl
      ? node{ossia::net::find_node(*node_impl, addr)}
      : node{};
}

void node::remove_child(std::string addr)
{
  if(node_impl)
  {
    auto cld = ossia::net::find_node(*node_impl, addr);
    cld->get_parent()->remove_child(*cld);
  }
}

node node::create_child(std::string addr)
{
  if(node_impl)
  {
    return node{&ossia::net::create_node(*node_impl, addr)};
  }

  return {};
}

node node::create_impulse(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, n->create_address(ossia::val_type::IMPULSE)};
  }

  return {};
}

node node::create_int(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, n->create_address(ossia::val_type::INT)};
  }

  return {};
}

node node::create_float(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, n->create_address(ossia::val_type::FLOAT)};
  }

  return {};
}

node node::create_bool(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, n->create_address(ossia::val_type::BOOL)};
  }

  return {};
}

node node::create_list(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, n->create_address(ossia::val_type::TUPLE)};
  }

  return {};
}

node node::create_string(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, n->create_address(ossia::val_type::STRING)};
  }

  return {};
}

node node::create_buffer(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::generic_buffer_type(), *n)};
  }

  return {};
}

node node::create_filepath(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::filesystem_path_type(), *n)};
  }

  return {};
}

node node::create_rgb(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::rgb_u{}, *n)};
  }

  return {};
}

node node::create_rgba(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::rgba_u{}, *n)};
  }

  return {};
}

node node::create_argb(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::argb_u{}, *n)};
  }

  return {};
}

node node::create_argb8(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::argb8_u{}, *n)};
  }

  return {};
}

node node::create_hsv(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::hsv_u{}, *n)};
  }

  return {};
}

node node::create_cart2D(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::cartesian_2d_u{}, *n)};
  }

  return {};
}

node node::create_cart3D(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::cartesian_3d_u{}, *n)};
  }

  return {};
}

node node::create_polar(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::polar_u{}, *n)};
  }

  return {};
}

node node::create_spherical(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::spherical_u{}, *n)};
  }

  return {};
}

node node::create_opengl(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::opengl_u{}, *n)};
  }

  return {};
}

node node::create_cylindrical(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::cylindrical_u{}, *n)};
  }

  return {};
}

node node::create_quaternion(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::quaternion_u{}, *n)};
  }

  return {};
}

node node::create_euler(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::euler_u{}, *n)};
  }

  return {};
}

node node::create_axis(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::axis_u{}, *n)};
  }

  return {};
}

node node::create_decibel(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::decibel_u{}, *n)};
  }

  return {};
}

node node::create_midigain(std::string addr) {
  if(node_impl)
  {
    auto n = &ossia::net::create_node(*node_impl, addr);
    return node{n, ossia::setup_address(ossia::midigain_u{}, *n)};
  }

  return {};
}

node&node::set_value(value v) {
  if(addr_impl)
    addr_impl->push_value(*v.impl);
  return *this;
}

value node::get_value() const {
  if(addr_impl)
    return addr_impl->value();
  return {};
}

void node::set_value_callback(value_callback c, void* context)
{
  if(addr_impl)
  {
    addr_impl->add_callback([=] (const ossia::value& v) {
      c(context, v);
    });
  }
}

node&node::set_min(value min) {
  if(addr_impl)
  {
    auto dom = addr_impl->get_domain();
    dom.set_min(std::move(*min.impl));
    addr_impl->set_domain(std::move(dom));
  }
  return *this;
}

value node::get_min() const
{
  if(addr_impl)
  {
    auto dom = addr_impl->get_domain();
    return ossia::get_min(dom);
  }
  return {};
}

node&node::set_max(value max) {
  if(addr_impl)
  {
    auto dom = addr_impl->get_domain();
    dom.set_max(std::move(*max.impl));
    addr_impl->set_domain(std::move(dom));
  }
  return *this;
}

value node::get_max() const
{
  if(addr_impl)
  {
    auto dom = addr_impl->get_domain();
    return ossia::get_max(dom);
  }
  return {};
}

node&node::set_accepted_values(std::vector<value> v) {
  if(addr_impl)
  {
    auto dom = addr_impl->get_domain();

    std::vector<ossia::value> vals;
    for(const auto& val : v)
      vals.push_back(*val.impl);

    ossia::set_values(dom, std::move(vals));

    addr_impl->set_domain(std::move(dom));
  }
  return *this;
}

std::vector<value> node::get_accepted_values() const
{
  if(addr_impl)
  {
    auto dom = addr_impl->get_domain();
    // TODO
    return {};
  }
  return {};
}

node&node::set_unit(std::string v)
{
  if(addr_impl)
  {
    addr_impl->set_unit(ossia::parse_pretty_unit(v));
  }
  return *this;
}

std::string node::get_unit() const
{
  if(addr_impl)
  {
    return ossia::get_pretty_unit_text(addr_impl->get_unit());
  }
  return {};
}

node&node::set_access(access_mode v)
{
  if(addr_impl)
  {
    addr_impl->set_access(static_cast<ossia::access_mode>(v));
  }
  return *this;
}

access_mode node::get_access() const
{
  if(addr_impl)
  {
    return static_cast<opp::access_mode>(addr_impl->get_access());
  }
  return {};
}

node&node::set_bounding(bounding_mode v)
{
  if(addr_impl)
  {
    addr_impl->set_bounding(static_cast<ossia::bounding_mode>(v));
  }
  return *this;
}

bounding_mode node::get_bounding() const
{
  if(addr_impl)
  {
    return static_cast<opp::bounding_mode>(addr_impl->get_bounding());
  }
  return {};
}

node&node::set_default_value(value v)
{
  if(node_impl)
  {
    ossia::net::set_default_value(*node_impl, *v.impl);
  }
  return *this;
}

value node::get_default_value()
{
  if(node_impl)
  {
    auto v = ossia::net::get_default_value(*node_impl);
    if(v)
      return *v;
    return {};
  }
  return {};
}

node&node::set_description(std::string v)
{
  if(node_impl)
  {
    ossia::net::set_description(*node_impl, std::move(v));
  }
  return *this;
}

std::string node::get_description() const
{
  if(node_impl)
  {
    auto v = ossia::net::get_description(*node_impl);
    if(v)
      return *v;
    return {};
  }
  return {};
}

node&node::set_tags(std::vector<std::string> v)
{
  if(node_impl)
  {
    ossia::net::set_tags(*node_impl, v);
  }
  return *this;
}

std::vector<std::string> node::get_tags() const
{
  if(node_impl)
  {
    auto v = ossia::net::get_tags(*node_impl);
    if(v)
      return *v;
    return {};
  }
  return {};
}

node&node::set_priority(int v)
{
  if(node_impl)
  {
    ossia::net::set_priority(*node_impl, v);
  }
  return *this;
}

int node::get_priority()
{
  if(node_impl)
  {
    auto v = ossia::net::get_priority(*node_impl);
    if(v)
      return *v;
    return {};
  }
  return {};
}

node&node::set_refresh_rate(int v)
{
  if(node_impl)
  {
    ossia::net::set_refresh_rate(*node_impl, v);
  }
  return *this;
}

int node::get_refresh_rate()
{
  if(node_impl)
  {
    auto v = ossia::net::get_refresh_rate(*node_impl);
    if(v)
      return *v;
    return {};
  }
  return {};
}

node&node::set_value_step_size(double v)
{
  if(node_impl)
  {
    ossia::net::set_value_step_size(*node_impl, v);
  }
  return *this;
}

double node::get_value_step_size()
{
  if(node_impl)
  {
    auto v = ossia::net::get_value_step_size(*node_impl);
    if(v)
      return *v;
    return {};
  }
  return {};
}

node&node::set_hidden(bool v)
{
  if(node_impl)
  {
    ossia::net::set_hidden(*node_impl, v);
  }
  return *this;
}

bool node::get_hidden() const
{
  if(node_impl)
  {
    return ossia::net::get_hidden(*node_impl);
  }
  return {};
}

node&node::set_critical(bool v)
{
  if(node_impl)
  {
    ossia::net::set_critical(*node_impl, v);
  }
  return *this;
}

bool node::get_critical() const
{
  if(node_impl)
  {
    return ossia::net::get_critical(*node_impl);
  }
  return {};
}

bool node::get_zombie() const
{
  if(node_impl)
  {
    return ossia::net::get_zombie(*node_impl);
  }
  return {};
}

node&node::set_muted(bool v)
{
  if(node_impl)
  {
    ossia::net::set_muted(*node_impl, v);
  }
  return *this;
}

bool node::get_muted() const
{
  if(node_impl)
  {
    return ossia::net::get_muted(*node_impl);
  }
  return {};
}

node&node::set_repetition_filter(bool v)
{
  if(addr_impl)
  {
    addr_impl->set_repetition_filter(v ? ossia::repetition_filter::ON : ossia::repetition_filter::OFF);
  }
  return *this;
}

bool node::get_repetition_filter() const
{
  if(addr_impl)
  {
    return addr_impl->get_repetition_filter();
  }
  return {};
}

node::node(ossia::net::node_base* b)
  : node_impl{b}
  , addr_impl{b->get_address()}
{
  init();
}

node::node(ossia::net::node_base* b, ossia::net::address_base* a)
  : node_impl{b}
  , addr_impl{a}
{
  init();
}

void node::init()
{
  assert(node_impl);
  node_impl->about_to_be_deleted.connect<node, &node::cleanup>(*this);
  node_impl->get_device().on_address_removing.connect<node, &node::cleanup_address>(*this);
}

void node::cleanup(const ossia::net::node_base&)
{
  node_impl = nullptr;
  addr_impl = nullptr;

}
void node::cleanup_address(const ossia::net::address_base&)
{
  if(addr_impl)
    node_impl->get_device().on_address_removing.disconnect<node, &node::cleanup_address>(*this);
  addr_impl = nullptr;

}

oscquery_server::oscquery_server(std::string name, int wsPort, int oscPort)
{
  impl = new ossia::net::generic_device(
           std::make_unique<ossia::oscquery::oscquery_server_protocol>(oscPort, wsPort),
           name
           );
}

oscquery_server::~oscquery_server()
{
  delete impl;
}

node oscquery_server::get_root_node() const
{
  return node{&impl->get_root_node()};
}

oscquery_mirror::oscquery_mirror(std::string name, std::string host)
{
  impl = new ossia::net::generic_device(
           std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(host),
           name
           );
}

oscquery_mirror::~oscquery_mirror()
{
  delete impl;
}

node oscquery_mirror::get_root_node() const
{
  return node{&impl->get_root_node()};
}


}
