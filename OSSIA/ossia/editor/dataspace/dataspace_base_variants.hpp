struct angle_u
{
public:
struct dummy_t {};
union Impl { 
ossia::degree_u m_value0;
 Impl(ossia::degree_u v): m_value0{v} { }
Impl& operator=(ossia::degree_u v) { m_value0 = v; return *this; }

ossia::radian_u m_value1;
 Impl(ossia::radian_u v): m_value1{v} { }
Impl& operator=(ossia::radian_u v) { m_value1 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
angle_u(): m_type{Npos} { }
~angle_u() { destruct_impl(); }
 angle_u(ossia::degree_u v): m_impl{v}, m_type{Type0} { }
 angle_u(ossia::radian_u v): m_impl{v}, m_type{Type1} { }
angle_u(const angle_u& other):
 m_type{other.m_type} { 
  switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0 = other.m_impl.m_value0;
    break;
  case Type::Type1:
    m_impl.m_value1 = other.m_impl.m_value1;
    break;
    default: break;
  }
}
angle_u(angle_u&& other):
m_type{other.m_type} { 
  switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0 = std::move(other.m_impl.m_value0);
    break;
  case Type::Type1:
    m_impl.m_value1 = std::move(other.m_impl.m_value1);
    break;
    default: break;
  }
}
angle_u& operator=(const angle_u& other){ 
  m_type = other.m_type;
  switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0 = other.m_impl.m_value0;
    break;
  case Type::Type1:
    m_impl.m_value1 = other.m_impl.m_value1;
    break;
    default: break;
  }
  return *this;
}
angle_u& operator=(angle_u&& other)
{ 
  m_type = other.m_type;
  switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0 = std::move(other.m_impl.m_value0);
    break;
  case Type::Type1:
    m_impl.m_value1 = std::move(other.m_impl.m_value1);
    break;
    default: break;
  }
  return *this;
}
};
template<> inline const ossia::degree_u* angle_u::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::radian_u* angle_u::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::degree_u* angle_u::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::radian_u* angle_u::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::degree_u& angle_u::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::radian_u& angle_u::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::degree_u& angle_u::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::radian_u& angle_u::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const angle_u& var) {
  switch (var.m_type) { 
  case angle_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case angle_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  angle_u& var) {
  switch (var.m_type) { 
  case angle_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case angle_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  angle_u&& var) {
  switch (var.m_type) { 
  case angle_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case angle_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const angle_u& var) {
  switch (var.m_type) { 
  case angle_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case angle_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  angle_u& var) {
  switch (var.m_type) { 
  case angle_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case angle_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  angle_u&& var) {
  switch (var.m_type) { 
  case angle_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case angle_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  default: return functor();
  }
}
struct color_u
{
public:
struct dummy_t {};
union Impl { 
ossia::argb_u m_value0;
 Impl(ossia::argb_u v): m_value0{v} { }
Impl& operator=(ossia::argb_u v) { m_value0 = v; return *this; }

ossia::rgba_u m_value1;
 Impl(ossia::rgba_u v): m_value1{v} { }
Impl& operator=(ossia::rgba_u v) { m_value1 = v; return *this; }

ossia::rgb_u m_value2;
 Impl(ossia::rgb_u v): m_value2{v} { }
Impl& operator=(ossia::rgb_u v) { m_value2 = v; return *this; }

ossia::bgr_u m_value3;
 Impl(ossia::bgr_u v): m_value3{v} { }
Impl& operator=(ossia::bgr_u v) { m_value3 = v; return *this; }

ossia::argb8_u m_value4;
 Impl(ossia::argb8_u v): m_value4{v} { }
Impl& operator=(ossia::argb8_u v) { m_value4 = v; return *this; }

ossia::hsv_u m_value5;
 Impl(ossia::hsv_u v): m_value5{v} { }
Impl& operator=(ossia::hsv_u v) { m_value5 = v; return *this; }

ossia::cmy8_u m_value6;
 Impl(ossia::cmy8_u v): m_value6{v} { }
Impl& operator=(ossia::cmy8_u v) { m_value6 = v; return *this; }

ossia::xyz_u m_value7;
 Impl(ossia::xyz_u v): m_value7{v} { }
Impl& operator=(ossia::xyz_u v) { m_value7 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Type4, Type5, Type6, Type7, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
color_u(): m_type{Npos} { }
~color_u() { destruct_impl(); }
 color_u(ossia::argb_u v): m_impl{v}, m_type{Type0} { }
 color_u(ossia::rgba_u v): m_impl{v}, m_type{Type1} { }
 color_u(ossia::rgb_u v): m_impl{v}, m_type{Type2} { }
 color_u(ossia::bgr_u v): m_impl{v}, m_type{Type3} { }
 color_u(ossia::argb8_u v): m_impl{v}, m_type{Type4} { }
 color_u(ossia::hsv_u v): m_impl{v}, m_type{Type5} { }
 color_u(ossia::cmy8_u v): m_impl{v}, m_type{Type6} { }
 color_u(ossia::xyz_u v): m_impl{v}, m_type{Type7} { }
color_u(const color_u& other):
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
    default: break;
  }
}
color_u(color_u&& other):
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
    default: break;
  }
}
color_u& operator=(const color_u& other){ 
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
    default: break;
  }
  return *this;
}
color_u& operator=(color_u&& other)
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
    default: break;
  }
  return *this;
}
};
template<> inline const ossia::argb_u* color_u::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::rgba_u* color_u::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::rgb_u* color_u::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::bgr_u* color_u::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::argb8_u* color_u::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::hsv_u* color_u::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::cmy8_u* color_u::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline const ossia::xyz_u* color_u::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline ossia::argb_u* color_u::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::rgba_u* color_u::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::rgb_u* color_u::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::bgr_u* color_u::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::argb8_u* color_u::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::hsv_u* color_u::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::cmy8_u* color_u::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline ossia::xyz_u* color_u::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline const ossia::argb_u& color_u::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::rgba_u& color_u::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::rgb_u& color_u::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::bgr_u& color_u::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::argb8_u& color_u::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::hsv_u& color_u::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline const ossia::cmy8_u& color_u::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline const ossia::xyz_u& color_u::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline ossia::argb_u& color_u::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::rgba_u& color_u::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::rgb_u& color_u::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::bgr_u& color_u::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::argb8_u& color_u::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::hsv_u& color_u::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::cmy8_u& color_u::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline ossia::xyz_u& color_u::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const color_u& var) {
  switch (var.m_type) { 
  case color_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case color_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case color_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case color_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case color_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case color_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case color_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case color_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  color_u& var) {
  switch (var.m_type) { 
  case color_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case color_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case color_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case color_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case color_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case color_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case color_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case color_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  color_u&& var) {
  switch (var.m_type) { 
  case color_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case color_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case color_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case color_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case color_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case color_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case color_u::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case color_u::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const color_u& var) {
  switch (var.m_type) { 
  case color_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case color_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case color_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case color_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case color_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case color_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case color_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case color_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  color_u& var) {
  switch (var.m_type) { 
  case color_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case color_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case color_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case color_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case color_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case color_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case color_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case color_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  color_u&& var) {
  switch (var.m_type) { 
  case color_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case color_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case color_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case color_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case color_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case color_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case color_u::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case color_u::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  default: return functor();
  }
}
struct distance_u
{
public:
struct dummy_t {};
union Impl { 
ossia::meter_u m_value0;
 Impl(ossia::meter_u v): m_value0{v} { }
Impl& operator=(ossia::meter_u v) { m_value0 = v; return *this; }

ossia::kilometer_u m_value1;
 Impl(ossia::kilometer_u v): m_value1{v} { }
Impl& operator=(ossia::kilometer_u v) { m_value1 = v; return *this; }

ossia::decimeter_u m_value2;
 Impl(ossia::decimeter_u v): m_value2{v} { }
Impl& operator=(ossia::decimeter_u v) { m_value2 = v; return *this; }

ossia::centimeter_u m_value3;
 Impl(ossia::centimeter_u v): m_value3{v} { }
Impl& operator=(ossia::centimeter_u v) { m_value3 = v; return *this; }

ossia::millimeter_u m_value4;
 Impl(ossia::millimeter_u v): m_value4{v} { }
Impl& operator=(ossia::millimeter_u v) { m_value4 = v; return *this; }

ossia::micrometer_u m_value5;
 Impl(ossia::micrometer_u v): m_value5{v} { }
Impl& operator=(ossia::micrometer_u v) { m_value5 = v; return *this; }

ossia::nanometer_u m_value6;
 Impl(ossia::nanometer_u v): m_value6{v} { }
Impl& operator=(ossia::nanometer_u v) { m_value6 = v; return *this; }

ossia::picometer_u m_value7;
 Impl(ossia::picometer_u v): m_value7{v} { }
Impl& operator=(ossia::picometer_u v) { m_value7 = v; return *this; }

ossia::inch_u m_value8;
 Impl(ossia::inch_u v): m_value8{v} { }
Impl& operator=(ossia::inch_u v) { m_value8 = v; return *this; }

ossia::foot_u m_value9;
 Impl(ossia::foot_u v): m_value9{v} { }
Impl& operator=(ossia::foot_u v) { m_value9 = v; return *this; }

ossia::mile_u m_value10;
 Impl(ossia::mile_u v): m_value10{v} { }
Impl& operator=(ossia::mile_u v) { m_value10 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Type4, Type5, Type6, Type7, Type8, Type9, Type10, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
distance_u(): m_type{Npos} { }
~distance_u() { destruct_impl(); }
 distance_u(ossia::meter_u v): m_impl{v}, m_type{Type0} { }
 distance_u(ossia::kilometer_u v): m_impl{v}, m_type{Type1} { }
 distance_u(ossia::decimeter_u v): m_impl{v}, m_type{Type2} { }
 distance_u(ossia::centimeter_u v): m_impl{v}, m_type{Type3} { }
 distance_u(ossia::millimeter_u v): m_impl{v}, m_type{Type4} { }
 distance_u(ossia::micrometer_u v): m_impl{v}, m_type{Type5} { }
 distance_u(ossia::nanometer_u v): m_impl{v}, m_type{Type6} { }
 distance_u(ossia::picometer_u v): m_impl{v}, m_type{Type7} { }
 distance_u(ossia::inch_u v): m_impl{v}, m_type{Type8} { }
 distance_u(ossia::foot_u v): m_impl{v}, m_type{Type9} { }
 distance_u(ossia::mile_u v): m_impl{v}, m_type{Type10} { }
distance_u(const distance_u& other):
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
distance_u(distance_u&& other):
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
distance_u& operator=(const distance_u& other){ 
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
distance_u& operator=(distance_u&& other)
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
template<> inline const ossia::meter_u* distance_u::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::kilometer_u* distance_u::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::decimeter_u* distance_u::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::centimeter_u* distance_u::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::millimeter_u* distance_u::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::micrometer_u* distance_u::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::nanometer_u* distance_u::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline const ossia::picometer_u* distance_u::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline const ossia::inch_u* distance_u::target() const { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline const ossia::foot_u* distance_u::target() const { 
  if(m_type == Type9) 
    return &m_impl.m_value9 ;
  return nullptr; 
}
template<> inline const ossia::mile_u* distance_u::target() const { 
  if(m_type == Type10) 
    return &m_impl.m_value10 ;
  return nullptr; 
}
template<> inline ossia::meter_u* distance_u::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::kilometer_u* distance_u::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::decimeter_u* distance_u::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::centimeter_u* distance_u::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::millimeter_u* distance_u::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::micrometer_u* distance_u::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::nanometer_u* distance_u::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline ossia::picometer_u* distance_u::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline ossia::inch_u* distance_u::target() { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline ossia::foot_u* distance_u::target() { 
  if(m_type == Type9) 
    return &m_impl.m_value9 ;
  return nullptr; 
}
template<> inline ossia::mile_u* distance_u::target() { 
  if(m_type == Type10) 
    return &m_impl.m_value10 ;
  return nullptr; 
}
template<> inline const ossia::meter_u& distance_u::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::kilometer_u& distance_u::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::decimeter_u& distance_u::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::centimeter_u& distance_u::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::millimeter_u& distance_u::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::micrometer_u& distance_u::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline const ossia::nanometer_u& distance_u::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline const ossia::picometer_u& distance_u::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline const ossia::inch_u& distance_u::get() const { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> inline const ossia::foot_u& distance_u::get() const { 
  if(m_type == Type9) 
    return m_impl.m_value9 ;
  throw; 
}
template<> inline const ossia::mile_u& distance_u::get() const { 
  if(m_type == Type10) 
    return m_impl.m_value10 ;
  throw; 
}
template<> inline ossia::meter_u& distance_u::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::kilometer_u& distance_u::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::decimeter_u& distance_u::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::centimeter_u& distance_u::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::millimeter_u& distance_u::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::micrometer_u& distance_u::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::nanometer_u& distance_u::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline ossia::picometer_u& distance_u::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline ossia::inch_u& distance_u::get() { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> inline ossia::foot_u& distance_u::get() { 
  if(m_type == Type9) 
    return m_impl.m_value9 ;
  throw; 
}
template<> inline ossia::mile_u& distance_u::get() { 
  if(m_type == Type10) 
    return m_impl.m_value10 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const distance_u& var) {
  switch (var.m_type) { 
  case distance_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case distance_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case distance_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case distance_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case distance_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case distance_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case distance_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case distance_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  case distance_u::Type::Type8:
    return functor(var.m_impl.m_value8);
  case distance_u::Type::Type9:
    return functor(var.m_impl.m_value9);
  case distance_u::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  distance_u& var) {
  switch (var.m_type) { 
  case distance_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case distance_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case distance_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case distance_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case distance_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case distance_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case distance_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case distance_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  case distance_u::Type::Type8:
    return functor(var.m_impl.m_value8);
  case distance_u::Type::Type9:
    return functor(var.m_impl.m_value9);
  case distance_u::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  distance_u&& var) {
  switch (var.m_type) { 
  case distance_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case distance_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case distance_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case distance_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case distance_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case distance_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case distance_u::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case distance_u::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case distance_u::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  case distance_u::Type::Type9:
    return functor(std::move(var.m_impl.m_value9));
  case distance_u::Type::Type10:
    return functor(std::move(var.m_impl.m_value10));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const distance_u& var) {
  switch (var.m_type) { 
  case distance_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case distance_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case distance_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case distance_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case distance_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case distance_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case distance_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case distance_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  case distance_u::Type::Type8:
    return functor(var.m_impl.m_value8);
  case distance_u::Type::Type9:
    return functor(var.m_impl.m_value9);
  case distance_u::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  distance_u& var) {
  switch (var.m_type) { 
  case distance_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case distance_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case distance_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case distance_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case distance_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case distance_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case distance_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case distance_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  case distance_u::Type::Type8:
    return functor(var.m_impl.m_value8);
  case distance_u::Type::Type9:
    return functor(var.m_impl.m_value9);
  case distance_u::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  distance_u&& var) {
  switch (var.m_type) { 
  case distance_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case distance_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case distance_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case distance_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case distance_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case distance_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case distance_u::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case distance_u::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case distance_u::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  case distance_u::Type::Type9:
    return functor(std::move(var.m_impl.m_value9));
  case distance_u::Type::Type10:
    return functor(std::move(var.m_impl.m_value10));
  default: return functor();
  }
}
struct gain_u
{
public:
struct dummy_t {};
union Impl { 
ossia::linear_u m_value0;
 Impl(ossia::linear_u v): m_value0{v} { }
Impl& operator=(ossia::linear_u v) { m_value0 = v; return *this; }

ossia::midigain_u m_value1;
 Impl(ossia::midigain_u v): m_value1{v} { }
Impl& operator=(ossia::midigain_u v) { m_value1 = v; return *this; }

ossia::decibel_u m_value2;
 Impl(ossia::decibel_u v): m_value2{v} { }
Impl& operator=(ossia::decibel_u v) { m_value2 = v; return *this; }

ossia::decibel_raw_u m_value3;
 Impl(ossia::decibel_raw_u v): m_value3{v} { }
Impl& operator=(ossia::decibel_raw_u v) { m_value3 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
gain_u(): m_type{Npos} { }
~gain_u() { destruct_impl(); }
 gain_u(ossia::linear_u v): m_impl{v}, m_type{Type0} { }
 gain_u(ossia::midigain_u v): m_impl{v}, m_type{Type1} { }
 gain_u(ossia::decibel_u v): m_impl{v}, m_type{Type2} { }
 gain_u(ossia::decibel_raw_u v): m_impl{v}, m_type{Type3} { }
gain_u(const gain_u& other):
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
    default: break;
  }
}
gain_u(gain_u&& other):
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
    default: break;
  }
}
gain_u& operator=(const gain_u& other){ 
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
    default: break;
  }
  return *this;
}
gain_u& operator=(gain_u&& other)
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
    default: break;
  }
  return *this;
}
};
template<> inline const ossia::linear_u* gain_u::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::midigain_u* gain_u::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::decibel_u* gain_u::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::decibel_raw_u* gain_u::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::linear_u* gain_u::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::midigain_u* gain_u::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::decibel_u* gain_u::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::decibel_raw_u* gain_u::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::linear_u& gain_u::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::midigain_u& gain_u::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::decibel_u& gain_u::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::decibel_raw_u& gain_u::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::linear_u& gain_u::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::midigain_u& gain_u::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::decibel_u& gain_u::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::decibel_raw_u& gain_u::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const gain_u& var) {
  switch (var.m_type) { 
  case gain_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case gain_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case gain_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case gain_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  gain_u& var) {
  switch (var.m_type) { 
  case gain_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case gain_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case gain_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case gain_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  gain_u&& var) {
  switch (var.m_type) { 
  case gain_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case gain_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case gain_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case gain_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const gain_u& var) {
  switch (var.m_type) { 
  case gain_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case gain_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case gain_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case gain_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  gain_u& var) {
  switch (var.m_type) { 
  case gain_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case gain_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case gain_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case gain_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  gain_u&& var) {
  switch (var.m_type) { 
  case gain_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case gain_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case gain_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case gain_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  default: return functor();
  }
}
struct orientation_u
{
public:
struct dummy_t {};
union Impl { 
ossia::quaternion_u m_value0;
 Impl(ossia::quaternion_u v): m_value0{v} { }
Impl& operator=(ossia::quaternion_u v) { m_value0 = v; return *this; }

ossia::euler_u m_value1;
 Impl(ossia::euler_u v): m_value1{v} { }
Impl& operator=(ossia::euler_u v) { m_value1 = v; return *this; }

ossia::axis_u m_value2;
 Impl(ossia::axis_u v): m_value2{v} { }
Impl& operator=(ossia::axis_u v) { m_value2 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
orientation_u(): m_type{Npos} { }
~orientation_u() { destruct_impl(); }
 orientation_u(ossia::quaternion_u v): m_impl{v}, m_type{Type0} { }
 orientation_u(ossia::euler_u v): m_impl{v}, m_type{Type1} { }
 orientation_u(ossia::axis_u v): m_impl{v}, m_type{Type2} { }
orientation_u(const orientation_u& other):
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
    default: break;
  }
}
orientation_u(orientation_u&& other):
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
    default: break;
  }
}
orientation_u& operator=(const orientation_u& other){ 
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
    default: break;
  }
  return *this;
}
orientation_u& operator=(orientation_u&& other)
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
    default: break;
  }
  return *this;
}
};
template<> inline const ossia::quaternion_u* orientation_u::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::euler_u* orientation_u::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::axis_u* orientation_u::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::quaternion_u* orientation_u::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::euler_u* orientation_u::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::axis_u* orientation_u::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::quaternion_u& orientation_u::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::euler_u& orientation_u::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::axis_u& orientation_u::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::quaternion_u& orientation_u::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::euler_u& orientation_u::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::axis_u& orientation_u::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const orientation_u& var) {
  switch (var.m_type) { 
  case orientation_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case orientation_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case orientation_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  orientation_u& var) {
  switch (var.m_type) { 
  case orientation_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case orientation_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case orientation_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  orientation_u&& var) {
  switch (var.m_type) { 
  case orientation_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case orientation_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case orientation_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const orientation_u& var) {
  switch (var.m_type) { 
  case orientation_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case orientation_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case orientation_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  orientation_u& var) {
  switch (var.m_type) { 
  case orientation_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case orientation_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case orientation_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  orientation_u&& var) {
  switch (var.m_type) { 
  case orientation_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case orientation_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case orientation_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  default: return functor();
  }
}
struct position_u
{
public:
struct dummy_t {};
union Impl { 
ossia::cartesian_3d_u m_value0;
 Impl(ossia::cartesian_3d_u v): m_value0{v} { }
Impl& operator=(ossia::cartesian_3d_u v) { m_value0 = v; return *this; }

ossia::cartesian_2d_u m_value1;
 Impl(ossia::cartesian_2d_u v): m_value1{v} { }
Impl& operator=(ossia::cartesian_2d_u v) { m_value1 = v; return *this; }

ossia::spherical_u m_value2;
 Impl(ossia::spherical_u v): m_value2{v} { }
Impl& operator=(ossia::spherical_u v) { m_value2 = v; return *this; }

ossia::polar_u m_value3;
 Impl(ossia::polar_u v): m_value3{v} { }
Impl& operator=(ossia::polar_u v) { m_value3 = v; return *this; }

ossia::opengl_u m_value4;
 Impl(ossia::opengl_u v): m_value4{v} { }
Impl& operator=(ossia::opengl_u v) { m_value4 = v; return *this; }

ossia::cylindrical_u m_value5;
 Impl(ossia::cylindrical_u v): m_value5{v} { }
Impl& operator=(ossia::cylindrical_u v) { m_value5 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Type4, Type5, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
position_u(): m_type{Npos} { }
~position_u() { destruct_impl(); }
 position_u(ossia::cartesian_3d_u v): m_impl{v}, m_type{Type0} { }
 position_u(ossia::cartesian_2d_u v): m_impl{v}, m_type{Type1} { }
 position_u(ossia::spherical_u v): m_impl{v}, m_type{Type2} { }
 position_u(ossia::polar_u v): m_impl{v}, m_type{Type3} { }
 position_u(ossia::opengl_u v): m_impl{v}, m_type{Type4} { }
 position_u(ossia::cylindrical_u v): m_impl{v}, m_type{Type5} { }
position_u(const position_u& other):
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
    default: break;
  }
}
position_u(position_u&& other):
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
    default: break;
  }
}
position_u& operator=(const position_u& other){ 
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
    default: break;
  }
  return *this;
}
position_u& operator=(position_u&& other)
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
    default: break;
  }
  return *this;
}
};
template<> inline const ossia::cartesian_3d_u* position_u::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::cartesian_2d_u* position_u::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::spherical_u* position_u::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::polar_u* position_u::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::opengl_u* position_u::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::cylindrical_u* position_u::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::cartesian_3d_u* position_u::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::cartesian_2d_u* position_u::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::spherical_u* position_u::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::polar_u* position_u::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::opengl_u* position_u::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::cylindrical_u* position_u::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::cartesian_3d_u& position_u::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::cartesian_2d_u& position_u::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::spherical_u& position_u::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::polar_u& position_u::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::opengl_u& position_u::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::cylindrical_u& position_u::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::cartesian_3d_u& position_u::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::cartesian_2d_u& position_u::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::spherical_u& position_u::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::polar_u& position_u::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::opengl_u& position_u::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::cylindrical_u& position_u::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const position_u& var) {
  switch (var.m_type) { 
  case position_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case position_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case position_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case position_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case position_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case position_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  position_u& var) {
  switch (var.m_type) { 
  case position_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case position_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case position_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case position_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case position_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case position_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  position_u&& var) {
  switch (var.m_type) { 
  case position_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case position_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case position_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case position_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case position_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case position_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const position_u& var) {
  switch (var.m_type) { 
  case position_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case position_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case position_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case position_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case position_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case position_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  position_u& var) {
  switch (var.m_type) { 
  case position_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case position_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case position_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case position_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case position_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case position_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  position_u&& var) {
  switch (var.m_type) { 
  case position_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case position_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case position_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case position_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case position_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case position_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  default: return functor();
  }
}
struct speed_u
{
public:
struct dummy_t {};
union Impl { 
ossia::meter_per_second_u m_value0;
 Impl(ossia::meter_per_second_u v): m_value0{v} { }
Impl& operator=(ossia::meter_per_second_u v) { m_value0 = v; return *this; }

ossia::miles_per_hour_u m_value1;
 Impl(ossia::miles_per_hour_u v): m_value1{v} { }
Impl& operator=(ossia::miles_per_hour_u v) { m_value1 = v; return *this; }

ossia::kilometer_per_hour_u m_value2;
 Impl(ossia::kilometer_per_hour_u v): m_value2{v} { }
Impl& operator=(ossia::kilometer_per_hour_u v) { m_value2 = v; return *this; }

ossia::knot_u m_value3;
 Impl(ossia::knot_u v): m_value3{v} { }
Impl& operator=(ossia::knot_u v) { m_value3 = v; return *this; }

ossia::foot_per_second_u m_value4;
 Impl(ossia::foot_per_second_u v): m_value4{v} { }
Impl& operator=(ossia::foot_per_second_u v) { m_value4 = v; return *this; }

ossia::foot_per_hour_u m_value5;
 Impl(ossia::foot_per_hour_u v): m_value5{v} { }
Impl& operator=(ossia::foot_per_hour_u v) { m_value5 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Type4, Type5, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
speed_u(): m_type{Npos} { }
~speed_u() { destruct_impl(); }
 speed_u(ossia::meter_per_second_u v): m_impl{v}, m_type{Type0} { }
 speed_u(ossia::miles_per_hour_u v): m_impl{v}, m_type{Type1} { }
 speed_u(ossia::kilometer_per_hour_u v): m_impl{v}, m_type{Type2} { }
 speed_u(ossia::knot_u v): m_impl{v}, m_type{Type3} { }
 speed_u(ossia::foot_per_second_u v): m_impl{v}, m_type{Type4} { }
 speed_u(ossia::foot_per_hour_u v): m_impl{v}, m_type{Type5} { }
speed_u(const speed_u& other):
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
    default: break;
  }
}
speed_u(speed_u&& other):
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
    default: break;
  }
}
speed_u& operator=(const speed_u& other){ 
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
    default: break;
  }
  return *this;
}
speed_u& operator=(speed_u&& other)
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
    default: break;
  }
  return *this;
}
};
template<> inline const ossia::meter_per_second_u* speed_u::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::miles_per_hour_u* speed_u::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::kilometer_per_hour_u* speed_u::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::knot_u* speed_u::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::foot_per_second_u* speed_u::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::foot_per_hour_u* speed_u::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::meter_per_second_u* speed_u::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::miles_per_hour_u* speed_u::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::kilometer_per_hour_u* speed_u::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::knot_u* speed_u::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::foot_per_second_u* speed_u::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::foot_per_hour_u* speed_u::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::meter_per_second_u& speed_u::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::miles_per_hour_u& speed_u::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::kilometer_per_hour_u& speed_u::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::knot_u& speed_u::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::foot_per_second_u& speed_u::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::foot_per_hour_u& speed_u::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::meter_per_second_u& speed_u::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::miles_per_hour_u& speed_u::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::kilometer_per_hour_u& speed_u::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::knot_u& speed_u::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::foot_per_second_u& speed_u::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::foot_per_hour_u& speed_u::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const speed_u& var) {
  switch (var.m_type) { 
  case speed_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case speed_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case speed_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case speed_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case speed_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case speed_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  speed_u& var) {
  switch (var.m_type) { 
  case speed_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case speed_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case speed_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case speed_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case speed_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case speed_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  speed_u&& var) {
  switch (var.m_type) { 
  case speed_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case speed_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case speed_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case speed_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case speed_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case speed_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const speed_u& var) {
  switch (var.m_type) { 
  case speed_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case speed_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case speed_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case speed_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case speed_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case speed_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  speed_u& var) {
  switch (var.m_type) { 
  case speed_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case speed_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case speed_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case speed_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case speed_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case speed_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  speed_u&& var) {
  switch (var.m_type) { 
  case speed_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case speed_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case speed_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case speed_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case speed_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case speed_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  default: return functor();
  }
}
struct time_u
{
public:
struct dummy_t {};
union Impl { 
ossia::second_u m_value0;
 Impl(ossia::second_u v): m_value0{v} { }
Impl& operator=(ossia::second_u v) { m_value0 = v; return *this; }

ossia::bark_u m_value1;
 Impl(ossia::bark_u v): m_value1{v} { }
Impl& operator=(ossia::bark_u v) { m_value1 = v; return *this; }

ossia::bpm_u m_value2;
 Impl(ossia::bpm_u v): m_value2{v} { }
Impl& operator=(ossia::bpm_u v) { m_value2 = v; return *this; }

ossia::cent_u m_value3;
 Impl(ossia::cent_u v): m_value3{v} { }
Impl& operator=(ossia::cent_u v) { m_value3 = v; return *this; }

ossia::frequency_u m_value4;
 Impl(ossia::frequency_u v): m_value4{v} { }
Impl& operator=(ossia::frequency_u v) { m_value4 = v; return *this; }

ossia::mel_u m_value5;
 Impl(ossia::mel_u v): m_value5{v} { }
Impl& operator=(ossia::mel_u v) { m_value5 = v; return *this; }

ossia::midi_pitch_u m_value6;
 Impl(ossia::midi_pitch_u v): m_value6{v} { }
Impl& operator=(ossia::midi_pitch_u v) { m_value6 = v; return *this; }

ossia::millisecond_u m_value7;
 Impl(ossia::millisecond_u v): m_value7{v} { }
Impl& operator=(ossia::millisecond_u v) { m_value7 = v; return *this; }

ossia::playback_speed_u m_value8;
 Impl(ossia::playback_speed_u v): m_value8{v} { }
Impl& operator=(ossia::playback_speed_u v) { m_value8 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Type4, Type5, Type6, Type7, Type8, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
time_u(): m_type{Npos} { }
~time_u() { destruct_impl(); }
 time_u(ossia::second_u v): m_impl{v}, m_type{Type0} { }
 time_u(ossia::bark_u v): m_impl{v}, m_type{Type1} { }
 time_u(ossia::bpm_u v): m_impl{v}, m_type{Type2} { }
 time_u(ossia::cent_u v): m_impl{v}, m_type{Type3} { }
 time_u(ossia::frequency_u v): m_impl{v}, m_type{Type4} { }
 time_u(ossia::mel_u v): m_impl{v}, m_type{Type5} { }
 time_u(ossia::midi_pitch_u v): m_impl{v}, m_type{Type6} { }
 time_u(ossia::millisecond_u v): m_impl{v}, m_type{Type7} { }
 time_u(ossia::playback_speed_u v): m_impl{v}, m_type{Type8} { }
time_u(const time_u& other):
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
    default: break;
  }
}
time_u(time_u&& other):
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
    default: break;
  }
}
time_u& operator=(const time_u& other){ 
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
    default: break;
  }
  return *this;
}
time_u& operator=(time_u&& other)
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
    default: break;
  }
  return *this;
}
};
template<> inline const ossia::second_u* time_u::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::bark_u* time_u::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::bpm_u* time_u::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::cent_u* time_u::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::frequency_u* time_u::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::mel_u* time_u::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::midi_pitch_u* time_u::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline const ossia::millisecond_u* time_u::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline const ossia::playback_speed_u* time_u::target() const { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline ossia::second_u* time_u::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::bark_u* time_u::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::bpm_u* time_u::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::cent_u* time_u::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::frequency_u* time_u::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::mel_u* time_u::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::midi_pitch_u* time_u::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline ossia::millisecond_u* time_u::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline ossia::playback_speed_u* time_u::target() { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline const ossia::second_u& time_u::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::bark_u& time_u::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::bpm_u& time_u::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::cent_u& time_u::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::frequency_u& time_u::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::mel_u& time_u::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline const ossia::midi_pitch_u& time_u::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline const ossia::millisecond_u& time_u::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline const ossia::playback_speed_u& time_u::get() const { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> inline ossia::second_u& time_u::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::bark_u& time_u::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::bpm_u& time_u::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::cent_u& time_u::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::frequency_u& time_u::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::mel_u& time_u::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::midi_pitch_u& time_u::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline ossia::millisecond_u& time_u::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline ossia::playback_speed_u& time_u::get() { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const time_u& var) {
  switch (var.m_type) { 
  case time_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case time_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case time_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case time_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case time_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case time_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case time_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case time_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  case time_u::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  time_u& var) {
  switch (var.m_type) { 
  case time_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case time_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case time_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case time_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case time_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case time_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case time_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case time_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  case time_u::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  time_u&& var) {
  switch (var.m_type) { 
  case time_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case time_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case time_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case time_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case time_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case time_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case time_u::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case time_u::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case time_u::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const time_u& var) {
  switch (var.m_type) { 
  case time_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case time_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case time_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case time_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case time_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case time_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case time_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case time_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  case time_u::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  time_u& var) {
  switch (var.m_type) { 
  case time_u::Type::Type0:
    return functor(var.m_impl.m_value0);
  case time_u::Type::Type1:
    return functor(var.m_impl.m_value1);
  case time_u::Type::Type2:
    return functor(var.m_impl.m_value2);
  case time_u::Type::Type3:
    return functor(var.m_impl.m_value3);
  case time_u::Type::Type4:
    return functor(var.m_impl.m_value4);
  case time_u::Type::Type5:
    return functor(var.m_impl.m_value5);
  case time_u::Type::Type6:
    return functor(var.m_impl.m_value6);
  case time_u::Type::Type7:
    return functor(var.m_impl.m_value7);
  case time_u::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  time_u&& var) {
  switch (var.m_type) { 
  case time_u::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case time_u::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case time_u::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case time_u::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case time_u::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case time_u::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case time_u::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case time_u::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case time_u::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  default: return functor();
  }
}
struct unit_variant
{
public:
struct dummy_t {};
union Impl { 
ossia::distance_u m_value0;
 Impl(ossia::distance_u v): m_value0{v} { }
Impl& operator=(ossia::distance_u v) { m_value0 = v; return *this; }

ossia::position_u m_value1;
 Impl(ossia::position_u v): m_value1{v} { }
Impl& operator=(ossia::position_u v) { m_value1 = v; return *this; }

ossia::speed_u m_value2;
 Impl(ossia::speed_u v): m_value2{v} { }
Impl& operator=(ossia::speed_u v) { m_value2 = v; return *this; }

ossia::orientation_u m_value3;
 Impl(ossia::orientation_u v): m_value3{v} { }
Impl& operator=(ossia::orientation_u v) { m_value3 = v; return *this; }

ossia::angle_u m_value4;
 Impl(ossia::angle_u v): m_value4{v} { }
Impl& operator=(ossia::angle_u v) { m_value4 = v; return *this; }

ossia::color_u m_value5;
 Impl(ossia::color_u v): m_value5{v} { }
Impl& operator=(ossia::color_u v) { m_value5 = v; return *this; }

ossia::gain_u m_value6;
 Impl(ossia::gain_u v): m_value6{v} { }
Impl& operator=(ossia::gain_u v) { m_value6 = v; return *this; }

ossia::time_u m_value7;
 Impl(ossia::time_u v): m_value7{v} { }
Impl& operator=(ossia::time_u v) { m_value7 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Type4, Type5, Type6, Type7, Npos
};

void destruct_impl() { 
switch(m_type) { 
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
unit_variant(): m_type{Npos} { }
~unit_variant() { destruct_impl(); }
 unit_variant(ossia::distance_u v): m_impl{v}, m_type{Type0} { }
 unit_variant(ossia::position_u v): m_impl{v}, m_type{Type1} { }
 unit_variant(ossia::speed_u v): m_impl{v}, m_type{Type2} { }
 unit_variant(ossia::orientation_u v): m_impl{v}, m_type{Type3} { }
 unit_variant(ossia::angle_u v): m_impl{v}, m_type{Type4} { }
 unit_variant(ossia::color_u v): m_impl{v}, m_type{Type5} { }
 unit_variant(ossia::gain_u v): m_impl{v}, m_type{Type6} { }
 unit_variant(ossia::time_u v): m_impl{v}, m_type{Type7} { }
unit_variant(const unit_variant& other):
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
    default: break;
  }
}
unit_variant(unit_variant&& other):
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
    default: break;
  }
}
unit_variant& operator=(const unit_variant& other){ 
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
    default: break;
  }
  return *this;
}
unit_variant& operator=(unit_variant&& other)
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
    default: break;
  }
  return *this;
}
};
template<> inline const ossia::distance_u* unit_variant::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::position_u* unit_variant::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::speed_u* unit_variant::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::orientation_u* unit_variant::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::angle_u* unit_variant::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::color_u* unit_variant::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::gain_u* unit_variant::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline const ossia::time_u* unit_variant::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline ossia::distance_u* unit_variant::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::position_u* unit_variant::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::speed_u* unit_variant::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::orientation_u* unit_variant::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::angle_u* unit_variant::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::color_u* unit_variant::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::gain_u* unit_variant::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline ossia::time_u* unit_variant::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline const ossia::distance_u& unit_variant::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::position_u& unit_variant::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::speed_u& unit_variant::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::orientation_u& unit_variant::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::angle_u& unit_variant::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::color_u& unit_variant::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline const ossia::gain_u& unit_variant::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline const ossia::time_u& unit_variant::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline ossia::distance_u& unit_variant::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::position_u& unit_variant::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::speed_u& unit_variant::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::orientation_u& unit_variant::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::angle_u& unit_variant::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::color_u& unit_variant::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::gain_u& unit_variant::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline ossia::time_u& unit_variant::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const unit_variant& var) {
  switch (var.m_type) { 
  case unit_variant::Type::Type0:
    return functor(var.m_impl.m_value0);
  case unit_variant::Type::Type1:
    return functor(var.m_impl.m_value1);
  case unit_variant::Type::Type2:
    return functor(var.m_impl.m_value2);
  case unit_variant::Type::Type3:
    return functor(var.m_impl.m_value3);
  case unit_variant::Type::Type4:
    return functor(var.m_impl.m_value4);
  case unit_variant::Type::Type5:
    return functor(var.m_impl.m_value5);
  case unit_variant::Type::Type6:
    return functor(var.m_impl.m_value6);
  case unit_variant::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  unit_variant& var) {
  switch (var.m_type) { 
  case unit_variant::Type::Type0:
    return functor(var.m_impl.m_value0);
  case unit_variant::Type::Type1:
    return functor(var.m_impl.m_value1);
  case unit_variant::Type::Type2:
    return functor(var.m_impl.m_value2);
  case unit_variant::Type::Type3:
    return functor(var.m_impl.m_value3);
  case unit_variant::Type::Type4:
    return functor(var.m_impl.m_value4);
  case unit_variant::Type::Type5:
    return functor(var.m_impl.m_value5);
  case unit_variant::Type::Type6:
    return functor(var.m_impl.m_value6);
  case unit_variant::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  unit_variant&& var) {
  switch (var.m_type) { 
  case unit_variant::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case unit_variant::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case unit_variant::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case unit_variant::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case unit_variant::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case unit_variant::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case unit_variant::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case unit_variant::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const unit_variant& var) {
  switch (var.m_type) { 
  case unit_variant::Type::Type0:
    return functor(var.m_impl.m_value0);
  case unit_variant::Type::Type1:
    return functor(var.m_impl.m_value1);
  case unit_variant::Type::Type2:
    return functor(var.m_impl.m_value2);
  case unit_variant::Type::Type3:
    return functor(var.m_impl.m_value3);
  case unit_variant::Type::Type4:
    return functor(var.m_impl.m_value4);
  case unit_variant::Type::Type5:
    return functor(var.m_impl.m_value5);
  case unit_variant::Type::Type6:
    return functor(var.m_impl.m_value6);
  case unit_variant::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  unit_variant& var) {
  switch (var.m_type) { 
  case unit_variant::Type::Type0:
    return functor(var.m_impl.m_value0);
  case unit_variant::Type::Type1:
    return functor(var.m_impl.m_value1);
  case unit_variant::Type::Type2:
    return functor(var.m_impl.m_value2);
  case unit_variant::Type::Type3:
    return functor(var.m_impl.m_value3);
  case unit_variant::Type::Type4:
    return functor(var.m_impl.m_value4);
  case unit_variant::Type::Type5:
    return functor(var.m_impl.m_value5);
  case unit_variant::Type::Type6:
    return functor(var.m_impl.m_value6);
  case unit_variant::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  unit_variant&& var) {
  switch (var.m_type) { 
  case unit_variant::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case unit_variant::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case unit_variant::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case unit_variant::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case unit_variant::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case unit_variant::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case unit_variant::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case unit_variant::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  default: return functor();
  }
}
