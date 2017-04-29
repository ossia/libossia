struct angle
{
public:
struct dummy_t {};
union Impl { 
ossia::degree m_value0;
 Impl(ossia::degree v): m_value0{v} { }
Impl& operator=(ossia::degree v) { m_value0 = v; return *this; }

ossia::radian m_value1;
 Impl(ossia::radian v): m_value1{v} { }
Impl& operator=(ossia::radian v) { m_value1 = v; return *this; }

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
angle(): m_type{Npos} { }
~angle() { destruct_impl(); }
 angle(ossia::degree v): m_impl{v}, m_type{Type0} { }
 angle(ossia::radian v): m_impl{v}, m_type{Type1} { }
angle(const angle& other):
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
angle(angle&& other):
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
angle& operator=(const angle& other){ 
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
angle& operator=(angle&& other)
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
template<> inline const ossia::degree* angle::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::radian* angle::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::degree* angle::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::radian* angle::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::degree& angle::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::radian& angle::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::degree& angle::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::radian& angle::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const angle& var) {
  switch (var.m_type) { 
  case angle::Type::Type0:
    return functor(var.m_impl.m_value0);
  case angle::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  angle& var) {
  switch (var.m_type) { 
  case angle::Type::Type0:
    return functor(var.m_impl.m_value0);
  case angle::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  angle&& var) {
  switch (var.m_type) { 
  case angle::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case angle::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const angle& var) {
  switch (var.m_type) { 
  case angle::Type::Type0:
    return functor(var.m_impl.m_value0);
  case angle::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  angle& var) {
  switch (var.m_type) { 
  case angle::Type::Type0:
    return functor(var.m_impl.m_value0);
  case angle::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  angle&& var) {
  switch (var.m_type) { 
  case angle::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case angle::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  default: return functor();
  }
}
inline bool operator==(const angle& lhs, const angle& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case angle::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const angle& lhs, const angle& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case angle::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    default: return false;
  }
  return true; 
}
inline bool operator==(const angle& lhs, const ossia::degree& rhs){ 
  return (lhs.m_type == angle::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::degree& lhs, const angle& rhs){ 
  return (rhs.m_type == angle::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const angle& lhs, const ossia::degree& rhs){ 
  return (lhs.m_type != angle::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::degree& lhs, const angle& rhs){ 
  return (rhs.m_type != angle::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const angle& lhs, const ossia::radian& rhs){ 
  return (lhs.m_type == angle::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::radian& lhs, const angle& rhs){ 
  return (rhs.m_type == angle::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const angle& lhs, const ossia::radian& rhs){ 
  return (lhs.m_type != angle::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::radian& lhs, const angle& rhs){ 
  return (rhs.m_type != angle::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
struct color
{
public:
struct dummy_t {};
union Impl { 
ossia::argb m_value0;
 Impl(ossia::argb v): m_value0{v} { }
Impl& operator=(ossia::argb v) { m_value0 = v; return *this; }

ossia::rgba m_value1;
 Impl(ossia::rgba v): m_value1{v} { }
Impl& operator=(ossia::rgba v) { m_value1 = v; return *this; }

ossia::rgb m_value2;
 Impl(ossia::rgb v): m_value2{v} { }
Impl& operator=(ossia::rgb v) { m_value2 = v; return *this; }

ossia::bgr m_value3;
 Impl(ossia::bgr v): m_value3{v} { }
Impl& operator=(ossia::bgr v) { m_value3 = v; return *this; }

ossia::argb8 m_value4;
 Impl(ossia::argb8 v): m_value4{v} { }
Impl& operator=(ossia::argb8 v) { m_value4 = v; return *this; }

ossia::hsv m_value5;
 Impl(ossia::hsv v): m_value5{v} { }
Impl& operator=(ossia::hsv v) { m_value5 = v; return *this; }

ossia::cmy8 m_value6;
 Impl(ossia::cmy8 v): m_value6{v} { }
Impl& operator=(ossia::cmy8 v) { m_value6 = v; return *this; }

ossia::xyz m_value7;
 Impl(ossia::xyz v): m_value7{v} { }
Impl& operator=(ossia::xyz v) { m_value7 = v; return *this; }

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
color(): m_type{Npos} { }
~color() { destruct_impl(); }
 color(ossia::argb v): m_impl{v}, m_type{Type0} { }
 color(ossia::rgba v): m_impl{v}, m_type{Type1} { }
 color(ossia::rgb v): m_impl{v}, m_type{Type2} { }
 color(ossia::bgr v): m_impl{v}, m_type{Type3} { }
 color(ossia::argb8 v): m_impl{v}, m_type{Type4} { }
 color(ossia::hsv v): m_impl{v}, m_type{Type5} { }
 color(ossia::cmy8 v): m_impl{v}, m_type{Type6} { }
 color(ossia::xyz v): m_impl{v}, m_type{Type7} { }
color(const color& other):
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
color(color&& other):
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
color& operator=(const color& other){ 
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
color& operator=(color&& other)
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
template<> inline const ossia::argb* color::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::rgba* color::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::rgb* color::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::bgr* color::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::argb8* color::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::hsv* color::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::cmy8* color::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline const ossia::xyz* color::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline ossia::argb* color::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::rgba* color::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::rgb* color::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::bgr* color::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::argb8* color::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::hsv* color::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::cmy8* color::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline ossia::xyz* color::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline const ossia::argb& color::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::rgba& color::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::rgb& color::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::bgr& color::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::argb8& color::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::hsv& color::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline const ossia::cmy8& color::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline const ossia::xyz& color::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline ossia::argb& color::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::rgba& color::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::rgb& color::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::bgr& color::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::argb8& color::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::hsv& color::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::cmy8& color::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline ossia::xyz& color::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const color& var) {
  switch (var.m_type) { 
  case color::Type::Type0:
    return functor(var.m_impl.m_value0);
  case color::Type::Type1:
    return functor(var.m_impl.m_value1);
  case color::Type::Type2:
    return functor(var.m_impl.m_value2);
  case color::Type::Type3:
    return functor(var.m_impl.m_value3);
  case color::Type::Type4:
    return functor(var.m_impl.m_value4);
  case color::Type::Type5:
    return functor(var.m_impl.m_value5);
  case color::Type::Type6:
    return functor(var.m_impl.m_value6);
  case color::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  color& var) {
  switch (var.m_type) { 
  case color::Type::Type0:
    return functor(var.m_impl.m_value0);
  case color::Type::Type1:
    return functor(var.m_impl.m_value1);
  case color::Type::Type2:
    return functor(var.m_impl.m_value2);
  case color::Type::Type3:
    return functor(var.m_impl.m_value3);
  case color::Type::Type4:
    return functor(var.m_impl.m_value4);
  case color::Type::Type5:
    return functor(var.m_impl.m_value5);
  case color::Type::Type6:
    return functor(var.m_impl.m_value6);
  case color::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  color&& var) {
  switch (var.m_type) { 
  case color::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case color::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case color::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case color::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case color::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case color::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case color::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case color::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const color& var) {
  switch (var.m_type) { 
  case color::Type::Type0:
    return functor(var.m_impl.m_value0);
  case color::Type::Type1:
    return functor(var.m_impl.m_value1);
  case color::Type::Type2:
    return functor(var.m_impl.m_value2);
  case color::Type::Type3:
    return functor(var.m_impl.m_value3);
  case color::Type::Type4:
    return functor(var.m_impl.m_value4);
  case color::Type::Type5:
    return functor(var.m_impl.m_value5);
  case color::Type::Type6:
    return functor(var.m_impl.m_value6);
  case color::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  color& var) {
  switch (var.m_type) { 
  case color::Type::Type0:
    return functor(var.m_impl.m_value0);
  case color::Type::Type1:
    return functor(var.m_impl.m_value1);
  case color::Type::Type2:
    return functor(var.m_impl.m_value2);
  case color::Type::Type3:
    return functor(var.m_impl.m_value3);
  case color::Type::Type4:
    return functor(var.m_impl.m_value4);
  case color::Type::Type5:
    return functor(var.m_impl.m_value5);
  case color::Type::Type6:
    return functor(var.m_impl.m_value6);
  case color::Type::Type7:
    return functor(var.m_impl.m_value7);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  color&& var) {
  switch (var.m_type) { 
  case color::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case color::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case color::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case color::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case color::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case color::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case color::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case color::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  default: return functor();
  }
}
inline bool operator==(const color& lhs, const color& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case color::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
    case color::Type::Type2:
      return lhs.m_impl.m_value2==rhs.m_impl.m_value2;
    case color::Type::Type4:
      return lhs.m_impl.m_value4==rhs.m_impl.m_value4;
    case color::Type::Type6:
      return lhs.m_impl.m_value6==rhs.m_impl.m_value6;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const color& lhs, const color& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case color::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    case color::Type::Type2:
      return lhs.m_impl.m_value2!=rhs.m_impl.m_value2;
    case color::Type::Type4:
      return lhs.m_impl.m_value4!=rhs.m_impl.m_value4;
    case color::Type::Type6:
      return lhs.m_impl.m_value6!=rhs.m_impl.m_value6;
    default: return false;
  }
  return true; 
}
inline bool operator==(const color& lhs, const ossia::argb& rhs){ 
  return (lhs.m_type == color::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::argb& lhs, const color& rhs){ 
  return (rhs.m_type == color::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const color& lhs, const ossia::argb& rhs){ 
  return (lhs.m_type != color::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::argb& lhs, const color& rhs){ 
  return (rhs.m_type != color::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const color& lhs, const ossia::rgba& rhs){ 
  return (lhs.m_type == color::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::rgba& lhs, const color& rhs){ 
  return (rhs.m_type == color::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const color& lhs, const ossia::rgba& rhs){ 
  return (lhs.m_type != color::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::rgba& lhs, const color& rhs){ 
  return (rhs.m_type != color::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
inline bool operator==(const color& lhs, const ossia::rgb& rhs){ 
  return (lhs.m_type == color::Type::Type2) && (lhs.m_impl.m_value2 == rhs); 
}
inline bool operator==(const ossia::rgb& lhs, const color& rhs){ 
  return (rhs.m_type == color::Type::Type2) && (rhs.m_impl.m_value2 == lhs); 
}
inline bool operator!=(const color& lhs, const ossia::rgb& rhs){ 
  return (lhs.m_type != color::Type::Type2) || (lhs.m_impl.m_value2 != rhs); 
}
inline bool operator!=(const ossia::rgb& lhs, const color& rhs){ 
  return (rhs.m_type != color::Type::Type2) || (rhs.m_impl.m_value2 != lhs); 
}
inline bool operator==(const color& lhs, const ossia::bgr& rhs){ 
  return (lhs.m_type == color::Type::Type3) && (lhs.m_impl.m_value3 == rhs); 
}
inline bool operator==(const ossia::bgr& lhs, const color& rhs){ 
  return (rhs.m_type == color::Type::Type3) && (rhs.m_impl.m_value3 == lhs); 
}
inline bool operator!=(const color& lhs, const ossia::bgr& rhs){ 
  return (lhs.m_type != color::Type::Type3) || (lhs.m_impl.m_value3 != rhs); 
}
inline bool operator!=(const ossia::bgr& lhs, const color& rhs){ 
  return (rhs.m_type != color::Type::Type3) || (rhs.m_impl.m_value3 != lhs); 
}
inline bool operator==(const color& lhs, const ossia::argb8& rhs){ 
  return (lhs.m_type == color::Type::Type4) && (lhs.m_impl.m_value4 == rhs); 
}
inline bool operator==(const ossia::argb8& lhs, const color& rhs){ 
  return (rhs.m_type == color::Type::Type4) && (rhs.m_impl.m_value4 == lhs); 
}
inline bool operator!=(const color& lhs, const ossia::argb8& rhs){ 
  return (lhs.m_type != color::Type::Type4) || (lhs.m_impl.m_value4 != rhs); 
}
inline bool operator!=(const ossia::argb8& lhs, const color& rhs){ 
  return (rhs.m_type != color::Type::Type4) || (rhs.m_impl.m_value4 != lhs); 
}
inline bool operator==(const color& lhs, const ossia::hsv& rhs){ 
  return (lhs.m_type == color::Type::Type5) && (lhs.m_impl.m_value5 == rhs); 
}
inline bool operator==(const ossia::hsv& lhs, const color& rhs){ 
  return (rhs.m_type == color::Type::Type5) && (rhs.m_impl.m_value5 == lhs); 
}
inline bool operator!=(const color& lhs, const ossia::hsv& rhs){ 
  return (lhs.m_type != color::Type::Type5) || (lhs.m_impl.m_value5 != rhs); 
}
inline bool operator!=(const ossia::hsv& lhs, const color& rhs){ 
  return (rhs.m_type != color::Type::Type5) || (rhs.m_impl.m_value5 != lhs); 
}
inline bool operator==(const color& lhs, const ossia::cmy8& rhs){ 
  return (lhs.m_type == color::Type::Type6) && (lhs.m_impl.m_value6 == rhs); 
}
inline bool operator==(const ossia::cmy8& lhs, const color& rhs){ 
  return (rhs.m_type == color::Type::Type6) && (rhs.m_impl.m_value6 == lhs); 
}
inline bool operator!=(const color& lhs, const ossia::cmy8& rhs){ 
  return (lhs.m_type != color::Type::Type6) || (lhs.m_impl.m_value6 != rhs); 
}
inline bool operator!=(const ossia::cmy8& lhs, const color& rhs){ 
  return (rhs.m_type != color::Type::Type6) || (rhs.m_impl.m_value6 != lhs); 
}
inline bool operator==(const color& lhs, const ossia::xyz& rhs){ 
  return (lhs.m_type == color::Type::Type7) && (lhs.m_impl.m_value7 == rhs); 
}
inline bool operator==(const ossia::xyz& lhs, const color& rhs){ 
  return (rhs.m_type == color::Type::Type7) && (rhs.m_impl.m_value7 == lhs); 
}
inline bool operator!=(const color& lhs, const ossia::xyz& rhs){ 
  return (lhs.m_type != color::Type::Type7) || (lhs.m_impl.m_value7 != rhs); 
}
inline bool operator!=(const ossia::xyz& lhs, const color& rhs){ 
  return (rhs.m_type != color::Type::Type7) || (rhs.m_impl.m_value7 != lhs); 
}
struct distance
{
public:
struct dummy_t {};
union Impl { 
ossia::meter m_value0;
 Impl(ossia::meter v): m_value0{v} { }
Impl& operator=(ossia::meter v) { m_value0 = v; return *this; }

ossia::kilometer m_value1;
 Impl(ossia::kilometer v): m_value1{v} { }
Impl& operator=(ossia::kilometer v) { m_value1 = v; return *this; }

ossia::decimeter m_value2;
 Impl(ossia::decimeter v): m_value2{v} { }
Impl& operator=(ossia::decimeter v) { m_value2 = v; return *this; }

ossia::centimeter m_value3;
 Impl(ossia::centimeter v): m_value3{v} { }
Impl& operator=(ossia::centimeter v) { m_value3 = v; return *this; }

ossia::millimeter m_value4;
 Impl(ossia::millimeter v): m_value4{v} { }
Impl& operator=(ossia::millimeter v) { m_value4 = v; return *this; }

ossia::micrometer m_value5;
 Impl(ossia::micrometer v): m_value5{v} { }
Impl& operator=(ossia::micrometer v) { m_value5 = v; return *this; }

ossia::nanometer m_value6;
 Impl(ossia::nanometer v): m_value6{v} { }
Impl& operator=(ossia::nanometer v) { m_value6 = v; return *this; }

ossia::picometer m_value7;
 Impl(ossia::picometer v): m_value7{v} { }
Impl& operator=(ossia::picometer v) { m_value7 = v; return *this; }

ossia::inch m_value8;
 Impl(ossia::inch v): m_value8{v} { }
Impl& operator=(ossia::inch v) { m_value8 = v; return *this; }

ossia::foot m_value9;
 Impl(ossia::foot v): m_value9{v} { }
Impl& operator=(ossia::foot v) { m_value9 = v; return *this; }

ossia::mile m_value10;
 Impl(ossia::mile v): m_value10{v} { }
Impl& operator=(ossia::mile v) { m_value10 = v; return *this; }

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
distance(): m_type{Npos} { }
~distance() { destruct_impl(); }
 distance(ossia::meter v): m_impl{v}, m_type{Type0} { }
 distance(ossia::kilometer v): m_impl{v}, m_type{Type1} { }
 distance(ossia::decimeter v): m_impl{v}, m_type{Type2} { }
 distance(ossia::centimeter v): m_impl{v}, m_type{Type3} { }
 distance(ossia::millimeter v): m_impl{v}, m_type{Type4} { }
 distance(ossia::micrometer v): m_impl{v}, m_type{Type5} { }
 distance(ossia::nanometer v): m_impl{v}, m_type{Type6} { }
 distance(ossia::picometer v): m_impl{v}, m_type{Type7} { }
 distance(ossia::inch v): m_impl{v}, m_type{Type8} { }
 distance(ossia::foot v): m_impl{v}, m_type{Type9} { }
 distance(ossia::mile v): m_impl{v}, m_type{Type10} { }
distance(const distance& other):
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
distance(distance&& other):
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
distance& operator=(const distance& other){ 
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
distance& operator=(distance&& other)
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
template<> inline const ossia::meter* distance::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::kilometer* distance::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::decimeter* distance::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::centimeter* distance::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::millimeter* distance::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::micrometer* distance::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::nanometer* distance::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline const ossia::picometer* distance::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline const ossia::inch* distance::target() const { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline const ossia::foot* distance::target() const { 
  if(m_type == Type9) 
    return &m_impl.m_value9 ;
  return nullptr; 
}
template<> inline const ossia::mile* distance::target() const { 
  if(m_type == Type10) 
    return &m_impl.m_value10 ;
  return nullptr; 
}
template<> inline ossia::meter* distance::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::kilometer* distance::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::decimeter* distance::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::centimeter* distance::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::millimeter* distance::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::micrometer* distance::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::nanometer* distance::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline ossia::picometer* distance::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline ossia::inch* distance::target() { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline ossia::foot* distance::target() { 
  if(m_type == Type9) 
    return &m_impl.m_value9 ;
  return nullptr; 
}
template<> inline ossia::mile* distance::target() { 
  if(m_type == Type10) 
    return &m_impl.m_value10 ;
  return nullptr; 
}
template<> inline const ossia::meter& distance::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::kilometer& distance::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::decimeter& distance::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::centimeter& distance::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::millimeter& distance::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::micrometer& distance::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline const ossia::nanometer& distance::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline const ossia::picometer& distance::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline const ossia::inch& distance::get() const { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> inline const ossia::foot& distance::get() const { 
  if(m_type == Type9) 
    return m_impl.m_value9 ;
  throw; 
}
template<> inline const ossia::mile& distance::get() const { 
  if(m_type == Type10) 
    return m_impl.m_value10 ;
  throw; 
}
template<> inline ossia::meter& distance::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::kilometer& distance::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::decimeter& distance::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::centimeter& distance::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::millimeter& distance::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::micrometer& distance::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::nanometer& distance::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline ossia::picometer& distance::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline ossia::inch& distance::get() { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> inline ossia::foot& distance::get() { 
  if(m_type == Type9) 
    return m_impl.m_value9 ;
  throw; 
}
template<> inline ossia::mile& distance::get() { 
  if(m_type == Type10) 
    return m_impl.m_value10 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const distance& var) {
  switch (var.m_type) { 
  case distance::Type::Type0:
    return functor(var.m_impl.m_value0);
  case distance::Type::Type1:
    return functor(var.m_impl.m_value1);
  case distance::Type::Type2:
    return functor(var.m_impl.m_value2);
  case distance::Type::Type3:
    return functor(var.m_impl.m_value3);
  case distance::Type::Type4:
    return functor(var.m_impl.m_value4);
  case distance::Type::Type5:
    return functor(var.m_impl.m_value5);
  case distance::Type::Type6:
    return functor(var.m_impl.m_value6);
  case distance::Type::Type7:
    return functor(var.m_impl.m_value7);
  case distance::Type::Type8:
    return functor(var.m_impl.m_value8);
  case distance::Type::Type9:
    return functor(var.m_impl.m_value9);
  case distance::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  distance& var) {
  switch (var.m_type) { 
  case distance::Type::Type0:
    return functor(var.m_impl.m_value0);
  case distance::Type::Type1:
    return functor(var.m_impl.m_value1);
  case distance::Type::Type2:
    return functor(var.m_impl.m_value2);
  case distance::Type::Type3:
    return functor(var.m_impl.m_value3);
  case distance::Type::Type4:
    return functor(var.m_impl.m_value4);
  case distance::Type::Type5:
    return functor(var.m_impl.m_value5);
  case distance::Type::Type6:
    return functor(var.m_impl.m_value6);
  case distance::Type::Type7:
    return functor(var.m_impl.m_value7);
  case distance::Type::Type8:
    return functor(var.m_impl.m_value8);
  case distance::Type::Type9:
    return functor(var.m_impl.m_value9);
  case distance::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  distance&& var) {
  switch (var.m_type) { 
  case distance::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case distance::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case distance::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case distance::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case distance::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case distance::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case distance::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case distance::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case distance::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  case distance::Type::Type9:
    return functor(std::move(var.m_impl.m_value9));
  case distance::Type::Type10:
    return functor(std::move(var.m_impl.m_value10));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const distance& var) {
  switch (var.m_type) { 
  case distance::Type::Type0:
    return functor(var.m_impl.m_value0);
  case distance::Type::Type1:
    return functor(var.m_impl.m_value1);
  case distance::Type::Type2:
    return functor(var.m_impl.m_value2);
  case distance::Type::Type3:
    return functor(var.m_impl.m_value3);
  case distance::Type::Type4:
    return functor(var.m_impl.m_value4);
  case distance::Type::Type5:
    return functor(var.m_impl.m_value5);
  case distance::Type::Type6:
    return functor(var.m_impl.m_value6);
  case distance::Type::Type7:
    return functor(var.m_impl.m_value7);
  case distance::Type::Type8:
    return functor(var.m_impl.m_value8);
  case distance::Type::Type9:
    return functor(var.m_impl.m_value9);
  case distance::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  distance& var) {
  switch (var.m_type) { 
  case distance::Type::Type0:
    return functor(var.m_impl.m_value0);
  case distance::Type::Type1:
    return functor(var.m_impl.m_value1);
  case distance::Type::Type2:
    return functor(var.m_impl.m_value2);
  case distance::Type::Type3:
    return functor(var.m_impl.m_value3);
  case distance::Type::Type4:
    return functor(var.m_impl.m_value4);
  case distance::Type::Type5:
    return functor(var.m_impl.m_value5);
  case distance::Type::Type6:
    return functor(var.m_impl.m_value6);
  case distance::Type::Type7:
    return functor(var.m_impl.m_value7);
  case distance::Type::Type8:
    return functor(var.m_impl.m_value8);
  case distance::Type::Type9:
    return functor(var.m_impl.m_value9);
  case distance::Type::Type10:
    return functor(var.m_impl.m_value10);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  distance&& var) {
  switch (var.m_type) { 
  case distance::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case distance::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case distance::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case distance::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case distance::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case distance::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case distance::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case distance::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case distance::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  case distance::Type::Type9:
    return functor(std::move(var.m_impl.m_value9));
  case distance::Type::Type10:
    return functor(std::move(var.m_impl.m_value10));
  default: return functor();
  }
}
inline bool operator==(const distance& lhs, const distance& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case distance::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
    case distance::Type::Type2:
      return lhs.m_impl.m_value2==rhs.m_impl.m_value2;
    case distance::Type::Type4:
      return lhs.m_impl.m_value4==rhs.m_impl.m_value4;
    case distance::Type::Type6:
      return lhs.m_impl.m_value6==rhs.m_impl.m_value6;
    case distance::Type::Type8:
      return lhs.m_impl.m_value8==rhs.m_impl.m_value8;
    case distance::Type::Type10:
      return lhs.m_impl.m_value10==rhs.m_impl.m_value10;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const distance& lhs, const distance& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case distance::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    case distance::Type::Type2:
      return lhs.m_impl.m_value2!=rhs.m_impl.m_value2;
    case distance::Type::Type4:
      return lhs.m_impl.m_value4!=rhs.m_impl.m_value4;
    case distance::Type::Type6:
      return lhs.m_impl.m_value6!=rhs.m_impl.m_value6;
    case distance::Type::Type8:
      return lhs.m_impl.m_value8!=rhs.m_impl.m_value8;
    case distance::Type::Type10:
      return lhs.m_impl.m_value10!=rhs.m_impl.m_value10;
    default: return false;
  }
  return true; 
}
inline bool operator==(const distance& lhs, const ossia::meter& rhs){ 
  return (lhs.m_type == distance::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::meter& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::meter& rhs){ 
  return (lhs.m_type != distance::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::meter& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::kilometer& rhs){ 
  return (lhs.m_type == distance::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::kilometer& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::kilometer& rhs){ 
  return (lhs.m_type != distance::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::kilometer& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::decimeter& rhs){ 
  return (lhs.m_type == distance::Type::Type2) && (lhs.m_impl.m_value2 == rhs); 
}
inline bool operator==(const ossia::decimeter& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type2) && (rhs.m_impl.m_value2 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::decimeter& rhs){ 
  return (lhs.m_type != distance::Type::Type2) || (lhs.m_impl.m_value2 != rhs); 
}
inline bool operator!=(const ossia::decimeter& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type2) || (rhs.m_impl.m_value2 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::centimeter& rhs){ 
  return (lhs.m_type == distance::Type::Type3) && (lhs.m_impl.m_value3 == rhs); 
}
inline bool operator==(const ossia::centimeter& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type3) && (rhs.m_impl.m_value3 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::centimeter& rhs){ 
  return (lhs.m_type != distance::Type::Type3) || (lhs.m_impl.m_value3 != rhs); 
}
inline bool operator!=(const ossia::centimeter& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type3) || (rhs.m_impl.m_value3 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::millimeter& rhs){ 
  return (lhs.m_type == distance::Type::Type4) && (lhs.m_impl.m_value4 == rhs); 
}
inline bool operator==(const ossia::millimeter& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type4) && (rhs.m_impl.m_value4 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::millimeter& rhs){ 
  return (lhs.m_type != distance::Type::Type4) || (lhs.m_impl.m_value4 != rhs); 
}
inline bool operator!=(const ossia::millimeter& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type4) || (rhs.m_impl.m_value4 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::micrometer& rhs){ 
  return (lhs.m_type == distance::Type::Type5) && (lhs.m_impl.m_value5 == rhs); 
}
inline bool operator==(const ossia::micrometer& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type5) && (rhs.m_impl.m_value5 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::micrometer& rhs){ 
  return (lhs.m_type != distance::Type::Type5) || (lhs.m_impl.m_value5 != rhs); 
}
inline bool operator!=(const ossia::micrometer& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type5) || (rhs.m_impl.m_value5 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::nanometer& rhs){ 
  return (lhs.m_type == distance::Type::Type6) && (lhs.m_impl.m_value6 == rhs); 
}
inline bool operator==(const ossia::nanometer& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type6) && (rhs.m_impl.m_value6 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::nanometer& rhs){ 
  return (lhs.m_type != distance::Type::Type6) || (lhs.m_impl.m_value6 != rhs); 
}
inline bool operator!=(const ossia::nanometer& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type6) || (rhs.m_impl.m_value6 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::picometer& rhs){ 
  return (lhs.m_type == distance::Type::Type7) && (lhs.m_impl.m_value7 == rhs); 
}
inline bool operator==(const ossia::picometer& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type7) && (rhs.m_impl.m_value7 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::picometer& rhs){ 
  return (lhs.m_type != distance::Type::Type7) || (lhs.m_impl.m_value7 != rhs); 
}
inline bool operator!=(const ossia::picometer& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type7) || (rhs.m_impl.m_value7 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::inch& rhs){ 
  return (lhs.m_type == distance::Type::Type8) && (lhs.m_impl.m_value8 == rhs); 
}
inline bool operator==(const ossia::inch& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type8) && (rhs.m_impl.m_value8 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::inch& rhs){ 
  return (lhs.m_type != distance::Type::Type8) || (lhs.m_impl.m_value8 != rhs); 
}
inline bool operator!=(const ossia::inch& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type8) || (rhs.m_impl.m_value8 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::foot& rhs){ 
  return (lhs.m_type == distance::Type::Type9) && (lhs.m_impl.m_value9 == rhs); 
}
inline bool operator==(const ossia::foot& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type9) && (rhs.m_impl.m_value9 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::foot& rhs){ 
  return (lhs.m_type != distance::Type::Type9) || (lhs.m_impl.m_value9 != rhs); 
}
inline bool operator!=(const ossia::foot& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type9) || (rhs.m_impl.m_value9 != lhs); 
}
inline bool operator==(const distance& lhs, const ossia::mile& rhs){ 
  return (lhs.m_type == distance::Type::Type10) && (lhs.m_impl.m_value10 == rhs); 
}
inline bool operator==(const ossia::mile& lhs, const distance& rhs){ 
  return (rhs.m_type == distance::Type::Type10) && (rhs.m_impl.m_value10 == lhs); 
}
inline bool operator!=(const distance& lhs, const ossia::mile& rhs){ 
  return (lhs.m_type != distance::Type::Type10) || (lhs.m_impl.m_value10 != rhs); 
}
inline bool operator!=(const ossia::mile& lhs, const distance& rhs){ 
  return (rhs.m_type != distance::Type::Type10) || (rhs.m_impl.m_value10 != lhs); 
}
struct gain
{
public:
struct dummy_t {};
union Impl { 
ossia::linear m_value0;
 Impl(ossia::linear v): m_value0{v} { }
Impl& operator=(ossia::linear v) { m_value0 = v; return *this; }

ossia::midigain m_value1;
 Impl(ossia::midigain v): m_value1{v} { }
Impl& operator=(ossia::midigain v) { m_value1 = v; return *this; }

ossia::decibel m_value2;
 Impl(ossia::decibel v): m_value2{v} { }
Impl& operator=(ossia::decibel v) { m_value2 = v; return *this; }

ossia::decibel_raw m_value3;
 Impl(ossia::decibel_raw v): m_value3{v} { }
Impl& operator=(ossia::decibel_raw v) { m_value3 = v; return *this; }

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
gain(): m_type{Npos} { }
~gain() { destruct_impl(); }
 gain(ossia::linear v): m_impl{v}, m_type{Type0} { }
 gain(ossia::midigain v): m_impl{v}, m_type{Type1} { }
 gain(ossia::decibel v): m_impl{v}, m_type{Type2} { }
 gain(ossia::decibel_raw v): m_impl{v}, m_type{Type3} { }
gain(const gain& other):
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
gain(gain&& other):
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
gain& operator=(const gain& other){ 
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
gain& operator=(gain&& other)
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
template<> inline const ossia::linear* gain::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::midigain* gain::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::decibel* gain::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::decibel_raw* gain::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::linear* gain::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::midigain* gain::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::decibel* gain::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::decibel_raw* gain::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::linear& gain::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::midigain& gain::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::decibel& gain::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::decibel_raw& gain::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::linear& gain::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::midigain& gain::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::decibel& gain::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::decibel_raw& gain::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const gain& var) {
  switch (var.m_type) { 
  case gain::Type::Type0:
    return functor(var.m_impl.m_value0);
  case gain::Type::Type1:
    return functor(var.m_impl.m_value1);
  case gain::Type::Type2:
    return functor(var.m_impl.m_value2);
  case gain::Type::Type3:
    return functor(var.m_impl.m_value3);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  gain& var) {
  switch (var.m_type) { 
  case gain::Type::Type0:
    return functor(var.m_impl.m_value0);
  case gain::Type::Type1:
    return functor(var.m_impl.m_value1);
  case gain::Type::Type2:
    return functor(var.m_impl.m_value2);
  case gain::Type::Type3:
    return functor(var.m_impl.m_value3);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  gain&& var) {
  switch (var.m_type) { 
  case gain::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case gain::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case gain::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case gain::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const gain& var) {
  switch (var.m_type) { 
  case gain::Type::Type0:
    return functor(var.m_impl.m_value0);
  case gain::Type::Type1:
    return functor(var.m_impl.m_value1);
  case gain::Type::Type2:
    return functor(var.m_impl.m_value2);
  case gain::Type::Type3:
    return functor(var.m_impl.m_value3);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  gain& var) {
  switch (var.m_type) { 
  case gain::Type::Type0:
    return functor(var.m_impl.m_value0);
  case gain::Type::Type1:
    return functor(var.m_impl.m_value1);
  case gain::Type::Type2:
    return functor(var.m_impl.m_value2);
  case gain::Type::Type3:
    return functor(var.m_impl.m_value3);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  gain&& var) {
  switch (var.m_type) { 
  case gain::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case gain::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case gain::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case gain::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  default: return functor();
  }
}
inline bool operator==(const gain& lhs, const gain& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case gain::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
    case gain::Type::Type2:
      return lhs.m_impl.m_value2==rhs.m_impl.m_value2;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const gain& lhs, const gain& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case gain::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    case gain::Type::Type2:
      return lhs.m_impl.m_value2!=rhs.m_impl.m_value2;
    default: return false;
  }
  return true; 
}
inline bool operator==(const gain& lhs, const ossia::linear& rhs){ 
  return (lhs.m_type == gain::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::linear& lhs, const gain& rhs){ 
  return (rhs.m_type == gain::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const gain& lhs, const ossia::linear& rhs){ 
  return (lhs.m_type != gain::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::linear& lhs, const gain& rhs){ 
  return (rhs.m_type != gain::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const gain& lhs, const ossia::midigain& rhs){ 
  return (lhs.m_type == gain::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::midigain& lhs, const gain& rhs){ 
  return (rhs.m_type == gain::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const gain& lhs, const ossia::midigain& rhs){ 
  return (lhs.m_type != gain::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::midigain& lhs, const gain& rhs){ 
  return (rhs.m_type != gain::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
inline bool operator==(const gain& lhs, const ossia::decibel& rhs){ 
  return (lhs.m_type == gain::Type::Type2) && (lhs.m_impl.m_value2 == rhs); 
}
inline bool operator==(const ossia::decibel& lhs, const gain& rhs){ 
  return (rhs.m_type == gain::Type::Type2) && (rhs.m_impl.m_value2 == lhs); 
}
inline bool operator!=(const gain& lhs, const ossia::decibel& rhs){ 
  return (lhs.m_type != gain::Type::Type2) || (lhs.m_impl.m_value2 != rhs); 
}
inline bool operator!=(const ossia::decibel& lhs, const gain& rhs){ 
  return (rhs.m_type != gain::Type::Type2) || (rhs.m_impl.m_value2 != lhs); 
}
inline bool operator==(const gain& lhs, const ossia::decibel_raw& rhs){ 
  return (lhs.m_type == gain::Type::Type3) && (lhs.m_impl.m_value3 == rhs); 
}
inline bool operator==(const ossia::decibel_raw& lhs, const gain& rhs){ 
  return (rhs.m_type == gain::Type::Type3) && (rhs.m_impl.m_value3 == lhs); 
}
inline bool operator!=(const gain& lhs, const ossia::decibel_raw& rhs){ 
  return (lhs.m_type != gain::Type::Type3) || (lhs.m_impl.m_value3 != rhs); 
}
inline bool operator!=(const ossia::decibel_raw& lhs, const gain& rhs){ 
  return (rhs.m_type != gain::Type::Type3) || (rhs.m_impl.m_value3 != lhs); 
}
struct orientation
{
public:
struct dummy_t {};
union Impl { 
ossia::quaternion m_value0;
 Impl(ossia::quaternion v): m_value0{v} { }
Impl& operator=(ossia::quaternion v) { m_value0 = v; return *this; }

ossia::euler m_value1;
 Impl(ossia::euler v): m_value1{v} { }
Impl& operator=(ossia::euler v) { m_value1 = v; return *this; }

ossia::axis m_value2;
 Impl(ossia::axis v): m_value2{v} { }
Impl& operator=(ossia::axis v) { m_value2 = v; return *this; }

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
orientation(): m_type{Npos} { }
~orientation() { destruct_impl(); }
 orientation(ossia::quaternion v): m_impl{v}, m_type{Type0} { }
 orientation(ossia::euler v): m_impl{v}, m_type{Type1} { }
 orientation(ossia::axis v): m_impl{v}, m_type{Type2} { }
orientation(const orientation& other):
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
orientation(orientation&& other):
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
orientation& operator=(const orientation& other){ 
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
orientation& operator=(orientation&& other)
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
template<> inline const ossia::quaternion* orientation::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::euler* orientation::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::axis* orientation::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::quaternion* orientation::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::euler* orientation::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::axis* orientation::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::quaternion& orientation::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::euler& orientation::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::axis& orientation::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::quaternion& orientation::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::euler& orientation::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::axis& orientation::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const orientation& var) {
  switch (var.m_type) { 
  case orientation::Type::Type0:
    return functor(var.m_impl.m_value0);
  case orientation::Type::Type1:
    return functor(var.m_impl.m_value1);
  case orientation::Type::Type2:
    return functor(var.m_impl.m_value2);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  orientation& var) {
  switch (var.m_type) { 
  case orientation::Type::Type0:
    return functor(var.m_impl.m_value0);
  case orientation::Type::Type1:
    return functor(var.m_impl.m_value1);
  case orientation::Type::Type2:
    return functor(var.m_impl.m_value2);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  orientation&& var) {
  switch (var.m_type) { 
  case orientation::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case orientation::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case orientation::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const orientation& var) {
  switch (var.m_type) { 
  case orientation::Type::Type0:
    return functor(var.m_impl.m_value0);
  case orientation::Type::Type1:
    return functor(var.m_impl.m_value1);
  case orientation::Type::Type2:
    return functor(var.m_impl.m_value2);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  orientation& var) {
  switch (var.m_type) { 
  case orientation::Type::Type0:
    return functor(var.m_impl.m_value0);
  case orientation::Type::Type1:
    return functor(var.m_impl.m_value1);
  case orientation::Type::Type2:
    return functor(var.m_impl.m_value2);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  orientation&& var) {
  switch (var.m_type) { 
  case orientation::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case orientation::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case orientation::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  default: return functor();
  }
}
inline bool operator==(const orientation& lhs, const orientation& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case orientation::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
    case orientation::Type::Type2:
      return lhs.m_impl.m_value2==rhs.m_impl.m_value2;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const orientation& lhs, const orientation& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case orientation::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    case orientation::Type::Type2:
      return lhs.m_impl.m_value2!=rhs.m_impl.m_value2;
    default: return false;
  }
  return true; 
}
inline bool operator==(const orientation& lhs, const ossia::quaternion& rhs){ 
  return (lhs.m_type == orientation::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::quaternion& lhs, const orientation& rhs){ 
  return (rhs.m_type == orientation::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const orientation& lhs, const ossia::quaternion& rhs){ 
  return (lhs.m_type != orientation::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::quaternion& lhs, const orientation& rhs){ 
  return (rhs.m_type != orientation::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const orientation& lhs, const ossia::euler& rhs){ 
  return (lhs.m_type == orientation::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::euler& lhs, const orientation& rhs){ 
  return (rhs.m_type == orientation::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const orientation& lhs, const ossia::euler& rhs){ 
  return (lhs.m_type != orientation::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::euler& lhs, const orientation& rhs){ 
  return (rhs.m_type != orientation::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
inline bool operator==(const orientation& lhs, const ossia::axis& rhs){ 
  return (lhs.m_type == orientation::Type::Type2) && (lhs.m_impl.m_value2 == rhs); 
}
inline bool operator==(const ossia::axis& lhs, const orientation& rhs){ 
  return (rhs.m_type == orientation::Type::Type2) && (rhs.m_impl.m_value2 == lhs); 
}
inline bool operator!=(const orientation& lhs, const ossia::axis& rhs){ 
  return (lhs.m_type != orientation::Type::Type2) || (lhs.m_impl.m_value2 != rhs); 
}
inline bool operator!=(const ossia::axis& lhs, const orientation& rhs){ 
  return (rhs.m_type != orientation::Type::Type2) || (rhs.m_impl.m_value2 != lhs); 
}
struct position
{
public:
struct dummy_t {};
union Impl { 
ossia::cartesian_3d m_value0;
 Impl(ossia::cartesian_3d v): m_value0{v} { }
Impl& operator=(ossia::cartesian_3d v) { m_value0 = v; return *this; }

ossia::cartesian_2d m_value1;
 Impl(ossia::cartesian_2d v): m_value1{v} { }
Impl& operator=(ossia::cartesian_2d v) { m_value1 = v; return *this; }

ossia::spherical m_value2;
 Impl(ossia::spherical v): m_value2{v} { }
Impl& operator=(ossia::spherical v) { m_value2 = v; return *this; }

ossia::polar m_value3;
 Impl(ossia::polar v): m_value3{v} { }
Impl& operator=(ossia::polar v) { m_value3 = v; return *this; }

ossia::opengl m_value4;
 Impl(ossia::opengl v): m_value4{v} { }
Impl& operator=(ossia::opengl v) { m_value4 = v; return *this; }

ossia::cylindrical m_value5;
 Impl(ossia::cylindrical v): m_value5{v} { }
Impl& operator=(ossia::cylindrical v) { m_value5 = v; return *this; }

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
position(): m_type{Npos} { }
~position() { destruct_impl(); }
 position(ossia::cartesian_3d v): m_impl{v}, m_type{Type0} { }
 position(ossia::cartesian_2d v): m_impl{v}, m_type{Type1} { }
 position(ossia::spherical v): m_impl{v}, m_type{Type2} { }
 position(ossia::polar v): m_impl{v}, m_type{Type3} { }
 position(ossia::opengl v): m_impl{v}, m_type{Type4} { }
 position(ossia::cylindrical v): m_impl{v}, m_type{Type5} { }
position(const position& other):
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
position(position&& other):
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
position& operator=(const position& other){ 
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
position& operator=(position&& other)
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
template<> inline const ossia::cartesian_3d* position::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::cartesian_2d* position::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::spherical* position::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::polar* position::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::opengl* position::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::cylindrical* position::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::cartesian_3d* position::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::cartesian_2d* position::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::spherical* position::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::polar* position::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::opengl* position::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::cylindrical* position::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::cartesian_3d& position::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::cartesian_2d& position::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::spherical& position::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::polar& position::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::opengl& position::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::cylindrical& position::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::cartesian_3d& position::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::cartesian_2d& position::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::spherical& position::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::polar& position::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::opengl& position::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::cylindrical& position::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const position& var) {
  switch (var.m_type) { 
  case position::Type::Type0:
    return functor(var.m_impl.m_value0);
  case position::Type::Type1:
    return functor(var.m_impl.m_value1);
  case position::Type::Type2:
    return functor(var.m_impl.m_value2);
  case position::Type::Type3:
    return functor(var.m_impl.m_value3);
  case position::Type::Type4:
    return functor(var.m_impl.m_value4);
  case position::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  position& var) {
  switch (var.m_type) { 
  case position::Type::Type0:
    return functor(var.m_impl.m_value0);
  case position::Type::Type1:
    return functor(var.m_impl.m_value1);
  case position::Type::Type2:
    return functor(var.m_impl.m_value2);
  case position::Type::Type3:
    return functor(var.m_impl.m_value3);
  case position::Type::Type4:
    return functor(var.m_impl.m_value4);
  case position::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  position&& var) {
  switch (var.m_type) { 
  case position::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case position::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case position::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case position::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case position::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case position::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const position& var) {
  switch (var.m_type) { 
  case position::Type::Type0:
    return functor(var.m_impl.m_value0);
  case position::Type::Type1:
    return functor(var.m_impl.m_value1);
  case position::Type::Type2:
    return functor(var.m_impl.m_value2);
  case position::Type::Type3:
    return functor(var.m_impl.m_value3);
  case position::Type::Type4:
    return functor(var.m_impl.m_value4);
  case position::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  position& var) {
  switch (var.m_type) { 
  case position::Type::Type0:
    return functor(var.m_impl.m_value0);
  case position::Type::Type1:
    return functor(var.m_impl.m_value1);
  case position::Type::Type2:
    return functor(var.m_impl.m_value2);
  case position::Type::Type3:
    return functor(var.m_impl.m_value3);
  case position::Type::Type4:
    return functor(var.m_impl.m_value4);
  case position::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  position&& var) {
  switch (var.m_type) { 
  case position::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case position::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case position::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case position::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case position::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case position::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  default: return functor();
  }
}
inline bool operator==(const position& lhs, const position& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case position::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
    case position::Type::Type2:
      return lhs.m_impl.m_value2==rhs.m_impl.m_value2;
    case position::Type::Type4:
      return lhs.m_impl.m_value4==rhs.m_impl.m_value4;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const position& lhs, const position& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case position::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    case position::Type::Type2:
      return lhs.m_impl.m_value2!=rhs.m_impl.m_value2;
    case position::Type::Type4:
      return lhs.m_impl.m_value4!=rhs.m_impl.m_value4;
    default: return false;
  }
  return true; 
}
inline bool operator==(const position& lhs, const ossia::cartesian_3d& rhs){ 
  return (lhs.m_type == position::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::cartesian_3d& lhs, const position& rhs){ 
  return (rhs.m_type == position::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const position& lhs, const ossia::cartesian_3d& rhs){ 
  return (lhs.m_type != position::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::cartesian_3d& lhs, const position& rhs){ 
  return (rhs.m_type != position::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const position& lhs, const ossia::cartesian_2d& rhs){ 
  return (lhs.m_type == position::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::cartesian_2d& lhs, const position& rhs){ 
  return (rhs.m_type == position::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const position& lhs, const ossia::cartesian_2d& rhs){ 
  return (lhs.m_type != position::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::cartesian_2d& lhs, const position& rhs){ 
  return (rhs.m_type != position::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
inline bool operator==(const position& lhs, const ossia::spherical& rhs){ 
  return (lhs.m_type == position::Type::Type2) && (lhs.m_impl.m_value2 == rhs); 
}
inline bool operator==(const ossia::spherical& lhs, const position& rhs){ 
  return (rhs.m_type == position::Type::Type2) && (rhs.m_impl.m_value2 == lhs); 
}
inline bool operator!=(const position& lhs, const ossia::spherical& rhs){ 
  return (lhs.m_type != position::Type::Type2) || (lhs.m_impl.m_value2 != rhs); 
}
inline bool operator!=(const ossia::spherical& lhs, const position& rhs){ 
  return (rhs.m_type != position::Type::Type2) || (rhs.m_impl.m_value2 != lhs); 
}
inline bool operator==(const position& lhs, const ossia::polar& rhs){ 
  return (lhs.m_type == position::Type::Type3) && (lhs.m_impl.m_value3 == rhs); 
}
inline bool operator==(const ossia::polar& lhs, const position& rhs){ 
  return (rhs.m_type == position::Type::Type3) && (rhs.m_impl.m_value3 == lhs); 
}
inline bool operator!=(const position& lhs, const ossia::polar& rhs){ 
  return (lhs.m_type != position::Type::Type3) || (lhs.m_impl.m_value3 != rhs); 
}
inline bool operator!=(const ossia::polar& lhs, const position& rhs){ 
  return (rhs.m_type != position::Type::Type3) || (rhs.m_impl.m_value3 != lhs); 
}
inline bool operator==(const position& lhs, const ossia::opengl& rhs){ 
  return (lhs.m_type == position::Type::Type4) && (lhs.m_impl.m_value4 == rhs); 
}
inline bool operator==(const ossia::opengl& lhs, const position& rhs){ 
  return (rhs.m_type == position::Type::Type4) && (rhs.m_impl.m_value4 == lhs); 
}
inline bool operator!=(const position& lhs, const ossia::opengl& rhs){ 
  return (lhs.m_type != position::Type::Type4) || (lhs.m_impl.m_value4 != rhs); 
}
inline bool operator!=(const ossia::opengl& lhs, const position& rhs){ 
  return (rhs.m_type != position::Type::Type4) || (rhs.m_impl.m_value4 != lhs); 
}
inline bool operator==(const position& lhs, const ossia::cylindrical& rhs){ 
  return (lhs.m_type == position::Type::Type5) && (lhs.m_impl.m_value5 == rhs); 
}
inline bool operator==(const ossia::cylindrical& lhs, const position& rhs){ 
  return (rhs.m_type == position::Type::Type5) && (rhs.m_impl.m_value5 == lhs); 
}
inline bool operator!=(const position& lhs, const ossia::cylindrical& rhs){ 
  return (lhs.m_type != position::Type::Type5) || (lhs.m_impl.m_value5 != rhs); 
}
inline bool operator!=(const ossia::cylindrical& lhs, const position& rhs){ 
  return (rhs.m_type != position::Type::Type5) || (rhs.m_impl.m_value5 != lhs); 
}
struct speed
{
public:
struct dummy_t {};
union Impl { 
ossia::meter_per_second m_value0;
 Impl(ossia::meter_per_second v): m_value0{v} { }
Impl& operator=(ossia::meter_per_second v) { m_value0 = v; return *this; }

ossia::miles_per_hour m_value1;
 Impl(ossia::miles_per_hour v): m_value1{v} { }
Impl& operator=(ossia::miles_per_hour v) { m_value1 = v; return *this; }

ossia::kilometer_per_hour m_value2;
 Impl(ossia::kilometer_per_hour v): m_value2{v} { }
Impl& operator=(ossia::kilometer_per_hour v) { m_value2 = v; return *this; }

ossia::knot m_value3;
 Impl(ossia::knot v): m_value3{v} { }
Impl& operator=(ossia::knot v) { m_value3 = v; return *this; }

ossia::foot_per_second m_value4;
 Impl(ossia::foot_per_second v): m_value4{v} { }
Impl& operator=(ossia::foot_per_second v) { m_value4 = v; return *this; }

ossia::foot_per_hour m_value5;
 Impl(ossia::foot_per_hour v): m_value5{v} { }
Impl& operator=(ossia::foot_per_hour v) { m_value5 = v; return *this; }

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
speed(): m_type{Npos} { }
~speed() { destruct_impl(); }
 speed(ossia::meter_per_second v): m_impl{v}, m_type{Type0} { }
 speed(ossia::miles_per_hour v): m_impl{v}, m_type{Type1} { }
 speed(ossia::kilometer_per_hour v): m_impl{v}, m_type{Type2} { }
 speed(ossia::knot v): m_impl{v}, m_type{Type3} { }
 speed(ossia::foot_per_second v): m_impl{v}, m_type{Type4} { }
 speed(ossia::foot_per_hour v): m_impl{v}, m_type{Type5} { }
speed(const speed& other):
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
speed(speed&& other):
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
speed& operator=(const speed& other){ 
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
speed& operator=(speed&& other)
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
template<> inline const ossia::meter_per_second* speed::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::miles_per_hour* speed::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::kilometer_per_hour* speed::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::knot* speed::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::foot_per_second* speed::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::foot_per_hour* speed::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::meter_per_second* speed::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::miles_per_hour* speed::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::kilometer_per_hour* speed::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::knot* speed::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::foot_per_second* speed::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::foot_per_hour* speed::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::meter_per_second& speed::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::miles_per_hour& speed::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::kilometer_per_hour& speed::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::knot& speed::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::foot_per_second& speed::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::foot_per_hour& speed::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::meter_per_second& speed::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::miles_per_hour& speed::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::kilometer_per_hour& speed::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::knot& speed::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::foot_per_second& speed::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::foot_per_hour& speed::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const speed& var) {
  switch (var.m_type) { 
  case speed::Type::Type0:
    return functor(var.m_impl.m_value0);
  case speed::Type::Type1:
    return functor(var.m_impl.m_value1);
  case speed::Type::Type2:
    return functor(var.m_impl.m_value2);
  case speed::Type::Type3:
    return functor(var.m_impl.m_value3);
  case speed::Type::Type4:
    return functor(var.m_impl.m_value4);
  case speed::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  speed& var) {
  switch (var.m_type) { 
  case speed::Type::Type0:
    return functor(var.m_impl.m_value0);
  case speed::Type::Type1:
    return functor(var.m_impl.m_value1);
  case speed::Type::Type2:
    return functor(var.m_impl.m_value2);
  case speed::Type::Type3:
    return functor(var.m_impl.m_value3);
  case speed::Type::Type4:
    return functor(var.m_impl.m_value4);
  case speed::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  speed&& var) {
  switch (var.m_type) { 
  case speed::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case speed::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case speed::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case speed::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case speed::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case speed::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const speed& var) {
  switch (var.m_type) { 
  case speed::Type::Type0:
    return functor(var.m_impl.m_value0);
  case speed::Type::Type1:
    return functor(var.m_impl.m_value1);
  case speed::Type::Type2:
    return functor(var.m_impl.m_value2);
  case speed::Type::Type3:
    return functor(var.m_impl.m_value3);
  case speed::Type::Type4:
    return functor(var.m_impl.m_value4);
  case speed::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  speed& var) {
  switch (var.m_type) { 
  case speed::Type::Type0:
    return functor(var.m_impl.m_value0);
  case speed::Type::Type1:
    return functor(var.m_impl.m_value1);
  case speed::Type::Type2:
    return functor(var.m_impl.m_value2);
  case speed::Type::Type3:
    return functor(var.m_impl.m_value3);
  case speed::Type::Type4:
    return functor(var.m_impl.m_value4);
  case speed::Type::Type5:
    return functor(var.m_impl.m_value5);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  speed&& var) {
  switch (var.m_type) { 
  case speed::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case speed::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case speed::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case speed::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case speed::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case speed::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  default: return functor();
  }
}
inline bool operator==(const speed& lhs, const speed& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case speed::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
    case speed::Type::Type2:
      return lhs.m_impl.m_value2==rhs.m_impl.m_value2;
    case speed::Type::Type4:
      return lhs.m_impl.m_value4==rhs.m_impl.m_value4;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const speed& lhs, const speed& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case speed::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    case speed::Type::Type2:
      return lhs.m_impl.m_value2!=rhs.m_impl.m_value2;
    case speed::Type::Type4:
      return lhs.m_impl.m_value4!=rhs.m_impl.m_value4;
    default: return false;
  }
  return true; 
}
inline bool operator==(const speed& lhs, const ossia::meter_per_second& rhs){ 
  return (lhs.m_type == speed::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::meter_per_second& lhs, const speed& rhs){ 
  return (rhs.m_type == speed::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const speed& lhs, const ossia::meter_per_second& rhs){ 
  return (lhs.m_type != speed::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::meter_per_second& lhs, const speed& rhs){ 
  return (rhs.m_type != speed::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const speed& lhs, const ossia::miles_per_hour& rhs){ 
  return (lhs.m_type == speed::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::miles_per_hour& lhs, const speed& rhs){ 
  return (rhs.m_type == speed::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const speed& lhs, const ossia::miles_per_hour& rhs){ 
  return (lhs.m_type != speed::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::miles_per_hour& lhs, const speed& rhs){ 
  return (rhs.m_type != speed::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
inline bool operator==(const speed& lhs, const ossia::kilometer_per_hour& rhs){ 
  return (lhs.m_type == speed::Type::Type2) && (lhs.m_impl.m_value2 == rhs); 
}
inline bool operator==(const ossia::kilometer_per_hour& lhs, const speed& rhs){ 
  return (rhs.m_type == speed::Type::Type2) && (rhs.m_impl.m_value2 == lhs); 
}
inline bool operator!=(const speed& lhs, const ossia::kilometer_per_hour& rhs){ 
  return (lhs.m_type != speed::Type::Type2) || (lhs.m_impl.m_value2 != rhs); 
}
inline bool operator!=(const ossia::kilometer_per_hour& lhs, const speed& rhs){ 
  return (rhs.m_type != speed::Type::Type2) || (rhs.m_impl.m_value2 != lhs); 
}
inline bool operator==(const speed& lhs, const ossia::knot& rhs){ 
  return (lhs.m_type == speed::Type::Type3) && (lhs.m_impl.m_value3 == rhs); 
}
inline bool operator==(const ossia::knot& lhs, const speed& rhs){ 
  return (rhs.m_type == speed::Type::Type3) && (rhs.m_impl.m_value3 == lhs); 
}
inline bool operator!=(const speed& lhs, const ossia::knot& rhs){ 
  return (lhs.m_type != speed::Type::Type3) || (lhs.m_impl.m_value3 != rhs); 
}
inline bool operator!=(const ossia::knot& lhs, const speed& rhs){ 
  return (rhs.m_type != speed::Type::Type3) || (rhs.m_impl.m_value3 != lhs); 
}
inline bool operator==(const speed& lhs, const ossia::foot_per_second& rhs){ 
  return (lhs.m_type == speed::Type::Type4) && (lhs.m_impl.m_value4 == rhs); 
}
inline bool operator==(const ossia::foot_per_second& lhs, const speed& rhs){ 
  return (rhs.m_type == speed::Type::Type4) && (rhs.m_impl.m_value4 == lhs); 
}
inline bool operator!=(const speed& lhs, const ossia::foot_per_second& rhs){ 
  return (lhs.m_type != speed::Type::Type4) || (lhs.m_impl.m_value4 != rhs); 
}
inline bool operator!=(const ossia::foot_per_second& lhs, const speed& rhs){ 
  return (rhs.m_type != speed::Type::Type4) || (rhs.m_impl.m_value4 != lhs); 
}
inline bool operator==(const speed& lhs, const ossia::foot_per_hour& rhs){ 
  return (lhs.m_type == speed::Type::Type5) && (lhs.m_impl.m_value5 == rhs); 
}
inline bool operator==(const ossia::foot_per_hour& lhs, const speed& rhs){ 
  return (rhs.m_type == speed::Type::Type5) && (rhs.m_impl.m_value5 == lhs); 
}
inline bool operator!=(const speed& lhs, const ossia::foot_per_hour& rhs){ 
  return (lhs.m_type != speed::Type::Type5) || (lhs.m_impl.m_value5 != rhs); 
}
inline bool operator!=(const ossia::foot_per_hour& lhs, const speed& rhs){ 
  return (rhs.m_type != speed::Type::Type5) || (rhs.m_impl.m_value5 != lhs); 
}
struct time
{
public:
struct dummy_t {};
union Impl { 
ossia::second m_value0;
 Impl(ossia::second v): m_value0{v} { }
Impl& operator=(ossia::second v) { m_value0 = v; return *this; }

ossia::bark m_value1;
 Impl(ossia::bark v): m_value1{v} { }
Impl& operator=(ossia::bark v) { m_value1 = v; return *this; }

ossia::bpm m_value2;
 Impl(ossia::bpm v): m_value2{v} { }
Impl& operator=(ossia::bpm v) { m_value2 = v; return *this; }

ossia::cent m_value3;
 Impl(ossia::cent v): m_value3{v} { }
Impl& operator=(ossia::cent v) { m_value3 = v; return *this; }

ossia::frequency m_value4;
 Impl(ossia::frequency v): m_value4{v} { }
Impl& operator=(ossia::frequency v) { m_value4 = v; return *this; }

ossia::mel m_value5;
 Impl(ossia::mel v): m_value5{v} { }
Impl& operator=(ossia::mel v) { m_value5 = v; return *this; }

ossia::midi_pitch m_value6;
 Impl(ossia::midi_pitch v): m_value6{v} { }
Impl& operator=(ossia::midi_pitch v) { m_value6 = v; return *this; }

ossia::millisecond m_value7;
 Impl(ossia::millisecond v): m_value7{v} { }
Impl& operator=(ossia::millisecond v) { m_value7 = v; return *this; }

ossia::playback_speed m_value8;
 Impl(ossia::playback_speed v): m_value8{v} { }
Impl& operator=(ossia::playback_speed v) { m_value8 = v; return *this; }

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
time(): m_type{Npos} { }
~time() { destruct_impl(); }
 time(ossia::second v): m_impl{v}, m_type{Type0} { }
 time(ossia::bark v): m_impl{v}, m_type{Type1} { }
 time(ossia::bpm v): m_impl{v}, m_type{Type2} { }
 time(ossia::cent v): m_impl{v}, m_type{Type3} { }
 time(ossia::frequency v): m_impl{v}, m_type{Type4} { }
 time(ossia::mel v): m_impl{v}, m_type{Type5} { }
 time(ossia::midi_pitch v): m_impl{v}, m_type{Type6} { }
 time(ossia::millisecond v): m_impl{v}, m_type{Type7} { }
 time(ossia::playback_speed v): m_impl{v}, m_type{Type8} { }
time(const time& other):
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
time(time&& other):
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
time& operator=(const time& other){ 
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
time& operator=(time&& other)
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
template<> inline const ossia::second* time::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::bark* time::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::bpm* time::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::cent* time::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::frequency* time::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::mel* time::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::midi_pitch* time::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline const ossia::millisecond* time::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline const ossia::playback_speed* time::target() const { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline ossia::second* time::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::bark* time::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::bpm* time::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::cent* time::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::frequency* time::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::mel* time::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::midi_pitch* time::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline ossia::millisecond* time::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline ossia::playback_speed* time::target() { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline const ossia::second& time::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::bark& time::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::bpm& time::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::cent& time::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::frequency& time::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::mel& time::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline const ossia::midi_pitch& time::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline const ossia::millisecond& time::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline const ossia::playback_speed& time::get() const { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> inline ossia::second& time::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::bark& time::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::bpm& time::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::cent& time::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::frequency& time::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::mel& time::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::midi_pitch& time::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline ossia::millisecond& time::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline ossia::playback_speed& time::get() { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const time& var) {
  switch (var.m_type) { 
  case time::Type::Type0:
    return functor(var.m_impl.m_value0);
  case time::Type::Type1:
    return functor(var.m_impl.m_value1);
  case time::Type::Type2:
    return functor(var.m_impl.m_value2);
  case time::Type::Type3:
    return functor(var.m_impl.m_value3);
  case time::Type::Type4:
    return functor(var.m_impl.m_value4);
  case time::Type::Type5:
    return functor(var.m_impl.m_value5);
  case time::Type::Type6:
    return functor(var.m_impl.m_value6);
  case time::Type::Type7:
    return functor(var.m_impl.m_value7);
  case time::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  time& var) {
  switch (var.m_type) { 
  case time::Type::Type0:
    return functor(var.m_impl.m_value0);
  case time::Type::Type1:
    return functor(var.m_impl.m_value1);
  case time::Type::Type2:
    return functor(var.m_impl.m_value2);
  case time::Type::Type3:
    return functor(var.m_impl.m_value3);
  case time::Type::Type4:
    return functor(var.m_impl.m_value4);
  case time::Type::Type5:
    return functor(var.m_impl.m_value5);
  case time::Type::Type6:
    return functor(var.m_impl.m_value6);
  case time::Type::Type7:
    return functor(var.m_impl.m_value7);
  case time::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  time&& var) {
  switch (var.m_type) { 
  case time::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case time::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case time::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case time::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case time::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case time::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case time::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case time::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case time::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const time& var) {
  switch (var.m_type) { 
  case time::Type::Type0:
    return functor(var.m_impl.m_value0);
  case time::Type::Type1:
    return functor(var.m_impl.m_value1);
  case time::Type::Type2:
    return functor(var.m_impl.m_value2);
  case time::Type::Type3:
    return functor(var.m_impl.m_value3);
  case time::Type::Type4:
    return functor(var.m_impl.m_value4);
  case time::Type::Type5:
    return functor(var.m_impl.m_value5);
  case time::Type::Type6:
    return functor(var.m_impl.m_value6);
  case time::Type::Type7:
    return functor(var.m_impl.m_value7);
  case time::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  time& var) {
  switch (var.m_type) { 
  case time::Type::Type0:
    return functor(var.m_impl.m_value0);
  case time::Type::Type1:
    return functor(var.m_impl.m_value1);
  case time::Type::Type2:
    return functor(var.m_impl.m_value2);
  case time::Type::Type3:
    return functor(var.m_impl.m_value3);
  case time::Type::Type4:
    return functor(var.m_impl.m_value4);
  case time::Type::Type5:
    return functor(var.m_impl.m_value5);
  case time::Type::Type6:
    return functor(var.m_impl.m_value6);
  case time::Type::Type7:
    return functor(var.m_impl.m_value7);
  case time::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  time&& var) {
  switch (var.m_type) { 
  case time::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case time::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case time::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case time::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case time::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case time::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case time::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case time::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case time::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  default: return functor();
  }
}
inline bool operator==(const time& lhs, const time& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case time::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
    case time::Type::Type2:
      return lhs.m_impl.m_value2==rhs.m_impl.m_value2;
    case time::Type::Type4:
      return lhs.m_impl.m_value4==rhs.m_impl.m_value4;
    case time::Type::Type6:
      return lhs.m_impl.m_value6==rhs.m_impl.m_value6;
    case time::Type::Type8:
      return lhs.m_impl.m_value8==rhs.m_impl.m_value8;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const time& lhs, const time& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case time::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    case time::Type::Type2:
      return lhs.m_impl.m_value2!=rhs.m_impl.m_value2;
    case time::Type::Type4:
      return lhs.m_impl.m_value4!=rhs.m_impl.m_value4;
    case time::Type::Type6:
      return lhs.m_impl.m_value6!=rhs.m_impl.m_value6;
    case time::Type::Type8:
      return lhs.m_impl.m_value8!=rhs.m_impl.m_value8;
    default: return false;
  }
  return true; 
}
inline bool operator==(const time& lhs, const ossia::second& rhs){ 
  return (lhs.m_type == time::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::second& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::second& rhs){ 
  return (lhs.m_type != time::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::second& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const time& lhs, const ossia::bark& rhs){ 
  return (lhs.m_type == time::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::bark& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::bark& rhs){ 
  return (lhs.m_type != time::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::bark& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
inline bool operator==(const time& lhs, const ossia::bpm& rhs){ 
  return (lhs.m_type == time::Type::Type2) && (lhs.m_impl.m_value2 == rhs); 
}
inline bool operator==(const ossia::bpm& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type2) && (rhs.m_impl.m_value2 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::bpm& rhs){ 
  return (lhs.m_type != time::Type::Type2) || (lhs.m_impl.m_value2 != rhs); 
}
inline bool operator!=(const ossia::bpm& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type2) || (rhs.m_impl.m_value2 != lhs); 
}
inline bool operator==(const time& lhs, const ossia::cent& rhs){ 
  return (lhs.m_type == time::Type::Type3) && (lhs.m_impl.m_value3 == rhs); 
}
inline bool operator==(const ossia::cent& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type3) && (rhs.m_impl.m_value3 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::cent& rhs){ 
  return (lhs.m_type != time::Type::Type3) || (lhs.m_impl.m_value3 != rhs); 
}
inline bool operator!=(const ossia::cent& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type3) || (rhs.m_impl.m_value3 != lhs); 
}
inline bool operator==(const time& lhs, const ossia::frequency& rhs){ 
  return (lhs.m_type == time::Type::Type4) && (lhs.m_impl.m_value4 == rhs); 
}
inline bool operator==(const ossia::frequency& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type4) && (rhs.m_impl.m_value4 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::frequency& rhs){ 
  return (lhs.m_type != time::Type::Type4) || (lhs.m_impl.m_value4 != rhs); 
}
inline bool operator!=(const ossia::frequency& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type4) || (rhs.m_impl.m_value4 != lhs); 
}
inline bool operator==(const time& lhs, const ossia::mel& rhs){ 
  return (lhs.m_type == time::Type::Type5) && (lhs.m_impl.m_value5 == rhs); 
}
inline bool operator==(const ossia::mel& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type5) && (rhs.m_impl.m_value5 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::mel& rhs){ 
  return (lhs.m_type != time::Type::Type5) || (lhs.m_impl.m_value5 != rhs); 
}
inline bool operator!=(const ossia::mel& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type5) || (rhs.m_impl.m_value5 != lhs); 
}
inline bool operator==(const time& lhs, const ossia::midi_pitch& rhs){ 
  return (lhs.m_type == time::Type::Type6) && (lhs.m_impl.m_value6 == rhs); 
}
inline bool operator==(const ossia::midi_pitch& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type6) && (rhs.m_impl.m_value6 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::midi_pitch& rhs){ 
  return (lhs.m_type != time::Type::Type6) || (lhs.m_impl.m_value6 != rhs); 
}
inline bool operator!=(const ossia::midi_pitch& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type6) || (rhs.m_impl.m_value6 != lhs); 
}
inline bool operator==(const time& lhs, const ossia::millisecond& rhs){ 
  return (lhs.m_type == time::Type::Type7) && (lhs.m_impl.m_value7 == rhs); 
}
inline bool operator==(const ossia::millisecond& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type7) && (rhs.m_impl.m_value7 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::millisecond& rhs){ 
  return (lhs.m_type != time::Type::Type7) || (lhs.m_impl.m_value7 != rhs); 
}
inline bool operator!=(const ossia::millisecond& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type7) || (rhs.m_impl.m_value7 != lhs); 
}
inline bool operator==(const time& lhs, const ossia::playback_speed& rhs){ 
  return (lhs.m_type == time::Type::Type8) && (lhs.m_impl.m_value8 == rhs); 
}
inline bool operator==(const ossia::playback_speed& lhs, const time& rhs){ 
  return (rhs.m_type == time::Type::Type8) && (rhs.m_impl.m_value8 == lhs); 
}
inline bool operator!=(const time& lhs, const ossia::playback_speed& rhs){ 
  return (lhs.m_type != time::Type::Type8) || (lhs.m_impl.m_value8 != rhs); 
}
inline bool operator!=(const ossia::playback_speed& lhs, const time& rhs){ 
  return (rhs.m_type != time::Type::Type8) || (rhs.m_impl.m_value8 != lhs); 
}
struct strong_value_variant
{
public:
struct dummy_t {};
union Impl { 
ossia::value m_value0;
Impl(const ossia::value& v): m_value0{v} { }
Impl(ossia::value&& v): m_value0{std::move(v)} { }
Impl& operator=(const ossia::value& v) { m_value0 = v; return *this; }
Impl& operator=(ossia::value&& v) { m_value0 = std::move(v); return *this; }

ossia::distance m_value1;
 Impl(ossia::distance v): m_value1{v} { }
Impl& operator=(ossia::distance v) { m_value1 = v; return *this; }

ossia::position m_value2;
 Impl(ossia::position v): m_value2{v} { }
Impl& operator=(ossia::position v) { m_value2 = v; return *this; }

ossia::speed m_value3;
 Impl(ossia::speed v): m_value3{v} { }
Impl& operator=(ossia::speed v) { m_value3 = v; return *this; }

ossia::orientation m_value4;
 Impl(ossia::orientation v): m_value4{v} { }
Impl& operator=(ossia::orientation v) { m_value4 = v; return *this; }

ossia::angle m_value5;
 Impl(ossia::angle v): m_value5{v} { }
Impl& operator=(ossia::angle v) { m_value5 = v; return *this; }

ossia::color m_value6;
 Impl(ossia::color v): m_value6{v} { }
Impl& operator=(ossia::color v) { m_value6 = v; return *this; }

ossia::gain m_value7;
 Impl(ossia::gain v): m_value7{v} { }
Impl& operator=(ossia::gain v) { m_value7 = v; return *this; }

ossia::time m_value8;
 Impl(ossia::time v): m_value8{v} { }
Impl& operator=(ossia::time v) { m_value8 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Type2, Type3, Type4, Type5, Type6, Type7, Type8, Npos
};

void destruct_impl() { 
switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0.~value();
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
strong_value_variant(): m_type{Npos} { }
~strong_value_variant() { destruct_impl(); }
strong_value_variant(const ossia::value& v): m_impl{v}, m_type{Type0} { }
strong_value_variant(ossia::value&& v): m_impl{v}, m_type{Type0} { }
 strong_value_variant(ossia::distance v): m_impl{v}, m_type{Type1} { }
 strong_value_variant(ossia::position v): m_impl{v}, m_type{Type2} { }
 strong_value_variant(ossia::speed v): m_impl{v}, m_type{Type3} { }
 strong_value_variant(ossia::orientation v): m_impl{v}, m_type{Type4} { }
 strong_value_variant(ossia::angle v): m_impl{v}, m_type{Type5} { }
 strong_value_variant(ossia::color v): m_impl{v}, m_type{Type6} { }
 strong_value_variant(ossia::gain v): m_impl{v}, m_type{Type7} { }
 strong_value_variant(ossia::time v): m_impl{v}, m_type{Type8} { }
strong_value_variant(const strong_value_variant& other):
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
strong_value_variant(strong_value_variant&& other):
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
strong_value_variant& operator=(const strong_value_variant& other){ 
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
strong_value_variant& operator=(strong_value_variant&& other)
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
template<> inline const ossia::value* strong_value_variant::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline const ossia::distance* strong_value_variant::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline const ossia::position* strong_value_variant::target() const { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline const ossia::speed* strong_value_variant::target() const { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline const ossia::orientation* strong_value_variant::target() const { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline const ossia::angle* strong_value_variant::target() const { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline const ossia::color* strong_value_variant::target() const { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline const ossia::gain* strong_value_variant::target() const { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline const ossia::time* strong_value_variant::target() const { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline ossia::value* strong_value_variant::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> inline ossia::distance* strong_value_variant::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> inline ossia::position* strong_value_variant::target() { 
  if(m_type == Type2) 
    return &m_impl.m_value2 ;
  return nullptr; 
}
template<> inline ossia::speed* strong_value_variant::target() { 
  if(m_type == Type3) 
    return &m_impl.m_value3 ;
  return nullptr; 
}
template<> inline ossia::orientation* strong_value_variant::target() { 
  if(m_type == Type4) 
    return &m_impl.m_value4 ;
  return nullptr; 
}
template<> inline ossia::angle* strong_value_variant::target() { 
  if(m_type == Type5) 
    return &m_impl.m_value5 ;
  return nullptr; 
}
template<> inline ossia::color* strong_value_variant::target() { 
  if(m_type == Type6) 
    return &m_impl.m_value6 ;
  return nullptr; 
}
template<> inline ossia::gain* strong_value_variant::target() { 
  if(m_type == Type7) 
    return &m_impl.m_value7 ;
  return nullptr; 
}
template<> inline ossia::time* strong_value_variant::target() { 
  if(m_type == Type8) 
    return &m_impl.m_value8 ;
  return nullptr; 
}
template<> inline const ossia::value& strong_value_variant::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline const ossia::distance& strong_value_variant::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline const ossia::position& strong_value_variant::get() const { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline const ossia::speed& strong_value_variant::get() const { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline const ossia::orientation& strong_value_variant::get() const { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline const ossia::angle& strong_value_variant::get() const { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline const ossia::color& strong_value_variant::get() const { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline const ossia::gain& strong_value_variant::get() const { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline const ossia::time& strong_value_variant::get() const { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<> inline ossia::value& strong_value_variant::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> inline ossia::distance& strong_value_variant::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> inline ossia::position& strong_value_variant::get() { 
  if(m_type == Type2) 
    return m_impl.m_value2 ;
  throw; 
}
template<> inline ossia::speed& strong_value_variant::get() { 
  if(m_type == Type3) 
    return m_impl.m_value3 ;
  throw; 
}
template<> inline ossia::orientation& strong_value_variant::get() { 
  if(m_type == Type4) 
    return m_impl.m_value4 ;
  throw; 
}
template<> inline ossia::angle& strong_value_variant::get() { 
  if(m_type == Type5) 
    return m_impl.m_value5 ;
  throw; 
}
template<> inline ossia::color& strong_value_variant::get() { 
  if(m_type == Type6) 
    return m_impl.m_value6 ;
  throw; 
}
template<> inline ossia::gain& strong_value_variant::get() { 
  if(m_type == Type7) 
    return m_impl.m_value7 ;
  throw; 
}
template<> inline ossia::time& strong_value_variant::get() { 
  if(m_type == Type8) 
    return m_impl.m_value8 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const strong_value_variant& var) {
  switch (var.m_type) { 
  case strong_value_variant::Type::Type0:
    return functor(var.m_impl.m_value0);
  case strong_value_variant::Type::Type1:
    return functor(var.m_impl.m_value1);
  case strong_value_variant::Type::Type2:
    return functor(var.m_impl.m_value2);
  case strong_value_variant::Type::Type3:
    return functor(var.m_impl.m_value3);
  case strong_value_variant::Type::Type4:
    return functor(var.m_impl.m_value4);
  case strong_value_variant::Type::Type5:
    return functor(var.m_impl.m_value5);
  case strong_value_variant::Type::Type6:
    return functor(var.m_impl.m_value6);
  case strong_value_variant::Type::Type7:
    return functor(var.m_impl.m_value7);
  case strong_value_variant::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  strong_value_variant& var) {
  switch (var.m_type) { 
  case strong_value_variant::Type::Type0:
    return functor(var.m_impl.m_value0);
  case strong_value_variant::Type::Type1:
    return functor(var.m_impl.m_value1);
  case strong_value_variant::Type::Type2:
    return functor(var.m_impl.m_value2);
  case strong_value_variant::Type::Type3:
    return functor(var.m_impl.m_value3);
  case strong_value_variant::Type::Type4:
    return functor(var.m_impl.m_value4);
  case strong_value_variant::Type::Type5:
    return functor(var.m_impl.m_value5);
  case strong_value_variant::Type::Type6:
    return functor(var.m_impl.m_value6);
  case strong_value_variant::Type::Type7:
    return functor(var.m_impl.m_value7);
  case strong_value_variant::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  strong_value_variant&& var) {
  switch (var.m_type) { 
  case strong_value_variant::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case strong_value_variant::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case strong_value_variant::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case strong_value_variant::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case strong_value_variant::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case strong_value_variant::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case strong_value_variant::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case strong_value_variant::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case strong_value_variant::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const strong_value_variant& var) {
  switch (var.m_type) { 
  case strong_value_variant::Type::Type0:
    return functor(var.m_impl.m_value0);
  case strong_value_variant::Type::Type1:
    return functor(var.m_impl.m_value1);
  case strong_value_variant::Type::Type2:
    return functor(var.m_impl.m_value2);
  case strong_value_variant::Type::Type3:
    return functor(var.m_impl.m_value3);
  case strong_value_variant::Type::Type4:
    return functor(var.m_impl.m_value4);
  case strong_value_variant::Type::Type5:
    return functor(var.m_impl.m_value5);
  case strong_value_variant::Type::Type6:
    return functor(var.m_impl.m_value6);
  case strong_value_variant::Type::Type7:
    return functor(var.m_impl.m_value7);
  case strong_value_variant::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  strong_value_variant& var) {
  switch (var.m_type) { 
  case strong_value_variant::Type::Type0:
    return functor(var.m_impl.m_value0);
  case strong_value_variant::Type::Type1:
    return functor(var.m_impl.m_value1);
  case strong_value_variant::Type::Type2:
    return functor(var.m_impl.m_value2);
  case strong_value_variant::Type::Type3:
    return functor(var.m_impl.m_value3);
  case strong_value_variant::Type::Type4:
    return functor(var.m_impl.m_value4);
  case strong_value_variant::Type::Type5:
    return functor(var.m_impl.m_value5);
  case strong_value_variant::Type::Type6:
    return functor(var.m_impl.m_value6);
  case strong_value_variant::Type::Type7:
    return functor(var.m_impl.m_value7);
  case strong_value_variant::Type::Type8:
    return functor(var.m_impl.m_value8);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  strong_value_variant&& var) {
  switch (var.m_type) { 
  case strong_value_variant::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case strong_value_variant::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  case strong_value_variant::Type::Type2:
    return functor(std::move(var.m_impl.m_value2));
  case strong_value_variant::Type::Type3:
    return functor(std::move(var.m_impl.m_value3));
  case strong_value_variant::Type::Type4:
    return functor(std::move(var.m_impl.m_value4));
  case strong_value_variant::Type::Type5:
    return functor(std::move(var.m_impl.m_value5));
  case strong_value_variant::Type::Type6:
    return functor(std::move(var.m_impl.m_value6));
  case strong_value_variant::Type::Type7:
    return functor(std::move(var.m_impl.m_value7));
  case strong_value_variant::Type::Type8:
    return functor(std::move(var.m_impl.m_value8));
  default: return functor();
  }
}
inline bool operator==(const strong_value_variant& lhs, const strong_value_variant& rhs){ 
  if(lhs.m_type == rhs.m_type) { 
    switch(lhs.m_type) { 
    case strong_value_variant::Type::Type0:
      return lhs.m_impl.m_value0==rhs.m_impl.m_value0;
    case strong_value_variant::Type::Type2:
      return lhs.m_impl.m_value2==rhs.m_impl.m_value2;
    case strong_value_variant::Type::Type4:
      return lhs.m_impl.m_value4==rhs.m_impl.m_value4;
    case strong_value_variant::Type::Type6:
      return lhs.m_impl.m_value6==rhs.m_impl.m_value6;
    case strong_value_variant::Type::Type8:
      return lhs.m_impl.m_value8==rhs.m_impl.m_value8;
      default: return true;
    }
  }
  return false; 
}
inline bool operator!=(const strong_value_variant& lhs, const strong_value_variant& rhs){ 
  if(lhs.m_type != rhs.m_type) return true; 
  switch(lhs.m_type) { 
    case strong_value_variant::Type::Type0:
      return lhs.m_impl.m_value0!=rhs.m_impl.m_value0;
    case strong_value_variant::Type::Type2:
      return lhs.m_impl.m_value2!=rhs.m_impl.m_value2;
    case strong_value_variant::Type::Type4:
      return lhs.m_impl.m_value4!=rhs.m_impl.m_value4;
    case strong_value_variant::Type::Type6:
      return lhs.m_impl.m_value6!=rhs.m_impl.m_value6;
    case strong_value_variant::Type::Type8:
      return lhs.m_impl.m_value8!=rhs.m_impl.m_value8;
    default: return false;
  }
  return true; 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::value& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type0) && (lhs.m_impl.m_value0 == rhs); 
}
inline bool operator==(const ossia::value& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type0) && (rhs.m_impl.m_value0 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::value& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type0) || (lhs.m_impl.m_value0 != rhs); 
}
inline bool operator!=(const ossia::value& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type0) || (rhs.m_impl.m_value0 != lhs); 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::distance& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type1) && (lhs.m_impl.m_value1 == rhs); 
}
inline bool operator==(const ossia::distance& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type1) && (rhs.m_impl.m_value1 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::distance& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type1) || (lhs.m_impl.m_value1 != rhs); 
}
inline bool operator!=(const ossia::distance& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type1) || (rhs.m_impl.m_value1 != lhs); 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::position& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type2) && (lhs.m_impl.m_value2 == rhs); 
}
inline bool operator==(const ossia::position& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type2) && (rhs.m_impl.m_value2 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::position& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type2) || (lhs.m_impl.m_value2 != rhs); 
}
inline bool operator!=(const ossia::position& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type2) || (rhs.m_impl.m_value2 != lhs); 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::speed& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type3) && (lhs.m_impl.m_value3 == rhs); 
}
inline bool operator==(const ossia::speed& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type3) && (rhs.m_impl.m_value3 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::speed& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type3) || (lhs.m_impl.m_value3 != rhs); 
}
inline bool operator!=(const ossia::speed& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type3) || (rhs.m_impl.m_value3 != lhs); 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::orientation& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type4) && (lhs.m_impl.m_value4 == rhs); 
}
inline bool operator==(const ossia::orientation& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type4) && (rhs.m_impl.m_value4 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::orientation& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type4) || (lhs.m_impl.m_value4 != rhs); 
}
inline bool operator!=(const ossia::orientation& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type4) || (rhs.m_impl.m_value4 != lhs); 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::angle& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type5) && (lhs.m_impl.m_value5 == rhs); 
}
inline bool operator==(const ossia::angle& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type5) && (rhs.m_impl.m_value5 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::angle& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type5) || (lhs.m_impl.m_value5 != rhs); 
}
inline bool operator!=(const ossia::angle& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type5) || (rhs.m_impl.m_value5 != lhs); 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::color& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type6) && (lhs.m_impl.m_value6 == rhs); 
}
inline bool operator==(const ossia::color& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type6) && (rhs.m_impl.m_value6 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::color& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type6) || (lhs.m_impl.m_value6 != rhs); 
}
inline bool operator!=(const ossia::color& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type6) || (rhs.m_impl.m_value6 != lhs); 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::gain& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type7) && (lhs.m_impl.m_value7 == rhs); 
}
inline bool operator==(const ossia::gain& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type7) && (rhs.m_impl.m_value7 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::gain& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type7) || (lhs.m_impl.m_value7 != rhs); 
}
inline bool operator!=(const ossia::gain& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type7) || (rhs.m_impl.m_value7 != lhs); 
}
inline bool operator==(const strong_value_variant& lhs, const ossia::time& rhs){ 
  return (lhs.m_type == strong_value_variant::Type::Type8) && (lhs.m_impl.m_value8 == rhs); 
}
inline bool operator==(const ossia::time& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type == strong_value_variant::Type::Type8) && (rhs.m_impl.m_value8 == lhs); 
}
inline bool operator!=(const strong_value_variant& lhs, const ossia::time& rhs){ 
  return (lhs.m_type != strong_value_variant::Type::Type8) || (lhs.m_impl.m_value8 != rhs); 
}
inline bool operator!=(const ossia::time& lhs, const strong_value_variant& rhs){ 
  return (rhs.m_type != strong_value_variant::Type::Type8) || (rhs.m_impl.m_value8 != lhs); 
}
