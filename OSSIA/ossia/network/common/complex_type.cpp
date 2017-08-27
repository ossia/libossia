// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/apply.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/dataspace/dataspace_base_fwd.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/editor/dataspace/detail/dataspace_parse.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <boost/algorithm/string.hpp>
namespace ossia
{

struct val_type_visitor
{
  using ret = ossia::val_type;
  ret operator()(ossia::val_type v) const
  {
    return v;
  }
  ret operator()(const ossia::unit_t& v) const
  {
    return ossia::matching_type(v);
  }
  ret operator()(const ossia::extended_type& v) const
  {
    auto t = ossia::underlying_type(v);
    if (!t.empty())
    {
      return t[0];
    }
    return ossia::val_type::IMPULSE;
  }

  ret operator()()
  {
    return ossia::val_type::IMPULSE;
  }
};

ossia::val_type underlying_type(const complex_type& t)
{
  return ossia::apply(val_type_visitor{}, t);
}

struct setup_parameter_visitor
{
  using ret = ossia::net::parameter_base*;
  ossia::net::node_base& n;
  ret operator()(ossia::val_type v) const
  {
    return n.create_parameter(v);
  }
  ret operator()(const ossia::unit_t& v) const
  {
    auto addr = n.create_parameter(ossia::matching_type(v));
    addr->set_unit(v);
    return addr;
  }
  ret operator()(const ossia::extended_type& v) const
  {
    auto t = ossia::underlying_type(v);
    if (!t.empty())
    {
      auto addr = n.create_parameter(t[0]);
      ossia::net::set_extended_type(n, v);
      return addr;
    }
    return nullptr;
  }

  ret operator()()
  {
    return nullptr;
  }
};

net::parameter_base* setup_parameter(const complex_type& t, net::node_base& node)
{
  if (!t)
    return nullptr;

  return ossia::apply(setup_parameter_visitor{node}, t);
}


static void list_units(std::unordered_map<std::string, net::parameter_data>& map)
{
  brigand::for_each<dataspace_u_list>([&](auto t) {
    ossia::net::parameter_data p;
    using dataspace_type = typename decltype(t)::type;
    using d_traits = dataspace_traits<dataspace_type>;
    for(auto dn : d_traits::text())
    {
      std::string dataspace_name(dn);
      using neutral_u = typename d_traits::neutral_unit;
      neutral_u neutral;
      p.unit = neutral;
      map.insert({dataspace_name, p});

      brigand::for_each<dataspace_type>([&](auto u) {
        using unit_type = typename decltype(u)::type;

        for (auto un : unit_traits<unit_type>::text())
        {
          // Add the unit in short form and long
          p.unit = unit_type{};
          map.insert({dataspace_name + "." + std::string(un), p});
          map.insert({std::string(un), p});
        }
      });
    }
  });
}

const std::unordered_map<std::string, net::parameter_data>& parameter_creation_map()
{
  static auto map = [] {
    std::unordered_map<std::string, net::parameter_data> t;
    list_units(t);

    auto add_simple = [&] (std::string s, ossia::val_type typ) {
      net::parameter_data p; p.type = typ; t.insert({std::move(s), p});
    };

    add_simple("int", ossia::val_type::INT);
    add_simple("integer", ossia::val_type::INT);
    add_simple("int32", ossia::val_type::INT);
    add_simple("i32", ossia::val_type::INT);

    add_simple("float", ossia::val_type::FLOAT);
    add_simple("single", ossia::val_type::FLOAT);
    add_simple("number", ossia::val_type::FLOAT);
    add_simple("num", ossia::val_type::FLOAT);
    add_simple("decimal", ossia::val_type::FLOAT);
    add_simple("flt", ossia::val_type::FLOAT);
    add_simple("float32", ossia::val_type::FLOAT);
    add_simple("ieee754", ossia::val_type::FLOAT);
    add_simple("real", ossia::val_type::FLOAT);
    add_simple("width", ossia::val_type::FLOAT);
    add_simple("length", ossia::val_type::FLOAT);
    add_simple("height", ossia::val_type::FLOAT);

    add_simple("string", ossia::val_type::STRING);
    add_simple("str", ossia::val_type::STRING);
    add_simple("symbol", ossia::val_type::STRING);
    add_simple("font", ossia::val_type::STRING);
    add_simple("label", ossia::val_type::STRING);
    add_simple("url", ossia::val_type::STRING);

    add_simple("bool", ossia::val_type::BOOL);
    add_simple("boolean", ossia::val_type::BOOL);

    add_simple("char", ossia::val_type::CHAR);
    add_simple("character", ossia::val_type::CHAR);
    add_simple("byte", ossia::val_type::CHAR);

    add_simple("vec2", ossia::val_type::VEC2F);
    add_simple("vec3", ossia::val_type::VEC3F);
    add_simple("vec4", ossia::val_type::VEC4F);
    add_simple("vector2", ossia::val_type::VEC2F);
    add_simple("vector3", ossia::val_type::VEC3F);
    add_simple("vector4", ossia::val_type::VEC4F);
    add_simple("vec2f", ossia::val_type::VEC2F);
    add_simple("vec3f", ossia::val_type::VEC3F);
    add_simple("vec4f", ossia::val_type::VEC4F);
    add_simple("size", ossia::val_type::VEC2F);
    add_simple("dim", ossia::val_type::VEC2F);
    add_simple("dims", ossia::val_type::VEC2F);
    add_simple("rect", ossia::val_type::VEC4F);
    add_simple("rectangle", ossia::val_type::VEC4F);

    add_simple("pulse", ossia::val_type::IMPULSE);
    add_simple("impulse", ossia::val_type::IMPULSE);
    add_simple("infinitum", ossia::val_type::IMPULSE);
    add_simple("bang", ossia::val_type::IMPULSE);

    auto add_ext = [&] (auto e) {
      net::parameter_data p; p.type = e; t.insert({e, p});
    };
    add_ext(generic_buffer_type());
    add_ext(filesystem_path_type());
    add_ext(float_array_type());
    add_ext(float_list_type());
    add_ext(integer_list_type());
    add_ext(string_list_type());
    add_ext(list_type());

    auto add_ext_2 = [&] (auto e, auto ext) {
      net::parameter_data p; p.type = ext; t.insert({e, p});
    };
    add_ext_2("path", filesystem_path_type());
    add_ext_2("file", filesystem_path_type());
    add_ext_2("folder", filesystem_path_type());
    add_ext_2("directory", filesystem_path_type());

    add_ext_2("bytearray", generic_buffer_type());
    add_ext_2("blob", generic_buffer_type());

    add_ext_2("array", list_type());
    add_ext_2("list", list_type());
    add_ext_2("tuple", list_type());
    add_ext_2("vector", list_type());
    add_ext_2("generic", list_type());

    auto add_u = [&] (auto e, auto u) {
      net::parameter_data p; p.type = u; t.insert({e, p});
    };

    add_u("complex", cartesian_2d_u{});
    add_u("point2d", ossia::cartesian_2d_u{});
    add_u("2d", ossia::cartesian_2d_u{});
    add_u("cartesian2d", ossia::cartesian_2d_u{});
    add_u("pos",   ossia::cartesian_3d_u{});
    add_u("point",   ossia::cartesian_3d_u{});
    add_u("point3d", ossia::cartesian_3d_u{});
    add_u("3d", ossia::cartesian_3d_u{});
    add_u("cartesian3d", ossia::cartesian_3d_u{});
    add_u("coord", ossia::cartesian_3d_u{});
    add_u("coordinate", ossia::cartesian_3d_u{});
    add_u("coordinates", ossia::cartesian_3d_u{});
    add_u("pvector", ossia::cartesian_3d_u{});
    add_u("vertex",  ossia::cartesian_3d_u{});
    add_u("gl",  ossia::opengl_u{});
    add_u("opengl",  ossia::opengl_u{});
    add_u("position.gl",  ossia::opengl_u{});
    add_u("position.opengl",  ossia::opengl_u{});

    return t;
  }();
  return map;
}

net::parameter_base* try_setup_parameter(std::string str, net::node_base& node)
{
  auto& map = parameter_creation_map();
  boost::algorithm::to_lower(str);
  auto it = map.find(str);
  if(it != map.end())
    return setup_parameter(it->second.type, node);
  return nullptr;
}

struct update_parameter_visitor
{
  using ret = void;
  ossia::net::parameter_base& addr;
  ret operator()(ossia::val_type v) const
  {
    addr.set_value_type(v);
  }
  ret operator()(const ossia::unit_t& v) const
  {
    addr.set_unit(v);
  }
  ret operator()(const ossia::extended_type& v) const
  {
    auto t = ossia::underlying_type(v);
    if (!t.empty())
    {
      addr.set_value_type(t[0]);
    }
    ossia::net::set_extended_type(addr.get_node(), v);
  }

  ret operator()()
  {
  }
};

void update_parameter_type(const complex_type& t, net::parameter_base& addr)
{
  ossia::apply(update_parameter_visitor{addr}, t);
}
}
