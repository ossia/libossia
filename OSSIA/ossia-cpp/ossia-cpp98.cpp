// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia-cpp/ossia-cpp98.hpp>
#include<array>
namespace opp
{

//**************************************************************//
//                          value                               //
//*************************************************************//


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
value::value(char v) : m_val{new ossia::value(v)}
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
value::value(impulse p) : m_val{new ossia::value(ossia::impulse{})}
{
}
value::value(const vec2f v) : m_val{new ossia::value()}
{
  *this = std::move(v);
}
value::value(const vec3f v) : m_val{new ossia::value()}
{
  *this = std::move(v);
}
value::value(const vec4f v) : m_val{new ossia::value()}
{
  *this = std::move(v);
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
  return m_val->get_type() == ossia::val_type::IMPULSE;
}
bool value::is_char() const
{
  return m_val->get_type() == ossia::val_type::CHAR;
}
bool value::is_int() const
{
  return m_val->get_type() == ossia::val_type::INT;
}
bool value::is_float() const
{
  return m_val->get_type() == ossia::val_type::FLOAT;
}
bool value::is_bool() const
{
  return m_val->get_type() == ossia::val_type::BOOL;
}
bool value::is_vec2f() const
{
  const auto t = m_val->get_type();
  return t == ossia::val_type::VEC2F;
}
bool value::is_vec3f() const
{
  const auto t = m_val->get_type();
  return t == ossia::val_type::VEC3F;
}
bool value::is_vec4f() const
{
  const auto t = m_val->get_type();
  return t == ossia::val_type::VEC4F;
}
bool value::is_list() const
{
  const auto t = m_val->get_type();
  return t == ossia::val_type::LIST || t == ossia::val_type::VEC2F
         || t == ossia::val_type::VEC3F || t == ossia::val_type::VEC4F;
}

bool value::is_string() const
{
  return m_val->get_type() == ossia::val_type::STRING;
}
char value::to_char() const
{
  return ossia::convert<char>(*m_val);
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
value::vec2f value::to_vec2f() const
{
  value::vec2f res;
  res[0] = ossia::convert<ossia::vec2f>(*m_val)[0];
  res[1] = ossia::convert<ossia::vec2f>(*m_val)[1];
  return res;
}
value::vec3f value::to_vec3f() const
{
  value::vec3f res;
  res[0] = ossia::convert<ossia::vec3f>(*m_val)[0];
  res[1] = ossia::convert<ossia::vec3f>(*m_val)[1];
  res[2] = ossia::convert<ossia::vec3f>(*m_val)[2];
  return res;
}
value::vec4f value::to_vec4f() const
{
  value::vec4f res;
  res[0] = ossia::convert<ossia::vec4f>(*m_val)[0];
  res[1] = ossia::convert<ossia::vec4f>(*m_val)[1];
  res[2] = ossia::convert<ossia::vec4f>(*m_val)[2];
  res[3] = ossia::convert<ossia::vec4f>(*m_val)[3];
  return res;
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
value& value::operator=(char v)
{
  *m_val = v;
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
value& value::operator=(vec2f v)
{
  *m_val = std::array<float, 2ul>{v[0], v[1]};
  return *this;
}
value& value::operator=(vec3f v)
{
  *m_val = std::array<float, 3ul>{v[0], v[1], v[2]};
  return *this;
}
value& value::operator=(vec4f v)
{
  *m_val = std::array<float, 4ul>{v[0], v[1], v[2], v[3]};
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
void value::set_char(char v)
{
  *this = std::move(v);
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
void value::set_vec2f(vec2f v)
{
  *this = std::move(v);
}
void value::set_vec3f(vec3f v)
{
  *this = std::move(v);
}
void value::set_vec4f(vec4f v)
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

//*************************************************************//
//                      callback_index                         //
//*************************************************************//

struct callback_index::impl {
  ossia::callback_container<ossia::value_callback>::iterator iterator;
};

callback_index::callback_index()
  : index{}
{
}

callback_index::~callback_index()
{
   delete index;
}

callback_index::callback_index(const callback_index& other)
{
  if(other.index)
    index = new impl{*other.index};
}

callback_index& callback_index::operator=(const callback_index& other)
{
  if(!other.index)
  {
    delete index;
    index = nullptr;
  }
  else
  {
    if(!index)
      index = new impl{*other.index};
    else
      *index = *other.index;
  }
  return *this;
}


callback_index::operator bool() const
{
  return bool(index);
}



//*************************************************************//
//                          node                               //
//*************************************************************//

node::node() : m_node{}, m_param{}
{
}

node::node(const node& other)
  : node{other.m_node, other.m_param}
{
}

node::node(node&& other)
  : node{other.m_node, other.m_param}
{

}


node::node(ossia::net::node_base* b)
  : node{b, (b ? b->get_parameter() : nullptr)}
{
}

node::node(ossia::net::node_base* b, ossia::net::parameter_base* a)
    : m_node{b}, m_param{a}
{
  init();
}

node& node::operator=(const node& other)
{
  if (m_node)
    cleanup(*m_node);

  m_node = other.m_node;
  m_param = other.m_param;

  init();

  return *this;
}

node& node::operator=(node&& other)
{
  if (m_node)
    cleanup(*m_node);

  m_node = other.m_node;
  m_param = other.m_param;

  if(m_node)
    other.cleanup(*m_node);

  init();

  return *this;
}

void node::init()
{
  if(m_node)
  {
    m_node->about_to_be_deleted.connect<&node::cleanup>(*this);
    m_node->get_device()
        .on_parameter_removing.connect<&node::cleanup_parameter>(*this);
  }
}

void node::cleanup(const ossia::net::node_base&)
{
  if (m_node)
  {
    m_node->about_to_be_deleted.disconnect<&node::cleanup>(*this);
    m_node->get_device().on_parameter_removing.disconnect<&node::cleanup_parameter>(*this);
  }

  m_node = nullptr;
  m_param = nullptr;
}

void node::cleanup_parameter(const ossia::net::parameter_base&)
{
  if(m_node)
    m_node->get_device().on_parameter_removing.disconnect<&node::cleanup_parameter>(*this);
  m_param = nullptr;
}


node::~node()
{
  if (m_node)
  {
    m_node->about_to_be_deleted.disconnect<&node::cleanup>(*this);
    m_node->get_device().on_parameter_removing.disconnect<&node::cleanup_parameter>(*this);
}
}

node node::parent() const
{
  if(m_node)
  {
    if(auto p = m_node->get_parent())
    {
      return node{p};
    }
  }

  return node{};
}

node::operator bool() const
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

bool node::has_parameter() const
{
    return(m_param);
}

std::string node::get_address() const
{
  if(m_param)
    return ossia::net::osc_parameter_string(*m_param);
  return "";
}

std::vector<node> node::get_namespace() const
{
  std::vector<node> res;
  auto list = ossia::net::list_all_child(m_node);
  for (auto child : list)
  {
    res.push_back(child);
  }
  return res;
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

node node::find_child(std::string addr) const
{
  return m_node ? node{ossia::net::find_node(*m_node, addr)} : node{};
}

std::vector<node> node::find_children(std::string addr) const
{
  std::vector<node> res;
  if (!m_node)
    return res;

  auto nodes = ossia::net::find_nodes(*m_node, addr);
  res.reserve(nodes.size());
  for(auto n : nodes)
    res.push_back(n);

  return res;
}

void node::remove_child(std::string addr)
{
  if (m_node)
  {
    if(auto cld = ossia::net::find_node(*m_node, addr))
      cld->get_parent()->remove_child(*cld);
  }
}

void node::remove_children()
{
  if (m_node)
  {
    m_node->clear_children();
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



void node::set_impulse()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::IMPULSE);
  }

}

void node::set_char()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::CHAR);
  }

}

void node::set_int()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::INT);
  }

}

void node::set_float()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::FLOAT);
  }

}

void node::set_bool()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::BOOL);
  }

}

void node::set_list()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::LIST);
  }

}

void node::set_vec2f()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::VEC2F);
  }

}

void node::set_vec3f()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::VEC3F);
  }

}

void node::set_vec4f()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = m_node->create_parameter(ossia::val_type::VEC4F);
  }

}

void node::set_string()
{
  if (m_node)
  {
     m_node->remove_parameter();
     m_param = m_node->create_parameter(ossia::val_type::STRING);
  }

}

void node::set_buffer()
{
  if (m_node)
  {
      m_node->remove_parameter();
      m_param = ossia::setup_parameter(ossia::generic_buffer_type(), *m_node);
  }
}

void node::set_filepath()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::filesystem_path_type(), *m_node);
  }

}

void node::set_rgb()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::rgb_u{}, *m_node);
  }

}

void node::set_rgba()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::rgba_u{}, *m_node);
  }
}

void node::set_rgba8()
{
  if (m_node)
  {
    m_node->remove_parameter();
    ossia::setup_parameter(ossia::rgba8_u{}, *m_node);
  }
}

void node::set_argb()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::argb_u{}, *m_node);
  }

}

void node::set_argb8()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::argb8_u{}, *m_node);
  }

}

void node::set_hsv()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::hsv_u{}, *m_node);
  }

}

void node::set_cart2D()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::cartesian_2d_u{}, *m_node);
  }

}

void node::set_cart3D()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::cartesian_3d_u{}, *m_node);
  }

}

void node::set_opengl()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::opengl_u{}, *m_node);
  }

}

void node::set_polar()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::polar_u{}, *m_node);
  }

}

void node::set_spherical()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::spherical_u{}, *m_node);
  }

}

void node::set_cylindrical()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::cylindrical_u{}, *m_node);
  }

}

void node::set_angle_radian()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::radian_u{}, *m_node);
  }

}

void node::set_angle_degree()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::degree_u{}, *m_node);
  }

}


void node::set_quaternion()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::quaternion_u{}, *m_node);
  }

}

void node::set_euler()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::euler_u{}, *m_node);
  }


}

void node::set_axis()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::axis_u{}, *m_node);
  }

}

void node::set_decibel()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::decibel_u{}, *m_node);
  }

}

void node::set_midigain()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::midigain_u{}, *m_node);
  }

}

void node::set_linear()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::linear_u{}, *m_node);
  }

}

void node::set_frequency()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::frequency_u{}, *m_node);
  }

}

void node::set_midi_pitch()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::midi_pitch_u{}, *m_node);
  }

}

void node::set_bpm()
{
  if (m_node)
  {
    m_node->remove_parameter();
    m_param = ossia::setup_parameter(ossia::bpm_u{}, *m_node);
  }

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

node node::create_char(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::CHAR)};
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

node node::create_vec2f(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::VEC2F)};
  }

  return {};
}

node node::create_vec3f(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::VEC3F)};
  }

  return {};
}

node node::create_vec4f(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, n->create_parameter(ossia::val_type::VEC4F)};
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

node node::create_rgba8(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::rgba8_u{}, *n)};
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

node node::create_opengl(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::opengl_u{}, *n)};
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

node node::create_cylindrical(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::cylindrical_u{}, *n)};
  }

  return {};
}

node node::create_angle_radian(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::radian_u{}, *n)};
  }

  return {};
}

node node::create_angle_degree(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::degree_u{}, *n)};
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

node node::create_linear(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::linear_u{}, *n)};
  }

  return {};
}

node node::create_frequency(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::frequency_u{}, *n)};
  }

  return {};
}

node node::create_midi_pitch(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::midi_pitch_u{}, *n)};
  }

  return {};
}

node node::create_bpm(std::string addr)
{
  if (m_node)
  {
    auto n = &ossia::net::create_node(*m_node, addr);
    return node{n, ossia::setup_parameter(ossia::bpm_u{}, *n)};
  }

  return {};
}

node& node::set_value(value v)
{
  if (m_param)
    m_param->push_value(*v.m_val);
  return *this;
}

value node::get_value() const
{
  if (m_param)
    return m_param->value();
  return {};
}

value node::fetch_value() const
{
  if (m_param)
    return m_param->fetch_value();
  return {};
}

callback_index node::set_value_callback(value_callback c, void* ctx)
{
  if (m_param)
  {
    callback_index idx;
    idx.index = new callback_index::impl{m_param->add_callback([=] (const ossia::value& v) { c(ctx, v); })};
    return idx;
  }
  return {};
}

void node::remove_value_callback(callback_index idx)
{
  if (m_param && idx.index)
  {
    m_param->remove_callback(idx.index->iterator);
  }
}

node& node::set_access(access_mode v)
{
  if (m_param)
  {
    m_param->set_access(static_cast<ossia::access_mode>(v));
  }
  return *this;
}

access_mode node::get_access() const
{
  if (m_param)
  {
    return static_cast<opp::access_mode>(m_param->get_access());
  }
  return {};
}

node& node::set_min(value min)
{
  if (m_param)
  {
    if(auto dom = m_param->get_domain())
    {
      dom.set_min(std::move(*min.m_val));
      m_param->set_domain(std::move(dom));
    }
    else
    {
      m_param->set_domain(ossia::make_domain(*min.m_val, ossia::value{}));
    }
  }
  return *this;
}

value node::get_min() const
{
  if (m_param)
  {
    auto dom = m_param->get_domain();
    return ossia::get_min(dom);
  }
  return {};
}

node& node::set_max(value max)
{
  if (m_param)
  {
    if(auto dom = m_param->get_domain())
    {
      dom.set_max(std::move(*max.m_val));
      m_param->set_domain(std::move(dom));
    }
    else
    {
      m_param->set_domain(ossia::make_domain(ossia::value{}, *max.m_val));
    }
  }
  return *this;
}

value node::get_max() const
{
  if (m_param)
  {
    auto dom = m_param->get_domain();
    return ossia::get_max(dom);
  }
  return {};
}

node& node::set_accepted_values(std::vector<value> v)
{
  if (m_param)
  {
    auto dom = m_param->get_domain();

    std::vector<ossia::value> vals;
    for (const auto& val : v)
      vals.push_back(*val.m_val);

    ossia::set_values(dom, std::move(vals));

    m_param->set_domain(std::move(dom));
  }
  return *this;
}

std::vector<value> node::get_accepted_values() const
{
  if (m_param)
  {
    auto dom = m_param->get_domain();
    // TODO
    return {};
  }
  return {};
}

node& node::set_bounding(bounding_mode v)
{
  if (m_param)
  {
    m_param->set_bounding(static_cast<ossia::bounding_mode>(v));
  }
  return *this;
}

bounding_mode node::get_bounding() const
{
  if (m_param)
  {
    return static_cast<opp::bounding_mode>(m_param->get_bounding());
  }
  return {};

}

node& node::set_unit(std::string v)
{
  if (m_param)
  {
    m_param->set_unit(ossia::parse_pretty_unit(v));
  }
  return *this;
}

std::string node::get_unit() const
{
  if (m_param)
  {
    return ossia::get_pretty_unit_text(m_param->get_unit());
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

node& node::set_repetition_filter(bool v)
{
  if (m_param)
  {
    m_param->set_repetition_filter(
        v ? ossia::repetition_filter::ON : ossia::repetition_filter::OFF);
  }
  return *this;
}

bool node::get_repetition_filter() const
{
  if (m_param)
  {
    return m_param->get_repetition_filter();
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

bool node::get_zombie() const
{
  if (m_node)
  {
    return ossia::net::get_zombie(*m_node);
  }
  return {};
}

void node::save_preset(const std::string& f)
{
  try{
    if (m_node)
    {
      std::string ext(".json");
      if(f.size() > ext.size()
        && std::equal(f.begin() + f.size() - ext.size(), f.end(), ext.begin()))
      {
        auto json = ossia::presets::make_json_preset(*m_node);
        ossia::presets::write_file(json,f);
      }
      else
      {
        auto preset = ossia::presets::make_preset(*m_node);
        auto kiss = ossia::presets::to_string(preset);
        ossia::presets::write_file(kiss, f);
      }
    }
  } catch (const std::exception& e)
  {
    std::cerr << "can't make preset file '" << f
             << "', error: " << e.what() << std::endl;
  }
}
void node::load_preset(const std::string& f)
{
  try{
    if (m_node)
    {
      std::string ext(".json");
      auto buf = ossia::presets::read_file(f);

      if(f.size() > ext.size()
        && std::equal(f.begin() + f.size() - ext.size(), f.end(), ext.begin()))
      {
       ossia::presets::apply_json(buf,*m_node);
      }
      else
      {
        ossia::presets::apply_preset(buf, *m_node);
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "can't read preset file '" << f
              << "', error: " << e.what() << std::endl;
  }
}


oscquery_server::oscquery_server()
  : oscquery_server{"Ossia OSCQuery server"}
{
}

oscquery_server::oscquery_server(std::string name, int oscPort, int wsPort)
  : m_dev{}
  , m_con{}
  , m_con_ctx{}
  , m_discon{}
  , m_discon_ctx{}
{
  setup(std::move(name), oscPort, wsPort);
}

oscquery_server::~oscquery_server()
{
  try
  {
    using ossia::oscquery::oscquery_server_protocol;
    if(m_dev)
    {
      if(auto proto = dynamic_cast<oscquery_server_protocol*>(&m_dev->get_protocol()))
      {
        proto->onClientConnected.disconnect<&oscquery_server::on_connection>(*this);
        proto->onClientDisconnected.disconnect<&oscquery_server::on_disconnection>(*this);
      }

      delete m_dev;
    }
  }
  catch(const std::exception& e)
  {
  }
}

void oscquery_server::setup(std::string name, int oscPort, int wsPort)
{
  using ossia::oscquery::oscquery_server_protocol;

  try
  {
    if(m_dev)
    {
      if(auto proto = dynamic_cast<oscquery_server_protocol*>(&m_dev->get_protocol()))
      {
        proto->onClientConnected.disconnect<&oscquery_server::on_connection>(*this);
        proto->onClientDisconnected.disconnect<&oscquery_server::on_disconnection>(*this);
      }

      delete m_dev;
    }

    m_dev = new ossia::net::generic_device(
        std::make_unique<oscquery_server_protocol>(oscPort, wsPort),
        std::move(name));
    if(auto proto = dynamic_cast<oscquery_server_protocol*>(&m_dev->get_protocol()))
    {
      proto->onClientConnected.connect<&oscquery_server::on_connection>(*this);
      proto->onClientDisconnected.connect<&oscquery_server::on_disconnection>(*this);
    }
  }
  catch(const std::exception& e)
  {
  }
}

bool oscquery_server::connected() const
{
  return bool(m_dev);
}

node oscquery_server::get_root_node() const
{
  return m_dev ? node{&m_dev->get_root_node()} : node{};
}

void oscquery_server::set_echo(bool echo)
{
  try
    {
      if(m_dev)
      {
      using ossia::oscquery::oscquery_server_protocol;
      if(auto proto = dynamic_cast<oscquery_server_protocol*>(&m_dev->get_protocol())){
       proto->set_echo(echo);
      }
    }
  }
  catch(const std::exception& e)
  {
  }
}

bool oscquery_server::get_echo()
{
  try
  {
    if(m_dev)
    {
      using ossia::oscquery::oscquery_server_protocol;
      if(auto proto = dynamic_cast<oscquery_server_protocol*>(&m_dev->get_protocol()))
        return proto->echo();
    }
  }
  catch(const std::exception& e)
  {
  }
  return false;
}

void oscquery_server::set_connection_callback(connection_callback c, void* ctx)
{
  m_con = c;
  m_con_ctx = ctx;
}

void oscquery_server::remove_connection_callback()
{
  set_connection_callback(nullptr, nullptr);
}

void oscquery_server::on_connection(const std::string& str)
{
  if(m_con)
  {
    m_con(m_con_ctx, str);
  }
}

void oscquery_server::set_disconnection_callback(disconnection_callback c, void* ctx)
{
  m_discon = c;
  m_discon_ctx = ctx;
}

void oscquery_server::remove_disconnection_callback()
{
  set_disconnection_callback(nullptr, nullptr);
}

void oscquery_server::on_disconnection(const std::string& str)
{
  if(m_discon)
  {
    m_discon(m_discon_ctx, str);
  }
}

oscquery_mirror::oscquery_mirror(std::string name, std::string host)
  : m_dev{}
  , m_param_cb{}
  , m_param_ctx{}
  , m_rm_param_cb{}
  , m_rm_param_ctx{}
  , m_node_cb{}
  , m_node_ctx{}
  , m_rm_node_cb{}
  , m_rm_node_ctx{}
  , m_name{name}
  , m_host{host}
{
  try
  {
    m_dev = new ossia::net::generic_device(
              std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(host), name);
    m_dev->on_parameter_created.connect<&oscquery_mirror::on_parameter_created>(*this);
    m_dev->on_parameter_removing.connect<&oscquery_mirror::on_parameter_removed>(*this);
    m_dev->on_node_created.connect<&oscquery_mirror::on_node_created>(*this);
    m_dev->on_node_removing.connect<&oscquery_mirror::on_node_removed>(*this);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Can't connect to oscquery device '" << name << "': " << e.what() << std::endl;
  }
}

oscquery_mirror::~oscquery_mirror()
{
  if(m_dev)
  {
    m_dev->on_parameter_created.disconnect<&oscquery_mirror::on_parameter_created>(*this);
    m_dev->on_parameter_removing.disconnect<&oscquery_mirror::on_parameter_removed>(*this);
    m_dev->on_node_created.disconnect<&oscquery_mirror::on_node_created>(*this);
    m_dev->on_node_removing.disconnect<&oscquery_mirror::on_node_removed>(*this);
    delete m_dev;
  }
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

void oscquery_mirror::update()
{
  if(m_dev)
    static_cast<ossia::oscquery::oscquery_mirror_protocol&>(
        m_dev->get_protocol()).run_commands();
}

void oscquery_mirror::reconnect(std::string name, std::string host)
{
  if (m_dev)
  {
    delete m_dev;
    m_dev=nullptr;
  }

  if(name == "" && host == "")
  {
    name = m_name;
    host = m_host;
  }

  try
  {
    m_dev = new ossia::net::generic_device(
              std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(host), name);
    m_dev->on_parameter_created.connect<&oscquery_mirror::on_parameter_created>(*this);
    m_dev->on_parameter_removing.connect<&oscquery_mirror::on_parameter_removed>(*this);
    m_dev->on_node_created.connect<&oscquery_mirror::on_node_created>(*this);
    m_dev->on_node_removing.connect<&oscquery_mirror::on_node_removed>(*this);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Can't connect to oscquery device '" << name << "': " << e.what() << std::endl;
  }
}

void oscquery_mirror::set_parameter_created_callback(parameter_callback c, void* ctx)
{
  m_param_cb = c;
  m_param_ctx = ctx;
}

void oscquery_mirror::remove_parameter_created_callback()
{
  set_parameter_created_callback(nullptr, nullptr);
}

void oscquery_mirror::on_parameter_created(const ossia::net::parameter_base& param)
{
  if(m_param_cb)
  {
    m_param_cb(m_param_ctx, node{&param.get_node()});
  }
}

void oscquery_mirror::set_parameter_removed_callback(parameter_callback c, void* ctx)
{
  m_rm_param_cb = c;
  m_rm_param_ctx = ctx;
}

void oscquery_mirror::remove_parameter_removed_callback()
{
  set_parameter_removed_callback(nullptr, nullptr);
}

void oscquery_mirror::on_parameter_removed(const ossia::net::parameter_base& param)
{
  if(m_rm_param_cb)
  {
    m_rm_param_cb(m_rm_param_ctx, node{&param.get_node()});
  }
}

void oscquery_mirror::set_node_created_callback(node_callback c, void* ctx)
{
  m_node_cb = c;
  m_node_ctx = ctx;
}

void oscquery_mirror::remove_node_created_callback()
{
  set_node_created_callback(nullptr, nullptr);
}

void oscquery_mirror::on_node_created(const ossia::net::node_base& n)
{
  if(m_node_cb)
  {
    m_node_cb(m_node_ctx, opp::node{n.get_parent()->find_child(n.get_name())});
  }
}

void oscquery_mirror::set_node_removed_callback(node_callback c, void* ctx)
{
  m_rm_node_cb= c;
  m_rm_node_ctx = ctx;
}

void oscquery_mirror::remove_node_removed_callback()
{
  set_node_removed_callback(nullptr, nullptr);
}

void oscquery_mirror::on_node_removed(const ossia::net::node_base& n)
{
  if(m_rm_node_cb)
  {
    m_rm_node_cb(m_rm_node_ctx, opp::node{n.get_parent()->find_child(n.get_name())});
  }
}
}
