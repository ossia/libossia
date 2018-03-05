// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia-cpp/ossia-cpp98.hpp>
namespace opp
{

value::value() : m_val{new ossia::value}
{
}
value::~value()
{
  delete m_val;
}
value::value(const value& v) : m_val{new ossia::value(*v.m_val)}
{
}
value::value(int v) : m_val{new ossia::value(v)}
{
}
value::value(bool v) : m_val{new ossia::value(v)}
{
}
value::value(float v) : m_val{new ossia::value(v)}
{
}
value::value(double v) : m_val{new ossia::value(v)}
{
}
value::value(const char* v) : m_val{new ossia::value(std::string(v))}
{
}
value::value(std::vector<opp::value> v) : m_val{new ossia::value()}
{
  *this = std::move(v);
}
value::value(std::string v) : m_val{new ossia::value(std::move(v))}
{
}

bool value::is_impulse() const
{
  return m_val->getType() == ossia::val_type::IMPULSE;
}
bool value::is_int() const
{
  return m_val->getType() == ossia::val_type::INT;
}
bool value::is_float() const
{
  return m_val->getType() == ossia::val_type::FLOAT;
}
bool value::is_bool() const
{
  return m_val->getType() == ossia::val_type::BOOL;
}
bool value::is_list() const
{
  const auto t = m_val->getType();
  return t == ossia::val_type::LIST || t == ossia::val_type::VEC2F
         || t == ossia::val_type::VEC3F || t == ossia::val_type::VEC4F;
}

bool value::is_string() const
{
  return m_val->getType() == ossia::val_type::STRING;
}
int value::to_int() const
{
  return ossia::convert<int>(*m_val);
}
float value::to_float() const
{
  return ossia::convert<float>(*m_val);
}
bool value::to_bool() const
{
  return ossia::convert<bool>(*m_val);
}
std::vector<value> value::to_list() const
{
  std::vector<opp::value> res;
  auto vec = ossia::convert<std::vector<ossia::value>>(*m_val);
  for (auto v : vec)
  {
    res.push_back(std::move(v));
  }
  return res;
}

std::string value::to_string() const
{
  return ossia::convert<std::string>(*m_val);
}

value& value::operator=(const value& v)
{
  *m_val = *v.m_val;
  return *this;
}
value& value::operator=(int v)
{
  *m_val = v;
  return *this;
}
value& value::operator=(float v)
{
  *m_val = v;
  return *this;
}
value& value::operator=(bool v)
{
  *m_val = v;
  return *this;
}
value& value::operator=(std::vector<value> v)
{
  std::vector<ossia::value> res;
  for (auto& val : v)
    res.push_back(std::move(*val.m_val));
  *m_val = std::move(res);
  return *this;
}

value& value::operator=(std::string v)
{
  *m_val = v;
  return *this;
}
void value::set_impulse()
{
  *m_val = ossia::impulse{};
}
void value::set_int(int v)
{
  *this = std::move(v);
}
void value::set_float(float v)
{
  *this = std::move(v);
}
void value::set_bool(bool v)
{
  *this = std::move(v);
}
void value::set_list(std::vector<value> v)
{
  *this = std::move(v);
}
void value::set_string(std::string v)
{
  *this = std::move(v);
}

#if defined(OSSIA_CPP_CXX11)
value::value(opp::value&& v) : m_val{new ossia::value{std::move(*v.m_val)}}
{
}

value& value::operator=(opp::value&& v)
{
  *m_val = std::move(*v.m_val);
  return *this;
}
#endif

value::value(const ossia::value& v) : m_val{new ossia::value(v)}
{
}

node::node() : m_node{}, m_addr{}
{
}

node::node(const node& other)
    : m_node{other.m_node}, m_addr{other.m_addr}
{
  init();
}

node& node::operator=(const node& other)
{
  if (m_node)
    cleanup(*m_node);

  m_node = other.m_node;
  m_addr = other.m_addr;

  init();

  return *this;
}

node::~node()
{
  if (m_node)
  {
    m_node->about_to_be_deleted.disconnect<node, &node::cleanup>(*this);
    m_node->get_device()
        .on_parameter_removing.disconnect<node, &node::cleanup_parameter>(*this);
  }
}

bool node::valid() const
{
  return m_node;
}

std::string node::get_name() const
{
  return m_node ? m_node->get_name() : "";
}

void node::set_name(std::string s)
{
  if (m_node)
    m_node->set_name(std::move(s));
}

std::string node::get_parameter() const
{
  if(m_addr)
    return ossia::net::osc_parameter_string(*m_addr);
  return "";
}

std::vector<node> node::get_children() const
{
  std::vector<node> res;
  auto copy = m_node->children_copy();
  for (auto node : copy)
  {
    res.push_back(node);
  }
  return res;
}

node node::find_child(std::string addr)
{
  return m_node ? node{ossia::net::find_node(*m_node, addr)} : node{};
}

void node::remove_child(std::string addr)
{
  if (m_node)
  {
    auto cld = ossia::net::find_node(*m_node, addr);
    cld->get_parent()->remove_child(*cld);
  }
}

node node::create_child(std::string addr)
{
  if (m_node)
  {
    return node{&ossia::net::create_node(*m_node, addr)};
  }

  return {};
}

node node::create_impulse(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::IMPULSE)};
  }

  return {};
}

node node::create_int(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::INT)};
  }

  return {};
}

node node::create_float(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::FLOAT)};
  }

  return {};
}

node node::create_bool(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::BOOL)};
  }

  return {};
}

node node::create_list(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::LIST)};
  }

  return {};
}

node node::create_vec2f(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::val_type::VEC2F, *n)};
  }

  return {};
}

node node::create_vec3f(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::val_type::VEC3F, *n)};
  }

  return {};
}

node node::create_vec4f(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::val_type::VEC4F, *n)};
  }

  return {};
}

node node::create_string(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::STRING)};
  }

  return {};
}

node node::create_buffer(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::generic_buffer_type(), *n)};
  }

  return {};
}

node node::create_filepath(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::filesystem_path_type(), *n)};
  }

  return {};
}

node node::create_rgb(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::rgb_u{}, *n)};
  }

  return {};
}

node node::create_rgba(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::rgba_u{}, *n)};
  }

  return {};
}

node node::create_argb(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::argb_u{}, *n)};
  }

  return {};
}

node node::create_argb8(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::argb8_u{}, *n)};
  }

  return {};
}

node node::create_hsv(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::hsv_u{}, *n)};
  }

  return {};
}

node node::create_cart2D(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::cartesian_2d_u{}, *n)};
  }

  return {};
}

node node::create_cart3D(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::cartesian_3d_u{}, *n)};
  }

  return {};
}

node node::create_polar(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::polar_u{}, *n)};
  }

  return {};
}

node node::create_spherical(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::spherical_u{}, *n)};
  }

  return {};
}

node node::create_opengl(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::opengl_u{}, *n)};
  }

  return {};
}

node node::create_cylindrical(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::cylindrical_u{}, *n)};
  }

  return {};
}

node node::create_quaternion(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::quaternion_u{}, *n)};
  }

  return {};
}

node node::create_euler(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::euler_u{}, *n)};
  }

  return {};
}

node node::create_axis(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::axis_u{}, *n)};
  }

  return {};
}

node node::create_decibel(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::decibel_u{}, *n)};
  }

  return {};
}

node node::create_midigain(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::midigain_u{}, *n)};
  }

  return {};
}

node& node::set_value(value v)
{
  if (m_addr)
    m_addr->push_value(*v.m_val);
  return *this;
}

value node::get_value() const
{
  if (m_addr)
    return m_addr->value();
  return {};
}

value node::fetch_value() const
{
  if (m_addr)
    return m_addr->fetch_value();
  return {};
}

void node::set_value_callback(value_callback c, void* context)
{
  if (m_addr)
  {
    m_addr->add_callback([=](const ossia::value& v) { c(context, v); });
  }
}

node& node::set_min(value min)
{
  if (m_addr)
  {
    auto dom = m_addr->get_domain();
    dom.set_min(std::move(*min.m_val));
    m_addr->set_domain(std::move(dom));
  }
  return *this;
}

value node::get_min() const
{
  if (m_addr)
  {
    auto dom = m_addr->get_domain();
    return ossia::get_min(dom);
  }
  return {};
}

node& node::set_max(value max)
{
  if (m_addr)
  {
    auto dom = m_addr->get_domain();
    dom.set_max(std::move(*max.m_val));
    m_addr->set_domain(std::move(dom));
  }
  return *this;
}

value node::get_max() const
{
  if (m_addr)
  {
    auto dom = m_addr->get_domain();
    return ossia::get_max(dom);
  }
  return {};
}

node& node::set_accepted_values(std::vector<value> v)
{
  if (m_addr)
  {
    auto dom = m_addr->get_domain();

    std::vector<ossia::value> vals;
    for (const auto& val : v)
      vals.push_back(*val.m_val);

    ossia::set_values(dom, std::move(vals));

    m_addr->set_domain(std::move(dom));
  }
  return *this;
}

std::vector<value> node::get_accepted_values() const
{
  if (m_addr)
  {
    auto dom = m_addr->get_domain();
    // TODO
    return {};
  }
  return {};
}

node& node::set_unit(std::string v)
{
  if (m_addr)
  {
    m_addr->set_unit(ossia::parse_pretty_unit(v));
  }
  return *this;
}

std::string node::get_unit() const
{
  if (m_addr)
  {
    return ossia::get_pretty_unit_text(m_addr->get_unit());
  }
  return {};
}

node& node::set_access(access_mode v)
{
  if (m_addr)
  {
    m_addr->set_access(static_cast<ossia::access_mode>(v));
  }
  return *this;
}

access_mode node::get_access() const
{
  if (m_addr)
  {
    return static_cast<opp::access_mode>(m_addr->get_access());
  }
  return {};
}

node& node::set_bounding(bounding_mode v)
{
  if (m_addr)
  {
    m_addr->set_bounding(static_cast<ossia::bounding_mode>(v));
  }
  return *this;
}

bounding_mode node::get_bounding() const
{
  if (m_addr)
  {
    return static_cast<opp::bounding_mode>(m_addr->get_bounding());
  }
  return {};
}

node& node::set_default_value(value v)
{
  if (m_node)
  {
    ossia::net::set_default_value(*m_node, *v.m_val);
  }
  return *this;
}

value node::get_default_value()
{
  if (m_node)
  {
    auto v = ossia::net::get_default_value(*m_node);
    if (v)
      return *v;
    return {};
  }
  return {};
}

node& node::set_description(std::string v)
{
  if (m_node)
  {
    ossia::net::set_description(*m_node, std::move(v));
  }
  return *this;
}

std::string node::get_description() const
{
  if (m_node)
  {
    auto v = ossia::net::get_description(*m_node);
    if (v)
      return *v;
    return {};
  }
  return {};
}

node& node::set_tags(std::vector<std::string> v)
{
  if (m_node)
  {
    ossia::net::set_tags(*m_node, v);
  }
  return *this;
}

std::vector<std::string> node::get_tags() const
{
  if (m_node)
  {
    auto v = ossia::net::get_tags(*m_node);
    if (v)
      return *v;
    return {};
  }
  return {};
}

node& node::set_instance_bounds(int min, int max)
{
  if (m_node)
  {
    ossia::net::set_instance_bounds(*m_node, ossia::net::instance_bounds{min, max});
  }
  return *this;
}
node& node::unset_instance_bounds()
{
  if (m_node)
  {
    ossia::net::set_instance_bounds(*m_node, ossia::none);
  }
  return *this;
}

std::pair<int,int> node::get_instance_bounds() const
{
  if (m_node)
  {
    auto v = ossia::net::get_instance_bounds(*m_node);
    if (v)
      return {v->min_instances, v->max_instances};
    return {};
  }
  return {};
}

node& node::set_priority(float v)
{
  if (m_node)
  {
    ossia::net::set_priority(*m_node, v);
  }
  return *this;
}

node& node::unset_priority()
{
  if (m_node)
  {
    ossia::net::set_priority(*m_node, ossia::none);
  }
  return *this;
}

float node::get_priority()
{
  if (m_node)
  {
    auto v = ossia::net::get_priority(*m_node);
    if (v)
      return *v;
    return {};
  }
  return {};
}

node& node::set_refresh_rate(int v)
{
  if (m_node)
  {
    ossia::net::set_refresh_rate(*m_node, v);
  }
  return *this;
}

node& node::unset_refresh_rate()
{
  if (m_node)
  {
    ossia::net::set_refresh_rate(*m_node, ossia::none);
  }
  return *this;
}

int node::get_refresh_rate()
{
  if (m_node)
  {
    auto v = ossia::net::get_refresh_rate(*m_node);
    if (v)
      return *v;
    return {};
  }
  return {};
}

node& node::set_value_step_size(double v)
{
  if (m_node)
  {
    ossia::net::set_value_step_size(*m_node, v);
  }
  return *this;
}


node& node::unset_value_step_size()
{
  if (m_node)
  {
    ossia::net::set_value_step_size(*m_node, ossia::none);
  }
  return *this;
}

double node::get_value_step_size()
{
  if (m_node)
  {
    auto v = ossia::net::get_value_step_size(*m_node);
    if (v)
      return *v;
    return {};
  }
  return {};
}

node& node::set_hidden(bool v)
{
  if (m_node)
  {
    ossia::net::set_hidden(*m_node, v);
  }
  return *this;
}

bool node::get_hidden() const
{
  if (m_node)
  {
    return ossia::net::get_hidden(*m_node);
  }
  return {};
}

node& node::set_disabled(bool v)
{
  if (m_node)
  {
    ossia::net::set_disabled(*m_node, v);
  }
  return *this;
}

bool node::get_disabled() const
{
  if (m_node)
  {
    return ossia::net::get_disabled(*m_node);
  }
  return {};
}

node& node::set_critical(bool v)
{
  if (m_node)
  {
    ossia::net::set_critical(*m_node, v);
  }
  return *this;
}

bool node::get_critical() const
{
  if (m_node)
  {
    return ossia::net::get_critical(*m_node);
  }
  return {};
}

bool node::get_zombie() const
{
  if (m_node)
  {
    return ossia::net::get_zombie(*m_node);
  }
  return {};
}

node& node::set_muted(bool v)
{
  if (m_node)
  {
    ossia::net::set_muted(*m_node, v);
  }
  return *this;
}

bool node::get_muted() const
{
  if (m_node)
  {
    return ossia::net::get_muted(*m_node);
  }
  return {};
}

node& node::set_repetition_filter(bool v)
{
  if (m_addr)
  {
    m_addr->set_repetition_filter(
        v ? ossia::repetition_filter::ON : ossia::repetition_filter::OFF);
  }
  return *this;
}

bool node::get_repetition_filter() const
{
  if (m_addr)
  {
    return m_addr->get_repetition_filter();
  }
  return {};
}

node::node(ossia::net::node_base* b)
    : m_node{b}, m_addr{b->get_parameter()}
{
  init();
}

node::node(ossia::net::node_base* b, ossia::net::parameter_base* a)
    : m_node{b}, m_addr{a}
{
  init();
}

void node::init()
{
  assert(m_node);
  m_node->about_to_be_deleted.connect<node, &node::cleanup>(*this);
  m_node->get_device()
      .on_parameter_removing.connect<node, &node::cleanup_parameter>(*this);
}

void node::cleanup(const ossia::net::node_base&)
{
  m_node = nullptr;
  m_addr = nullptr;
}
void node::cleanup_parameter(const ossia::net::parameter_base&)
{
  if (m_addr)
    m_node->get_device()
        .on_parameter_removing.disconnect<node, &node::cleanup_parameter>(*this);
  m_addr = nullptr;
}

oscquery_server::oscquery_server(std::string name, int wsPort, int oscPort)
{
  m_dev = new ossia::net::generic_device(
      std::make_unique<ossia::oscquery::oscquery_server_protocol>(
          oscPort, wsPort),
      name);
}

oscquery_server::~oscquery_server()
{
  delete m_dev;
}

node oscquery_server::get_root_node() const
{
  return node{&m_dev->get_root_node()};
}

oscquery_mirror::oscquery_mirror(std::string name, std::string host) try
{
  m_dev = new ossia::net::generic_device(
      std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(host), name);
}
catch (...)
{
}

oscquery_mirror::~oscquery_mirror()
{
  delete m_dev;
}

node oscquery_mirror::get_root_node() const
{
  if (m_dev)
    return node{&m_dev->get_root_node()};
  return node{};
}

void oscquery_mirror::refresh()
{
  if (m_dev)
    static_cast<ossia::oscquery::oscquery_mirror_protocol&>(
        m_dev->get_protocol())
        .update(m_dev->get_root_node());
}

void oscquery_mirror::reconnect(std::string name, std::string host)
{
  if (m_dev)
    delete m_dev;
  m_dev = new ossia::net::generic_device(
      std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(host), name);
}
}
