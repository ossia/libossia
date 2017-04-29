struct behavior_variant_type
{
public:
struct dummy_t {};
union Impl { 
std::shared_ptr<ossia::curve_abstract> m_value0;
Impl(const std::shared_ptr<ossia::curve_abstract>& v): m_value0{v} { }
Impl(std::shared_ptr<ossia::curve_abstract>&& v): m_value0{std::move(v)} { }
Impl& operator=(const std::shared_ptr<ossia::curve_abstract>& v) { m_value0 = v; return *this; }
Impl& operator=(std::shared_ptr<ossia::curve_abstract>&& v) { m_value0 = std::move(v); return *this; }

std::vector<ossia::behavior> m_value1;
Impl(std::vector<ossia::behavior> v): m_value1{v} { }
Impl& operator=(std::vector<ossia::behavior> v) { m_value1 = v; return *this; }

dummy_t m_dummy;
Impl(): m_dummy{} { }
~Impl() { }
};

enum Type: int8_t { 
Type0, Type1, Npos
};

void destruct_impl() { 
switch(m_type) { 
  case Type::Type0:
    m_impl.m_value0.~shared_ptr();
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
behavior_variant_type(): m_type{Npos} { }
~behavior_variant_type() { destruct_impl(); }
behavior_variant_type(const std::shared_ptr<ossia::curve_abstract>& v): m_impl{v}, m_type{Type0} { }
behavior_variant_type(std::shared_ptr<ossia::curve_abstract>&& v): m_impl{v}, m_type{Type0} { }
behavior_variant_type(std::vector<ossia::behavior> v): m_impl{v}, m_type{Type1} { }
behavior_variant_type(const behavior_variant_type& other):
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
behavior_variant_type(behavior_variant_type&& other):
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
behavior_variant_type& operator=(const behavior_variant_type& other){ 
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
behavior_variant_type& operator=(behavior_variant_type&& other)
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
template<> const std::shared_ptr<ossia::curve_abstract>* behavior_variant_type::target() const { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> const std::vector<ossia::behavior>* behavior_variant_type::target() const { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> std::shared_ptr<ossia::curve_abstract>* behavior_variant_type::target() { 
  if(m_type == Type0) 
    return &m_impl.m_value0 ;
  return nullptr; 
}
template<> std::vector<ossia::behavior>* behavior_variant_type::target() { 
  if(m_type == Type1) 
    return &m_impl.m_value1 ;
  return nullptr; 
}
template<> const std::shared_ptr<ossia::curve_abstract>& behavior_variant_type::get() const { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> const std::vector<ossia::behavior>& behavior_variant_type::get() const { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<> std::shared_ptr<ossia::curve_abstract>& behavior_variant_type::get() { 
  if(m_type == Type0) 
    return m_impl.m_value0 ;
  throw; 
}
template<> std::vector<ossia::behavior>& behavior_variant_type::get() { 
  if(m_type == Type1) 
    return m_impl.m_value1 ;
  throw; 
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor, const behavior_variant_type& var) {
  switch (var.m_type) { 
  case behavior_variant_type::Type::Type0:
    return functor(var.m_impl.m_value0);
  case behavior_variant_type::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  behavior_variant_type& var) {
  switch (var.m_type) { 
  case behavior_variant_type::Type::Type0:
    return functor(var.m_impl.m_value0);
  case behavior_variant_type::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: throw;
  }
}
template<typename Visitor>
auto apply_nonnull(Visitor&& functor,  behavior_variant_type&& var) {
  switch (var.m_type) { 
  case behavior_variant_type::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case behavior_variant_type::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  default: throw;
  }
}
template<typename Visitor>
auto apply(Visitor&& functor, const behavior_variant_type& var) {
  switch (var.m_type) { 
  case behavior_variant_type::Type::Type0:
    return functor(var.m_impl.m_value0);
  case behavior_variant_type::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  behavior_variant_type& var) {
  switch (var.m_type) { 
  case behavior_variant_type::Type::Type0:
    return functor(var.m_impl.m_value0);
  case behavior_variant_type::Type::Type1:
    return functor(var.m_impl.m_value1);
  default: return functor();
  }
}
template<typename Visitor>
auto apply(Visitor&& functor,  behavior_variant_type&& var) {
  switch (var.m_type) { 
  case behavior_variant_type::Type::Type0:
    return functor(std::move(var.m_impl.m_value0));
  case behavior_variant_type::Type::Type1:
    return functor(std::move(var.m_impl.m_value1));
  default: return functor();
  }
}
