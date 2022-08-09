
template <>
inline const std::shared_ptr<ossia::curve_abstract>*
behavior_variant_type::target() const
{
  if(m_type == Type0)
    return &m_impl.m_value0;
  return nullptr;
}
template <>
inline const std::vector<ossia::behavior>* behavior_variant_type::target() const
{
  if(m_type == Type1)
    return &m_impl.m_value1;
  return nullptr;
}
template <>
inline std::shared_ptr<ossia::curve_abstract>* behavior_variant_type::target()
{
  if(m_type == Type0)
    return &m_impl.m_value0;
  return nullptr;
}
template <>
inline std::vector<ossia::behavior>* behavior_variant_type::target()
{
  if(m_type == Type1)
    return &m_impl.m_value1;
  return nullptr;
}
template <>
inline const std::shared_ptr<ossia::curve_abstract>& behavior_variant_type::get() const
{
  if(m_type == Type0)
    return m_impl.m_value0;
  throw std::runtime_error("behavior_variant: bad type");
}
template <>
inline const std::vector<ossia::behavior>& behavior_variant_type::get() const
{
  if(m_type == Type1)
    return m_impl.m_value1;
  throw std::runtime_error("behavior_variant: bad type");
}
template <>
inline std::shared_ptr<ossia::curve_abstract>& behavior_variant_type::get()
{
  if(m_type == Type0)
    return m_impl.m_value0;
  throw std::runtime_error("behavior_variant: bad type");
}
template <>
inline std::vector<ossia::behavior>& behavior_variant_type::get()
{
  if(m_type == Type1)
    return m_impl.m_value1;
  throw std::runtime_error("behavior_variant: bad type");
}
template <typename Visitor>
auto apply_nonnull(Visitor&& functor, const behavior_variant_type& var)
{
  switch(var.m_type)
  {
    case behavior_variant_type::Type::Type0:
      return functor(var.m_impl.m_value0);
    case behavior_variant_type::Type::Type1:
      return functor(var.m_impl.m_value1);
    default:
      throw std::runtime_error("behavior_variant: bad type");
  }
}
template <typename Visitor>
auto apply_nonnull(Visitor&& functor, behavior_variant_type& var)
{
  switch(var.m_type)
  {
    case behavior_variant_type::Type::Type0:
      return functor(var.m_impl.m_value0);
    case behavior_variant_type::Type::Type1:
      return functor(var.m_impl.m_value1);
    default:
      throw std::runtime_error("behavior_variant: bad type");
  }
}
template <typename Visitor>
auto apply_nonnull(Visitor&& functor, behavior_variant_type&& var)
{
  switch(var.m_type)
  {
    case behavior_variant_type::Type::Type0:
      return functor(std::move(var.m_impl.m_value0));
    case behavior_variant_type::Type::Type1:
      return functor(std::move(var.m_impl.m_value1));
    default:
      throw std::runtime_error("behavior_variant: bad type");
  }
}
template <typename Visitor>
auto apply(Visitor&& functor, const behavior_variant_type& var)
{
  switch(var.m_type)
  {
    case behavior_variant_type::Type::Type0:
      return functor(var.m_impl.m_value0);
    case behavior_variant_type::Type::Type1:
      return functor(var.m_impl.m_value1);
    default:
      return functor();
  }
}
template <typename Visitor>
auto apply(Visitor&& functor, behavior_variant_type& var)
{
  switch(var.m_type)
  {
    case behavior_variant_type::Type::Type0:
      return functor(var.m_impl.m_value0);
    case behavior_variant_type::Type::Type1:
      return functor(var.m_impl.m_value1);
    default:
      return functor();
  }
}
template <typename Visitor>
auto apply(Visitor&& functor, behavior_variant_type&& var)
{
  switch(var.m_type)
  {
    case behavior_variant_type::Type::Type0:
      return functor(std::move(var.m_impl.m_value0));
    case behavior_variant_type::Type::Type1:
      return functor(std::move(var.m_impl.m_value1));
    default:
      return functor();
  }
}

inline void behavior_variant_type::destruct_impl()
{
  switch(m_type)
  {
    case Type::Type0:
      m_impl.m_value0.~shared_ptr();
      break;
    case Type::Type1:
      m_impl.m_value1.~vector();
      break;
    default:
      break;
  }
}

inline int behavior_variant_type::which() const
{
  return m_type;
}

inline behavior_variant_type::operator bool() const
{
  return m_type != npos;
}

inline behavior_variant_type::behavior_variant_type()
    : m_type{Npos}
{
}

inline behavior_variant_type::~behavior_variant_type()
{
  destruct_impl();
}

inline behavior_variant_type::behavior_variant_type(
    const std::shared_ptr<ossia::curve_abstract>& v)
    : m_type{Type0}
{
  new(&m_impl.m_value0) std::shared_ptr<ossia::curve_abstract>{v};
}

inline behavior_variant_type::behavior_variant_type(
    std::shared_ptr<ossia::curve_abstract>&& v)
    : m_type{Type0}
{
  new(&m_impl.m_value0) std::shared_ptr<ossia::curve_abstract>{std::move(v)};
}

inline behavior_variant_type::behavior_variant_type(
    const std::vector<ossia::behavior>& v)
    : m_type{Type1}
{
  new(&m_impl.m_value1) std::vector<ossia::behavior>{v};
}

inline behavior_variant_type::behavior_variant_type(std::vector<ossia::behavior>&& v)
    : m_type{Type1}
{
  new(&m_impl.m_value1) std::vector<ossia::behavior>{std::move(v)};
}

inline behavior_variant_type::behavior_variant_type(const behavior_variant_type& other)
    : m_type{other.m_type}
{
  switch(m_type)
  {
    case Type::Type0:
      new(&m_impl.m_value0)
          std::shared_ptr<ossia::curve_abstract>{other.m_impl.m_value0};
      break;
    case Type::Type1:
      new(&m_impl.m_value1) std::vector<ossia::behavior>{other.m_impl.m_value1};
      break;
    default:
      break;
  }
}

inline behavior_variant_type::behavior_variant_type(
    behavior_variant_type&& other) noexcept
    : m_type{other.m_type}
{
  switch(m_type)
  {
    case Type::Type0:
      new(&m_impl.m_value0)
          std::shared_ptr<ossia::curve_abstract>{std::move(other.m_impl.m_value0)};
      break;
    case Type::Type1:
      new(&m_impl.m_value1)
          std::vector<ossia::behavior>{std::move(other.m_impl.m_value1)};
      break;
    default:
      break;
  }
}

inline behavior_variant_type&
behavior_variant_type::operator=(const behavior_variant_type& other)
{
  destruct_impl();
  m_type = other.m_type;
  switch(m_type)
  {
    case Type::Type0:
      new(&m_impl.m_value0)
          std::shared_ptr<ossia::curve_abstract>{other.m_impl.m_value0};
      break;
    case Type::Type1:
      new(&m_impl.m_value1) std::vector<ossia::behavior>{other.m_impl.m_value1};
      break;
    default:
      break;
  }
  return *this;
}

inline behavior_variant_type&
behavior_variant_type::operator=(behavior_variant_type&& other) noexcept
{
  destruct_impl();
  m_type = other.m_type;
  switch(m_type)
  {
    case Type::Type0:
      new(&m_impl.m_value0)
          std::shared_ptr<ossia::curve_abstract>{std::move(other.m_impl.m_value0)};
      break;
    case Type::Type1:
      new(&m_impl.m_value1)
          std::vector<ossia::behavior>{std::move(other.m_impl.m_value1)};
      break;
    default:
      break;
  }
  return *this;
}
