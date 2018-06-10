// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/for_each.hpp>
#include <brigand/algorithms/transform.hpp>
#include <brigand/sequences/list.hpp>
#include <ossia/network/value/impulse.hpp>
#include <ossia/network/value/vec.hpp>
#include <ossia/network/dataspace/dataspace_base_defs_fwd.hpp>
#include <fmt/ostream.h>
namespace ossia
{
struct behavior {};
struct curve_abstract {};
struct Destination { std::string v; };
template<typename T>
struct domain_base { std::string v; };

struct vector_domain { std::string v; };
template<int N>
struct vecf_domain { std::string v; };
}
namespace gen_variant
{
using str_writer = fmt::MemoryWriter;
template<typename T>
struct var_member
{

};

#define UNIT_VARIANT_DEF(Text) \
template<> struct var_member<ossia::Text ## _u> \
{ \
  using type = ossia::Text ## _u; \
  const std::string type_str = "ossia::" # Text "_u"; \
  const std::string ctor_str = ""; \
  static const constexpr bool is_trivial = true; \
}; \
template<> struct var_member<ossia::Text> \
{ \
  using type = ossia::Text; \
  const std::string type_str = "ossia::" # Text; \
  const std::string ctor_str = ""; \
  static const constexpr bool is_trivial = true; \
};
UNIT_VARIANT_DEF(radian)
UNIT_VARIANT_DEF(degree)

UNIT_VARIANT_DEF(argb)
UNIT_VARIANT_DEF(rgba)
UNIT_VARIANT_DEF(rgb)
UNIT_VARIANT_DEF(bgr)
UNIT_VARIANT_DEF(argb8)
UNIT_VARIANT_DEF(hsv)
UNIT_VARIANT_DEF(cmy8)
UNIT_VARIANT_DEF(xyz)

UNIT_VARIANT_DEF(meter)
UNIT_VARIANT_DEF(kilometer)
UNIT_VARIANT_DEF(decimeter)
UNIT_VARIANT_DEF(centimeter)
UNIT_VARIANT_DEF(millimeter)
UNIT_VARIANT_DEF(micrometer)
UNIT_VARIANT_DEF(nanometer)
UNIT_VARIANT_DEF(picometer)
UNIT_VARIANT_DEF(inch)
UNIT_VARIANT_DEF(foot)
UNIT_VARIANT_DEF(mile)

UNIT_VARIANT_DEF(linear)
UNIT_VARIANT_DEF(midigain)
UNIT_VARIANT_DEF(decibel)
UNIT_VARIANT_DEF(decibel_raw)

UNIT_VARIANT_DEF(quaternion)
UNIT_VARIANT_DEF(euler)
UNIT_VARIANT_DEF(axis)

UNIT_VARIANT_DEF(cartesian_3d)
UNIT_VARIANT_DEF(cartesian_2d)
UNIT_VARIANT_DEF(spherical)
UNIT_VARIANT_DEF(polar)
UNIT_VARIANT_DEF(opengl)
UNIT_VARIANT_DEF(cylindrical)

UNIT_VARIANT_DEF(meter_per_second)
UNIT_VARIANT_DEF(miles_per_hour)
UNIT_VARIANT_DEF(kilometer_per_hour)
UNIT_VARIANT_DEF(knot)
UNIT_VARIANT_DEF(foot_per_second)
UNIT_VARIANT_DEF(foot_per_hour)

UNIT_VARIANT_DEF(second)
UNIT_VARIANT_DEF(bark)
UNIT_VARIANT_DEF(bpm)
UNIT_VARIANT_DEF(cent)
UNIT_VARIANT_DEF(frequency)
UNIT_VARIANT_DEF(mel)
UNIT_VARIANT_DEF(midi_pitch)
UNIT_VARIANT_DEF(millisecond)
UNIT_VARIANT_DEF(playback_speed)

// Dataspaces
UNIT_VARIANT_DEF(angle)
UNIT_VARIANT_DEF(color)
UNIT_VARIANT_DEF(distance)
UNIT_VARIANT_DEF(gain)
UNIT_VARIANT_DEF(orientation)
UNIT_VARIANT_DEF(position)
UNIT_VARIANT_DEF(speed)
UNIT_VARIANT_DEF(timing)

template<> struct var_member<ossia::domain_base<ossia::impulse>>
{
  using type = ossia::domain_base<ossia::impulse>;
  const std::string type_str = "ossia::domain_base<ossia::impulse>";
  const std::string ctor_str = "domain_base";
  static const constexpr bool is_trivial = false;
};

template<> struct var_member<ossia::domain_base<int>>
{
  using type = ossia::domain_base<int>;
  const std::string type_str = "ossia::domain_base<int>";
  const std::string ctor_str = "domain_base";
  static const constexpr bool is_trivial = false;
};

template<> struct var_member<ossia::domain_base<float>>
{
  using type = ossia::domain_base<float>;
  const std::string type_str = "ossia::domain_base<float>";
  const std::string ctor_str = "domain_base";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::domain_base<bool>>
{
  using type = ossia::domain_base<bool>;
  const std::string type_str = "ossia::domain_base<bool>";
  const std::string ctor_str = "domain_base";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::domain_base<char>>
{
  using type = ossia::domain_base<char>;
  const std::string type_str = "ossia::domain_base<char>";
  const std::string ctor_str = "domain_base";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::domain_base<std::string>>
{
  using type = ossia::domain_base<std::string>;
  const std::string type_str = "ossia::domain_base<std::string>";
  const std::string ctor_str = "domain_base";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::vector_domain>
{
  using type = ossia::vector_domain;
  const std::string type_str = "ossia::vector_domain";
  const std::string ctor_str = "vector_domain";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::vecf_domain<2>>
{
  using type = ossia::vecf_domain<2>;
  const std::string type_str = "ossia::vecf_domain<2>";
  const std::string ctor_str = "vecf_domain";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::vecf_domain<3>>
{
  using type = ossia::vecf_domain<3>;
  const std::string type_str = "ossia::vecf_domain<3>";
  const std::string ctor_str = "vecf_domain";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::vecf_domain<4>>
{
  using type = ossia::vecf_domain<4>;
  const std::string type_str = "ossia::vecf_domain<4>";
  const std::string ctor_str = "vecf_domain";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::domain_base<ossia::value>>
{
  using type = ossia::domain_base<ossia::value>;
  const std::string type_str = "ossia::domain_base<ossia::value>";
  const std::string ctor_str = "domain_base";
  static const constexpr bool is_trivial = false;
};

template<> struct var_member<std::shared_ptr<ossia::curve_abstract>>
{
  using type = std::shared_ptr<ossia::curve_abstract>;
  const std::string type_str = "std::shared_ptr<ossia::curve_abstract>";
  const std::string ctor_str = "shared_ptr";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<std::vector<ossia::behavior>>
{
  using type = float;
  const std::string type_str = "std::vector<ossia::behavior>";
  const std::string ctor_str = "vector";
  static const constexpr bool is_trivial = false;
};

template<> struct var_member<float>
{
  using type = float;
  const std::string type_str = "float";
  const std::string ctor_str = "";
  static const constexpr bool is_trivial = true;
};
template<> struct var_member<int>
{
  using type = int;
  const std::string type_str = "int";
  const std::string ctor_str = "";
  static const constexpr bool is_trivial = true;
};
template<> struct var_member<ossia::vec2f>
{
  using type = ossia::vec2f;
  const std::string type_str = "ossia::vec2f";
  const std::string ctor_str = "";
  static const constexpr bool is_trivial = true;
};
template<> struct var_member<ossia::vec3f>
{
  using type = ossia::vec3f;
  const std::string type_str = "ossia::vec3f";
  const std::string ctor_str = "";
  static const constexpr bool is_trivial = true;
};
template<> struct var_member<ossia::vec4f>
{
  using type = ossia::vec4f;
  const std::string type_str = "ossia::vec4f";
  const std::string ctor_str = "";
  static const constexpr bool is_trivial = true;
};
template<> struct var_member<std::vector<ossia::value>>
{
  using type = std::vector<ossia::value>;
  const std::string type_str = "std::vector<ossia::value>";
  const std::string ctor_str = "vector<ossia::value>";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::impulse>
{
  using type = ossia::impulse;
  const std::string type_str = "ossia::impulse";
  const std::string ctor_str = "";
  static const constexpr bool is_trivial = true;
};
template<> struct var_member<bool>
{
  using type = bool;
  const std::string type_str = "bool";
  const std::string ctor_str = "";
  static const constexpr bool is_trivial = true;
};
template<> struct var_member<std::string>
{
  using type = std::string;
  const std::string type_str = "std::string";
  const std::string ctor_str = "basic_string";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<char>
{
  using type = char;
  const std::string type_str = "char";
  const std::string ctor_str = "";
  static const constexpr bool is_trivial = true;
};
template<> struct var_member<ossia::Destination>
{
  using type = ossia::Destination;
  const std::string type_str = "ossia::Destination";
  const std::string ctor_str = "Destination";
  static const constexpr bool is_trivial = false;
};
template<> struct var_member<ossia::value>
{
  using type = ossia::value;
  const std::string type_str = "ossia::value";
  const std::string ctor_str = "value";
  static const constexpr bool is_trivial = false;
};

template<typename... Args>
struct gen_var
{
  gen_var(std::string n): class_name{n} {}
  std::string class_name;
  std::string constexpr_token = "";

  str_writer str;
  using var_type = brigand::list<Args...>;

  using var_impl = brigand::transform<var_type, brigand::bind<var_member, brigand::_1>>;
  static const constexpr int num_types = sizeof...(Args);

  void write_enum()
  {
    str << "enum Type: int8_t { \n";

    for(int i = 0; i < num_types; i++)
      str << "Type" << i << ", ";

    str << "Npos = std::numeric_limits<int8_t>::max()\n";
    str << "};\n";
  }

  void write_union()
  {
    str << "struct dummy_t {};\n";
    str << "union Impl { \n";

    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      using impl_t = typename meta_t::type;
      meta_t t;

      str << t.type_str << " m_value" << i << ";\n";
/*
      // Constructor
      if(t.is_trivial)
      {
        str << constexpr_token << " ";
      }

      if(t.is_trivial)
      {
        str << "Impl(" << t.type_str << " v): m_value" << i << "{v} { }\n";
      }
      else
      {
        str << "Impl(const " << t.type_str << "& v): m_value" << i << "{v} { }\n";
        str << "Impl(" << t.type_str << "&& v): m_value" << i << "{std::move(v)} { }\n";
      }

      // operator=
      if(t.is_trivial)
      {
        str << "Impl& operator=(" << t.type_str << " v) { m_value" << i << " = v; return *this; }\n";
      }
      else
      {
        str << "Impl& operator=(const " << t.type_str << "& v) { m_value" << i << " = v; return *this; }\n";
        str << "Impl& operator=(" << t.type_str << "&& v) { m_value" << i << " = std::move(v); return *this; }\n";
      }
*/
      str << "\n";
      i++;
    });

    str << "dummy_t m_dummy;\n"
           "Impl(): m_dummy{} { }\n"
           "~Impl() { }\n"
           "};\n";
  }

  void write_destruct()
  {
    str << "void destruct_impl() { \n";
    str << "switch(m_type) { \n";
    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      if(!t.is_trivial)
      {
        str << "  case Type::Type" << i << ":\n";
        str << "    m_impl.m_value" << i << ".~" << t.ctor_str << "();\n";
        str << "    break;\n";
      }
      i++;
    });

    str << "  default: break; \n } \n }";

  }

  void write_assign_switch(std::string orn_before, std::string orn_after)
  {
    // Write types
    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      str << "  case Type::Type" << i << ":\n";
      str << "    new(&m_impl.m_value" << i << ") " << t.type_str << "{" << orn_before << "other.m_impl.m_value" << i << orn_after << "};\n";
      str << "    break;\n";

      i++;
    });
  }

  void write_comp_switch(std::string comp)
  {
    for(int i = 0; i < num_types; i++) {
      str << "    case " << class_name << "::Type::Type" << i << ":\n";
      str << "      return lhs.m_impl.m_value" << i << comp << "rhs.m_impl.m_value" << i << ";\n";
    }
  }

  void write_typeonly_comparison_operators()
  {
    // Generic ones between two variants
    {
      str << "inline bool operator==(const " << class_name << "& lhs, const " << class_name << "& rhs)"
          << "{ \n"
             "  return (lhs.m_type == rhs.m_type);\n";
      str << "}\n";
    }

    {
      str << "inline bool operator!=(const " << class_name << "& lhs, const " << class_name << "& rhs)"
          << "{ \n"
             "  return (lhs.m_type != rhs.m_type);\n";
      str << "}\n";
    }

    // Then specific ones between an instance of the variant and an instance of an actual type
    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      // ==
      {
        str << "inline bool operator==(const " << class_name << "& lhs, const " << t.type_str << "& rhs)"
               "{ \n"
               "  return (lhs.m_type == " << class_name << "::Type::Type" << i << "); \n";
        str << "}\n";
      }

      {
        str << "inline bool operator==(const " << t.type_str << "& lhs, const " << class_name << "& rhs)"
               "{ \n"
               "  return (rhs.m_type == " << class_name << "::Type::Type" << i << "); \n";
        str << "}\n";
      }

      // !=
      {
        str << "inline bool operator!=(const " << class_name << "& lhs, const " << t.type_str << "& rhs)"
               "{ \n"
               "  return (lhs.m_type != " << class_name << "::Type::Type" << i << "); \n";
        str << "}\n";
      }

      {
        str << "inline bool operator!=(const " << t.type_str << "& lhs, const " << class_name << "& rhs)"
               "{ \n"
               "  return (rhs.m_type != " << class_name << "::Type::Type" << i << "); \n";
        str << "}\n";
      }

      i++;
    });
  }

  void write_comparison_operators()
  {
    // Generic ones between two variants
    {
      str << "inline bool operator==(const " << class_name << "& lhs, const " << class_name << "& rhs)"
          << "{ \n"
             "  if(lhs.m_type == rhs.m_type) { \n"
             "    switch(lhs.m_type) { \n";

      write_comp_switch("==");

      str << "      default: return true;\n";
      str << "    }\n";
      str << "  }\n";
      str << "  return false; \n";
      str << "}\n";
    }

    {
      str << "inline bool operator!=(const " << class_name << "& lhs, const " << class_name << "& rhs)"
          << "{ \n"
             "  if(lhs.m_type != rhs.m_type) return true; \n"
             "  switch(lhs.m_type) { \n";

      write_comp_switch("!=");

      str << "    default: return false;\n";
      str << "  }\n";
      str << "  return true; \n";
      str << "}\n";
    }

    // Then specific ones between an instance of the variant and an instance of an actual type
    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      // ==
      {
        str << "inline bool operator==(const " << class_name << "& lhs, const " << t.type_str << "& rhs)"
               "{ \n"
               "  return (lhs.m_type == " << class_name << "::Type::Type" << i << ") && (lhs.m_impl.m_value" << i <<" == rhs); \n";
        str << "}\n";
      }

      {
        str << "inline bool operator==(const " << t.type_str << "& lhs, const " << class_name << "& rhs)"
               "{ \n"
               "  return (rhs.m_type == " << class_name << "::Type::Type" << i << ") && (rhs.m_impl.m_value" << i <<" == lhs); \n";
        str << "}\n";
      }

      // !=
      {
        str << "inline bool operator!=(const " << class_name << "& lhs, const " << t.type_str << "& rhs)"
               "{ \n"
               "  return (lhs.m_type != " << class_name << "::Type::Type" << i << ") || (lhs.m_impl.m_value" << i <<" != rhs); \n";
        str << "}\n";
      }

      {
        str << "inline bool operator!=(const " << t.type_str << "& lhs, const " << class_name << "& rhs)"
               "{ \n"
               "  return (rhs.m_type != " << class_name << "::Type::Type" << i << ") || (rhs.m_impl.m_value" << i <<" != lhs); \n";
        str << "}\n";
      }

      i++;
    });
  }

  void write_copy_constructor()
  {
    // Copy
    {
      str << class_name << "(const " << class_name
          << "& other):\n"
             " m_type{other.m_type} { \n"
             "  switch(m_type) { \n";

      write_assign_switch({}, {});
      str << "    default: break;\n";
      str << "  }\n";
      str << "}\n";
    }

    // Move
    {
      str << class_name << "(" << class_name
          << "&& other):\n"
             "m_type{other.m_type} { \n"
             "  switch(m_type) { \n";

      write_assign_switch("std::move(", ")");

      str << "    default: break;\n";
      str << "  }\n";
      str << "}\n";
    }

    // Assign Copy
    {
      str << class_name << "& operator=(const " << class_name
          << "& other)"
             "{ \n"
             "  destruct_impl(); \n"
             "  m_type = other.m_type;\n"
             "  switch(m_type) { \n";

      write_assign_switch({}, {});
      str << "    default: break;\n";
      str << "  }\n";
      str << "  return *this;\n";
      str << "}\n";
    }

    // Assign Move
    {
      str << class_name << "& operator=(" << class_name
          << "&& other)\n"
             "{ \n"
             "  destruct_impl(); \n"
             "  m_type = other.m_type;\n"
             "  switch(m_type) { \n";

      write_assign_switch("std::move(", ")");

      str << "    default: break;\n";
      str << "  }\n";
      str << "  return *this;\n";
      str << "}\n";
    }
  }


  void write_constructor()
  {
    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      using impl_t = typename meta_t::type;
      meta_t t;

      if(t.is_trivial)
      {
        str << constexpr_token << " ";
      }

      if(t.is_trivial)
      {
        str << class_name << "(" << t.type_str << " v): m_type{Type" << i << "} { \n";
        str << "  new(&m_impl.m_value" << i << ") " << t.type_str << "{v};\n";
        str << "}\n";
      }
      else
      {
        str << class_name << "(const " << t.type_str << "& v): m_type{Type" << i << "} { \n";
        str << "  new(&m_impl.m_value" << i << ") " << t.type_str << "{v};\n";
        str << "}\n";
        str << class_name << "(" << t.type_str << "&& v): m_type{Type" << i << "} { \n";
        str << "  new(&m_impl.m_value" << i << ") " << t.type_str << "{std::move(v)};\n";
        str << "}\n";
      }
      i++;
    });
  }


  void write_matching_type()
  {
    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      str << "template<> const Type matching_type<" << t.type_str << ">() const { \n";
      str << "  return Type" << i << ";\n";
      str << "}\n";
      i++;
    });

  }
  void write_target()
  {
    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      str << "template<> inline const " << t.type_str << "* "<< class_name<<"::target() const { \n";
      str << "  if(m_type == Type" << i << ") \n";
      str << "    return &m_impl.m_value" << i << " ;\n";
      str << "  return nullptr; \n";
      str << "}\n";
      i++;
    });

    i = 0;
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      str << "template<> inline " << t.type_str << "* "<< class_name<<"::target() { \n";
      str << "  if(m_type == Type" << i << ") \n";
      str << "    return &m_impl.m_value" << i << " ;\n";
      str << "  return nullptr; \n";
      str << "}\n";
      i++;
    });

    i = 0;
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      str << "template<> inline const " << t.type_str << "& "<< class_name<<"::get() const { \n";
      str << "  if(m_type == Type" << i << ") \n";
      str << "    return m_impl.m_value" << i << " ;\n";
      str << "  throw; \n";
      str << "}\n";
      i++;
    });

    i = 0;
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      using meta_t = typename decltype(_)::type;
      meta_t t;

      str << "template<> inline " << t.type_str << "& "<< class_name<<"::get() { \n";
      str << "  if(m_type == Type" << i << ") \n";
      str << "    return m_impl.m_value" << i << " ;\n";
      str << "  throw; \n";
      str << "}\n";
      i++;
    });
  }

  void write_class()
  {
    str << "struct " << class_name << "\n"
                                     "{\n"
                                     "public:\n";
    write_union();
    str << "\n";
    write_enum();
    str << "\n";
    write_destruct();
    str <<
R"_(
Impl m_impl;
Type m_type;

public:
static const constexpr auto npos = Npos;
int which() const { return m_type; }

operator bool() const { return m_type != npos; }
template<typename T>
const T* target() const;
template<typename T>
T* target();
template<typename T>
const T& get() const;
template<typename T>
T& get();

template<typename T>
static Type matching_type();
)_";

    str << class_name << "(): m_type{Npos} { }\n";
    str << "~" << class_name << "() { destruct_impl(); }\n";


    write_constructor();
    write_copy_constructor();

    str << "};\n";

    write_target();

    write_apply();
  }


  void write_apply_switch(std::string orn_before, std::string orn_after)
  {
    int i = 0;
    // Write types
    ossia::for_each_tagged(var_impl{}, [&] (auto _) {
      str << "  case "<< class_name <<"::Type::Type" << i << ":\n";
      str << "    return functor(" << orn_before << "var.m_impl.m_value" << i << orn_after << ");\n";
      i++;
    });
  }
  struct cref
  {
    std::string type_prefix = "const";
    std::string type_suffix = "&";
    std::string val_prefix = "";
    std::string val_suffix = "";
  };
  struct ref
  {
    std::string type_prefix = "";
    std::string type_suffix = "&";
    std::string val_prefix = "";
    std::string val_suffix = "";
  };
  struct rv_ref
  {
    std::string type_prefix = "";
    std::string type_suffix = "&&";
    std::string val_prefix = "std::move(";
    std::string val_suffix = ")";
  };

  template<typename Ref>
  void write_apply_nonnull_impl(Ref r)
  {
    str << "template<typename Visitor>\n";
    str << "auto apply_nonnull(Visitor&& functor, " << r.type_prefix << " " << class_name << r.type_suffix << " var) {\n";
    str << "  switch (var.m_type) { \n";
    write_apply_switch(r.val_prefix, r.val_suffix);
    str << "  default: throw;\n";
    str << "  }\n";
    str << "}\n";
  }

  template<typename Ref>
  void write_apply_impl(Ref r)
  {
    str << "template<typename Visitor>\n";
    str << "auto apply(Visitor&& functor, " << r.type_prefix << " " << class_name << r.type_suffix << " var) {\n";
    str << "  switch (var.m_type) { \n";
    write_apply_switch(r.val_prefix, r.val_suffix);
    str << "  default: return functor();\n";
    str << "  }\n";
    str << "}\n";
  }

  void write_apply()
  {
    write_apply_nonnull_impl(cref{});
    write_apply_nonnull_impl(ref{});
    write_apply_nonnull_impl(rv_ref{});

    write_apply_impl(cref{});
    write_apply_impl(ref{});
    write_apply_impl(rv_ref{});
  }

};

struct class_info
{
  std::string name;
  std::size_t num_args;
  enum ref_type { Cref, Ref, RvRef } ref;
};
struct apply_writer
{
  str_writer str;

  void write_apply_switch(std::size_t level, const std::vector<class_info>& vec, std::vector<std::string> args)
  {

    str << "switch(arg" << level << ".m_type) {\n";
    for(std::size_t i = 0; i < vec[level].num_args; i++)
    {
      str << "case " << vec[level].name << "::Type::Type" << i << ":\n{\n";
      if(level + 1 < vec.size())
      {
        auto args_next = args;
        args_next.push_back(fmt::format("arg{}.m_impl.m_value{}", level, i));

        write_apply_switch(level + 1, vec, std::move(args_next));
      }
      else
      {
        auto args_next = args;
        args_next.push_back(fmt::format("arg{}.m_impl.m_value{}", level, i));

        str << "return functor(";
        for(std::size_t arg = 0; arg < args_next.size(); arg++)
        {
          switch(vec[arg].ref)
          {
            case class_info::ref_type::Cref:
            case class_info::ref_type::Ref:
              str << args_next[arg];
              break;
            case class_info::ref_type::RvRef:
              str << "std::move(" << args_next[arg] << ")";
              break;
          }
          if(arg < args_next.size() - 1)
            str << ", ";
        }
        str << ");\n";
      }
      str << "}\n";
    }
    str << "default: throw; \n";
    str << "}\n";
  }

  void write_apply_switch(std::vector<class_info> vec)
  {
    str << "template<typename Functor>\nauto apply(Functor&& functor, ";
    for(std::size_t i = 0; i < vec.size(); i++)
    {
      switch(vec[i].ref)
      {
        case class_info::Cref:
          str << "const " << vec[i].name << "&";
          break;
        case class_info::Ref:
          str << vec[i].name << "&";
          break;
        case class_info::RvRef:
          str << vec[i].name << "&&";
          break;
      }

      str << " arg" << i;
      if(i < vec.size() - 1)
        str << ", ";
    }
    str << ")\n{\n";
    write_apply_switch(0, vec, {});
    str << "}\n";
  }

  void write_apply_permutations(std::vector<class_info> args)
  {
    // For each permutation of arguments, write them all with :
    // const-ref, non-const-ref, rvalue-ref

    std::vector<std::vector<class_info>> vecs;

    // a * 3^0 , b * 3^1 , c * 3^2 , ... , x * 3 ^ args.size()
  }
};

}
#include <fstream>
#include <iostream>

int main()
{
  using namespace gen_variant;

  static const constexpr int value_size = 10;
  // Behavior variant generation
  {
    std::ofstream f("/home/jcelerier/travail/score/API/OSSIA/ossia/editor/curve/behavior_variant_impl.hpp");
    gen_var<std::shared_ptr<ossia::curve_abstract>, std::vector<ossia::behavior>>
        behav_gen("behavior_variant_type");
    behav_gen.write_class();
    f << behav_gen.str.str();
  }

  // Domain variant generation
  {
    using namespace ossia;
    std::ofstream f("/home/jcelerier/travail/score/API/OSSIA/ossia/network/domain/domain_variant_impl.hpp");
    gen_var<domain_base<impulse>, domain_base<bool>, domain_base<int32_t>,
        domain_base<float>, domain_base<char>, domain_base<std::string>,
        vector_domain, vecf_domain<2>, vecf_domain<3>,
        vecf_domain<4>, domain_base<ossia::value>>
        domain_gen("domain_base_variant");
    domain_gen.write_class();
    domain_gen.write_comparison_operators();
    f << domain_gen.str.str();


    apply_writer r;
    r.write_apply_switch({ class_info{"domain_base_variant", 11, class_info::Ref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({  class_info{"value_variant_type", value_size, class_info::Cref},
                            class_info{"domain_base_variant", 11, class_info::Cref}
                         });
    f << r.str.str();
  }
  {

    // Value variant generation
    std::ofstream f("/home/jcelerier/score/API/OSSIA/ossia/network/value/value_variant_impl.hpp");

    gen_var<float, int, ossia::vec2f, ossia::vec3f, ossia::vec4f,
            ossia::impulse,
            bool, std::string, std::vector<ossia::value>,
            char>
        value_gen("value_variant_type");
    value_gen.write_class();
    f << value_gen.str.str();

    apply_writer r;
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Ref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Ref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::RvRef},
                           class_info{"value_variant_type", value_size, class_info::RvRef}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Ref},
                           class_info{"value_variant_type", value_size, class_info::Ref}
                         });

    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::RvRef},
                           class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    f << r.str.str();
  }

  {
    std::ofstream f("/home/jcelerier/score/API/OSSIA/ossia/misc_visitors.hpp");
    apply_writer r;
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"behavior_variant_type", 2, class_info::Cref}
                         });

    f << "#pragma once\n";
    f << "#include <ossia/network/value/value.hpp>\n";
    f << "#include <ossia/editor/curve/behavior.hpp>\n";
    f << "namespace ossia {\n";
    f << r.str.str();
    f << "}\n";
  }
  using namespace ossia;

  {
    std::ofstream f("/home/jcelerier/score/API/OSSIA/ossia/network/dataspace/dataspace_base_variants.hpp");
    {
      gen_var<degree_u, radian_u> u("angle_u");
      u.write_class();
      u.write_typeonly_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<argb_u, rgba_u, rgb_u, bgr_u, argb8_u, hsv_u, cmy8_u, xyz_u> u("color_u");
      u.write_class();
      u.write_typeonly_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<meter_u, kilometer_u, decimeter_u, centimeter_u, millimeter_u, micrometer_u, nanometer_u, picometer_u, inch_u, foot_u, mile_u>
          u("distance_u");
      u.write_class();
      u.write_typeonly_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<linear_u, midigain_u, decibel_u, decibel_raw_u>
          u("gain_u");
      u.write_class();
      u.write_typeonly_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<quaternion_u, euler_u, axis_u>
          u("orientation_u");
      u.write_class();
      u.write_typeonly_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<cartesian_3d_u, cartesian_2d_u, spherical_u, polar_u, opengl_u, cylindrical_u>
          u("position_u");
      u.write_class();
      u.write_typeonly_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<meter_per_second_u, miles_per_hour_u, kilometer_per_hour_u, knot_u, foot_per_second_u, foot_per_hour_u>
          u("speed_u");
      u.write_class();
      u.write_typeonly_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<second_u, bark_u, bpm_u, cent_u, frequency_u, mel_u, midi_pitch_u, millisecond_u, playback_speed_u>
          u("timing_u");
      u.write_class();
      u.write_typeonly_comparison_operators();
      f << u.str.str();
    }

    {
      gen_var<distance_u, position_u, speed_u, orientation_u, angle_u, color_u, gain_u, timing_u>
          u("unit_variant");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
  }
  {
    std::ofstream f("/home/jcelerier/score/API/OSSIA/ossia/network/dataspace/dataspace_strong_variants.hpp");
    // Strong value form
    {
      gen_var<degree, radian> u("angle");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<argb, rgba, rgb, bgr, argb8, hsv, cmy8, xyz> u("color");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<meter, kilometer, decimeter, centimeter, millimeter, micrometer, nanometer, picometer, inch, foot, mile>
          u("distance");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<linear, midigain, decibel, decibel_raw>
          u("gain");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<quaternion, euler, axis>
          u("orientation");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<cartesian_3d, cartesian_2d, spherical, polar, opengl, cylindrical>
          u("position");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<meter_per_second, miles_per_hour, kilometer_per_hour, knot, foot_per_second, foot_per_hour>
          u("speed");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
    {
      gen_var<second, bark, bpm, cent, frequency, mel, midi_pitch, millisecond, playback_speed>
          u("timing");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }

    // List of dataspaces

    {
      gen_var<ossia::value, distance, position, speed, orientation, angle, color, gain, ossia::timing>
          u("strong_value_variant");
      u.write_class();
      u.write_comparison_operators();
      f << u.str.str();
    }
  }

  {
    std::ofstream f("/home/jcelerier/score/API/OSSIA/ossia/network/dataspace/dataspace_variant_visitors.hpp");
    apply_writer r;
    r.write_apply_switch({ class_info{"strong_value_variant", 9, class_info::Cref},
                           class_info{"unit_variant", 8, class_info::Cref}
                         });

    // Value & unit_t
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"angle_u", 2, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"color_u", 8, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"distance_u", 11, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"gain_u", 4, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"orientation_u", 3, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"position_u", 6, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"speed_u", 6, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"value_variant_type", value_size, class_info::Cref},
                           class_info{"timing_u", 9, class_info::Cref}
                         });

    // Strong value & unit
    r.write_apply_switch({ class_info{"angle", 2, class_info::Cref},
                           class_info{"angle_u", 2, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"color", 8, class_info::Cref},
                           class_info{"color_u", 8, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"distance", 11, class_info::Cref},
                           class_info{"distance_u", 11, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"gain", 4, class_info::Cref},
                           class_info{"gain_u", 4, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"orientation", 3, class_info::Cref},
                           class_info{"orientation_u", 3, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"position", 6, class_info::Cref},
                           class_info{"position_u", 6, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"speed", 6, class_info::Cref},
                           class_info{"speed_u", 6, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"timing", 9, class_info::Cref},
                           class_info{"timing_u", 9, class_info::Cref}
                         });

    // Strong value & value
    r.write_apply_switch({ class_info{"angle", 2, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"color", 8, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"distance", 11, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"gain", 4, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"orientation", 3, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"position", 6, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"speed", 6, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });
    r.write_apply_switch({ class_info{"timing", 9, class_info::Cref},
                           class_info{"value_variant_type", value_size, class_info::Cref}
                         });


    f << "#pragma once\n";
    f << "#include <ossia/network/dataspace/dataspace.hpp>\n";
    f << "#include <ossia/network/dataspace/value_with_unit.hpp>\n";
    f << "namespace ossia {\n";
    f << r.str.str();
    f << "}\n";
  }

}
