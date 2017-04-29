struct value_variant_type
{
public:
struct dummy_t {};
union Impl { 
float m_value0;
 Impl(float v): m_value0{v} { }
Impl& operator=(float v) { m_value0 = v; return *this; }

int m_value1;
 Impl(int v): m_value1{v} { }
Impl& operator=(int v) { m_value1 = v; return *this; }

ossia::vec2f m_value2;
 Impl(ossia::vec2f v): m_value2{v} { }
Impl& operator=(ossia::vec2f v) { m_value2 = v; return *this; }

ossia::vec3f m_value3;
 Impl(ossia::vec3f v): m_value3{v} { }
Impl& operator=(ossia::vec3f v) { m_value3 = v; return *this; }

ossia::vec4f m_value4;
 Impl(ossia::vec4f v): m_value4{v} { }
Impl& operator=(ossia::vec4f v) { m_value4 = v; return *this; }

ossia::impulse m_value5;
 Impl(const ossia::impulse& v): m_value5{v} { }
Impl(ossia::impulse&& v): m_value5{std::move(v)} { }
Impl& operator=(const ossia::impulse& v) { m_value5 = v; return *this; }
Impl& operator=(ossia::impulse&& v) { m_value5 = std::move(v); return *this; }

bool m_value6;
 Impl(bool v): m_value6{v} { }
Impl& operator=(bool v) { m_value6 = v; return *this; }

std::string m_value7;
Impl(const std::string& v): m_value7{v} { }
Impl(std::string&& v): m_value7{std::move(v)} { }
Impl& operator=(const std::string& v) { m_value7 = v; return *this; }
Impl& operator=(std::string&& v) { m_value7 = std::move(v); return *this; }

std::vector<ossia::value> m_value8;
Impl(const std::vector<ossia::value>& v): m_value8{v} { }
Impl(std::vector<ossia::value>&& v): m_value8{std::move(v)} { }
Impl& operator=(const std::vector<ossia::value>& v) { m_value8 = v; return *this; }
Impl& operator=(std::vector<ossia::value>&& v) { m_value8 = std::move(v); return *this; }

char m_value9;
 Impl(char v): m_value9{v} { }
Impl& operator=(char v) { m_value9 = v; return *this; }

ossia::Destination m_value10;
Impl(const ossia::Destination& v): m_value10{v} { }
Impl(ossia::Destination&& v): m_value10{std::move(v)} { }
Impl& operator=(const ossia::Destination& v) { m_value10 = v; return *this; }
Impl& operator=(ossia::Destination&& v) { m_value10 = std::move(v); return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Type4, Type5, Type6, Type7, Type8, Type9, Type10, Npos
};

void destruct_impl() { 
switch(m_type) { 
  case Type::Type7:
    m_impl.m_value7.~basic_string();
    break;
  case Type::Type8:
    m_impl.m_value8.~vector<ossia::value>();
    break;
  case Type::Type10:
    m_impl.m_value10.~Destination();
    break;
  default: break; 
 } 
 }
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
value_variant_type(): m_type{Npos} { }
~value_variant_type() { destruct_impl(); }
 value_variant_type(float v): m_impl{v}, m_type{Type0} { }
 value_variant_type(int v): m_impl{v}, m_type{Type1} { }
 value_variant_type(ossia::vec2f v): m_impl{v}, m_type{Type2} { }
 value_variant_type(ossia::vec3f v): m_impl{v}, m_type{Type3} { }
 value_variant_type(ossia::vec4f v): m_impl{v}, m_type{Type4} { }
 value_variant_type(const ossia::impulse& v): m_impl{v}, m_type{Type5} { }
value_variant_type(ossia::impulse&& v): m_impl{v}, m_type{Type5} { }
 value_variant_type(bool v): m_impl{v}, m_type{Type6} { }
value_variant_type(const std::string& v): m_impl{v}, m_type{Type7} { }
value_variant_type(std::string&& v): m_impl{v}, m_type{Type7} { }
value_variant_type(const std::vector<ossia::value>& v): m_impl{v}, m_type{Type8} { }
value_variant_type(std::vector<ossia::value>&& v): m_impl{v}, m_type{Type8} { }
 value_variant_type(char v): m_impl{v}, m_type{Type9} { }
value_variant_type(const ossia::Destination& v): m_impl{v}, m_type{Type10} { }
value_variant_type(ossia::Destination&& v): m_impl{v}, m_type{Type10} { }
value_variant_type(const value_variant_type& other):
 m_type{other.m_type} { 
  switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0 = other.m_impl.m_value0;
    break;
  case Type::Type1:
    m_impl.m_value1 = other.m_impl.m_value1;
    break;
  case Type::Type2:
    m_impl.m_value2 = other.m_impl.m_value2;
    break;
  case Type::Type3:
    m_impl.m_value3 = other.m_impl.m_value3;
    break;
  case Type::Type4:
    m_impl.m_value4 = other.m_impl.m_value4;
    break;
  case Type::Type5:
    m_impl.m_value5 = other.m_impl.m_value5;
    break;
  case Type::Type6:
    m_impl.m_value6 = other.m_impl.m_value6;
    break;
  case Type::Type7:
    m_impl.m_value7 = other.m_impl.m_value7;
    break;
  case Type::Type8:
    m_impl.m_value8 = other.m_impl.m_value8;
    break;
  case Type::Type9:
    m_impl.m_value9 = other.m_impl.m_value9;
    break;
  case Type::Type10:
    m_impl.m_value10 = other.m_impl.m_value10;
    break;
    default: break;
  }
}
value_variant_type(value_variant_type&& other):
m_type{other.m_type} { 
  switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0 = std::move(other.m_impl.m_value0);
    break;
  case Type::Type1:
    m_impl.m_value1 = std::move(other.m_impl.m_value1);
    break;
  case Type::Type2:
    m_impl.m_value2 = std::move(other.m_impl.m_value2);
    break;
  case Type::Type3:
    m_impl.m_value3 = std::move(other.m_impl.m_value3);
    break;
  case Type::Type4:
    m_impl.m_value4 = std::move(other.m_impl.m_value4);
    break;
  case Type::Type5:
    m_impl.m_value5 = std::move(other.m_impl.m_value5);
    break;
  case Type::Type6:
    m_impl.m_value6 = std::move(other.m_impl.m_value6);
    break;
  case Type::Type7:
    m_impl.m_value7 = std::move(other.m_impl.m_value7);
    break;
  case Type::Type8:
    m_impl.m_value8 = std::move(other.m_impl.m_value8);
    break;
  case Type::Type9:
    m_impl.m_value9 = std::move(other.m_impl.m_value9);
    break;
  case Type::Type10:
    m_impl.m_value10 = std::move(other.m_impl.m_value10);
    break;
    default: break;
  }
}
value_variant_type& operator=(const value_variant_type& other){ 
  m_type = other.m_type;
  switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0 = other.m_impl.m_value0;
    break;
  case Type::Type1:
    m_impl.m_value1 = other.m_impl.m_value1;
    break;
  case Type::Type2:
    m_impl.m_value2 = other.m_impl.m_value2;
    break;
  case Type::Type3:
    m_impl.m_value3 = other.m_impl.m_value3;
    break;
  case Type::Type4:
    m_impl.m_value4 = other.m_impl.m_value4;
    break;
  case Type::Type5:
    m_impl.m_value5 = other.m_impl.m_value5;
    break;
  case Type::Type6:
    m_impl.m_value6 = other.m_impl.m_value6;
    break;
  case Type::Type7:
    m_impl.m_value7 = other.m_impl.m_value7;
    break;
  case Type::Type8:
    m_impl.m_value8 = other.m_impl.m_value8;
    break;
  case Type::Type9:
    m_impl.m_value9 = other.m_impl.m_value9;
    break;
  case Type::Type10:
    m_impl.m_value10 = other.m_impl.m_value10;
    break;
    default: break;
  }
  return *this;
}
value_variant_type& operator=(value_variant_type&& other)
{ 
  m_type = other.m_type;
  switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0 = std::move(other.m_impl.m_value0);
    break;
  case Type::Type1:
    m_impl.m_value1 = std::move(other.m_impl.m_value1);
    break;
  case Type::Type2:
    m_impl.m_value2 = std::move(other.m_impl.m_value2);
    break;
  case Type::Type3:
    m_impl.m_value3 = std::move(other.m_impl.m_value3);
    break;
  case Type::Type4:
    m_impl.m_value4 = std::move(other.m_impl.m_value4);
    break;
  case Type::Type5:
    m_impl.m_value5 = std::move(other.m_impl.m_value5);
    break;
  case Type::Type6:
    m_impl.m_value6 = std::move(other.m_impl.m_value6);
    break;
  case Type::Type7:
    m_impl.m_value7 = std::move(other.m_impl.m_value7);
    break;
  case Type::Type8:
    m_impl.m_value8 = std::move(other.m_impl.m_value8);
    break;
  case Type::Type9:
    m_impl.m_value9 = std::move(other.m_impl.m_value9);
    break;
  case Type::Type10:
    m_impl.m_value10 = std::move(other.m_impl.m_value10);
    break;
    default: break;
  }
  return *this;
}
};
template<> const float* value_variant_type::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> const int* value_variant_type::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> const ossia::vec2f* value_variant_type::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> const ossia::vec3f* value_variant_type::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> const ossia::vec4f* value_variant_type::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> const ossia::impulse* value_variant_type::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> const bool* value_variant_type::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> const std::string* value_variant_type::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> const std::vector<ossia::value>* value_variant_type::target() const { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> const char* value_variant_type::target() const { 
  if(m_type == Type9) 
    return &m_impl.m_value9 ;
  return nullptr; 
}
template<> const ossia::Destination* value_variant_type::target() const { 
  if(m_type == Type10) 
    return &m_impl.m_value10 ;
  return nullptr; 
}
template<> float* value_variant_type::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> int* value_variant_type::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> ossia::vec2f* value_variant_type::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> ossia::vec3f* value_variant_type::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> ossia::vec4f* value_variant_type::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> ossia::impulse* value_variant_type::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> bool* value_variant_type::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> std::string* value_variant_type::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> std::vector<ossia::value>* value_variant_type::target() { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> char* value_variant_type::target() { 
  if(m_type == Type9) 
    return &m_impl.m_value9 ;
  return nullptr; 
}
template<> ossia::Destination* value_variant_type::target() { 
  if(m_type == Type10) 
    return &m_impl.m_value10 ;
  return nullptr; 
}
template<> const float& value_variant_type::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> const int& value_variant_type::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> const ossia::vec2f& value_variant_type::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> const ossia::vec3f& value_variant_type::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> const ossia::vec4f& value_variant_type::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> const ossia::impulse& value_variant_type::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> const bool& value_variant_type::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> const std::string& value_variant_type::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> const std::vector<ossia::value>& value_variant_type::get() const { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> const char& value_variant_type::get() const { 
  if(m_type == Type9) 
    return m_impl.m_value9 ;
  throw; 
}
template<> const ossia::Destination& value_variant_type::get() const { 
  if(m_type == Type10) 
    return m_impl.m_value10 ;
  throw; 
}
template<> float& value_variant_type::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> int& value_variant_type::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> ossia::vec2f& value_variant_type::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> ossia::vec3f& value_variant_type::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> ossia::vec4f& value_variant_type::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> ossia::impulse& value_variant_type::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> bool& value_variant_type::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> std::string& value_variant_type::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> std::vector<ossia::value>& value_variant_type::get() { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> char& value_variant_type::get() { 
  if(m_type == Type9) 
    return m_impl.m_value9 ;
  throw; 
}
template<> ossia::Destination& value_variant_type::get() { 
  if(m_type == Type10) 
    return m_impl.m_value10 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const value_variant_type& var) {
  switch (var.m_type) { 
  case value_variant_type::Type::Type0:
    return functor(var.m_impl.m_value0);
  case value_variant_type::Type::Type1:
    return functor(var.m_impl.m_value1);
  case value_variant_type::Type::Type2:
    return functor(var.m_impl.m_value2);
  case value_variant_type::Type::Type3:
    return functor(var.m_impl.m_value3);
  case value_variant_type::Type::Type4:
    return functor(var.m_impl.m_value4);
  case value_variant_type::Type::Type5:
    return functor(var.m_impl.m_value5);
  case value_variant_type::Type::Type6:
    return functor(var.m_impl.m_value6);
  case value_variant_type::Type::Type7:
    return functor(var.m_impl.m_value7);
  case value_variant_type::Type::Type8:
    return functor(var.m_impl.m_value8);
  case value_variant_type::Type::Type9:
    return functor(var.m_impl.m_value9);
  case value_variant_type::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  value_variant_type& var) {
  switch (var.m_type) { 
  case value_variant_type::Type::Type0:
    return functor(var.m_impl.m_value0);
  case value_variant_type::Type::Type1:
    return functor(var.m_impl.m_value1);
  case value_variant_type::Type::Type2:
    return functor(var.m_impl.m_value2);
  case value_variant_type::Type::Type3:
    return functor(var.m_impl.m_value3);
  case value_variant_type::Type::Type4:
    return functor(var.m_impl.m_value4);
  case value_variant_type::Type::Type5:
    return functor(var.m_impl.m_value5);
  case value_variant_type::Type::Type6:
    return functor(var.m_impl.m_value6);
  case value_variant_type::Type::Type7:
    return functor(var.m_impl.m_value7);
  case value_variant_type::Type::Type8:
    return functor(var.m_impl.m_value8);
  case value_variant_type::Type::Type9:
    return functor(var.m_impl.m_value9);
  case value_variant_type::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  value_variant_type&& var) {
  switch (var.m_type) { 
  case value_variant_type::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case value_variant_type::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case value_variant_type::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case value_variant_type::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case value_variant_type::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case value_variant_type::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case value_variant_type::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case value_variant_type::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case value_variant_type::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  case value_variant_type::Type::Type9:
    return functor(std::move(var.m_impl.m_value9));
  case value_variant_type::Type::Type10:
    return functor(std::move(var.m_impl.m_value10));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const value_variant_type& var) {
  switch (var.m_type) { 
  case value_variant_type::Type::Type0:
    return functor(var.m_impl.m_value0);
  case value_variant_type::Type::Type1:
    return functor(var.m_impl.m_value1);
  case value_variant_type::Type::Type2:
    return functor(var.m_impl.m_value2);
  case value_variant_type::Type::Type3:
    return functor(var.m_impl.m_value3);
  case value_variant_type::Type::Type4:
    return functor(var.m_impl.m_value4);
  case value_variant_type::Type::Type5:
    return functor(var.m_impl.m_value5);
  case value_variant_type::Type::Type6:
    return functor(var.m_impl.m_value6);
  case value_variant_type::Type::Type7:
    return functor(var.m_impl.m_value7);
  case value_variant_type::Type::Type8:
    return functor(var.m_impl.m_value8);
  case value_variant_type::Type::Type9:
    return functor(var.m_impl.m_value9);
  case value_variant_type::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  value_variant_type& var) {
  switch (var.m_type) { 
  case value_variant_type::Type::Type0:
    return functor(var.m_impl.m_value0);
  case value_variant_type::Type::Type1:
    return functor(var.m_impl.m_value1);
  case value_variant_type::Type::Type2:
    return functor(var.m_impl.m_value2);
  case value_variant_type::Type::Type3:
    return functor(var.m_impl.m_value3);
  case value_variant_type::Type::Type4:
    return functor(var.m_impl.m_value4);
  case value_variant_type::Type::Type5:
    return functor(var.m_impl.m_value5);
  case value_variant_type::Type::Type6:
    return functor(var.m_impl.m_value6);
  case value_variant_type::Type::Type7:
    return functor(var.m_impl.m_value7);
  case value_variant_type::Type::Type8:
    return functor(var.m_impl.m_value8);
  case value_variant_type::Type::Type9:
    return functor(var.m_impl.m_value9);
  case value_variant_type::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  value_variant_type&& var) {
  switch (var.m_type) { 
  case value_variant_type::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case value_variant_type::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case value_variant_type::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case value_variant_type::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case value_variant_type::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case value_variant_type::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case value_variant_type::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case value_variant_type::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case value_variant_type::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  case value_variant_type::Type::Type9:
    return functor(std::move(var.m_impl.m_value9));
  case value_variant_type::Type::Type10:
    return functor(std::move(var.m_impl.m_value10));
  default: return functor();
  }
}
template<typename Functor>
auto apply(Functor&& functor, const value_variant_type& arg0, const value_variant_type& arg1)
{
switch(arg0.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
template<typename Functor>
auto apply(Functor&& functor, value_variant_type&& arg0, value_variant_type&& arg1)
{
switch(arg0.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value0));
}
case value_variant_type::Type::Type1:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value1));
}
case value_variant_type::Type::Type2:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value2));
}
case value_variant_type::Type::Type3:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value3));
}
case value_variant_type::Type::Type4:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value4));
}
case value_variant_type::Type::Type5:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value5));
}
case value_variant_type::Type::Type6:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value6));
}
case value_variant_type::Type::Type7:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value7));
}
case value_variant_type::Type::Type8:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value8));
}
case value_variant_type::Type::Type9:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value9));
}
case value_variant_type::Type::Type10:
{
return functor(std::move(arg0.m_impl.m_value10), std::move(arg1.m_impl.m_value10));
}
default: throw; 
}
}
default: throw; 
}
}
template<typename Functor>
auto apply(Functor&& functor, value_variant_type& arg0, value_variant_type& arg1)
{
switch(arg0.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
template<typename Functor>
auto apply(Functor&& functor, const value_variant_type& arg0, const value_variant_type& arg1, const value_variant_type& arg2)
{
switch(arg0.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg1.m_type) {
case value_variant_type::Type::Type0:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value0, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type1:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value1, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type2:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value2, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type3:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value3, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type4:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value4, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type5:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value5, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type6:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value6, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type7:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value7, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type8:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value8, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type9:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value9, arg2.m_impl.m_value10);
}
default: throw; 
}
}
case value_variant_type::Type::Type10:
{
switch(arg2.m_type) {
case value_variant_type::Type::Type0:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value0);
}
case value_variant_type::Type::Type1:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value1);
}
case value_variant_type::Type::Type2:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value2);
}
case value_variant_type::Type::Type3:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value3);
}
case value_variant_type::Type::Type4:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value4);
}
case value_variant_type::Type::Type5:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value5);
}
case value_variant_type::Type::Type6:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value6);
}
case value_variant_type::Type::Type7:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value7);
}
case value_variant_type::Type::Type8:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value8);
}
case value_variant_type::Type::Type9:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value9);
}
case value_variant_type::Type::Type10:
{
return functor(arg0.m_impl.m_value10, arg1.m_impl.m_value10, arg2.m_impl.m_value10);
}
default: throw; 
}
}
default: throw; 
}
}
default: throw; 
}
}
