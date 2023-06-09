
template <>
inline const float* value_variant_type::target() const
{
  if(m_type == Type0)
    return &m_impl.m_value0;
  return nullptr;
}
template <>
inline const int32_t* value_variant_type::target() const
{
  if(m_type == Type1)
    return &m_impl.m_value1;
  return nullptr;
}
template <>
inline const ossia::vec2f* value_variant_type::target() const
{
  if(m_type == Type2)
    return &m_impl.m_value2;
  return nullptr;
}
template <>
inline const ossia::vec3f* value_variant_type::target() const
{
  if(m_type == Type3)
    return &m_impl.m_value3;
  return nullptr;
}
template <>
inline const ossia::vec4f* value_variant_type::target() const
{
  if(m_type == Type4)
    return &m_impl.m_value4;
  return nullptr;
}
template <>
inline const ossia::impulse* value_variant_type::target() const
{
  if(m_type == Type5)
    return &m_impl.m_value5;
  return nullptr;
}
template <>
inline const bool* value_variant_type::target() const
{
  if(m_type == Type6)
    return &m_impl.m_value6;
  return nullptr;
}
template <>
inline const std::string* value_variant_type::target() const
{
  if(m_type == Type7)
    return &m_impl.m_value7;
  return nullptr;
}
template <>
inline const std::vector<ossia::value>* value_variant_type::target() const
{
  if(m_type == Type8)
    return &m_impl.m_value8;
  return nullptr;
}
template <>
inline const value_map_type* value_variant_type::target() const
{
  if(m_type == Type9)
    return &m_impl.m_value9;
  return nullptr;
}
template <>
inline float* value_variant_type::target()
{
  if(m_type == Type0)
    return &m_impl.m_value0;
  return nullptr;
}
template <>
inline int32_t* value_variant_type::target()
{
  if(m_type == Type1)
    return &m_impl.m_value1;
  return nullptr;
}
template <>
inline ossia::vec2f* value_variant_type::target()
{
  if(m_type == Type2)
    return &m_impl.m_value2;
  return nullptr;
}
template <>
inline ossia::vec3f* value_variant_type::target()
{
  if(m_type == Type3)
    return &m_impl.m_value3;
  return nullptr;
}
template <>
inline ossia::vec4f* value_variant_type::target()
{
  if(m_type == Type4)
    return &m_impl.m_value4;
  return nullptr;
}
template <>
inline ossia::impulse* value_variant_type::target()
{
  if(m_type == Type5)
    return &m_impl.m_value5;
  return nullptr;
}
template <>
inline bool* value_variant_type::target()
{
  if(m_type == Type6)
    return &m_impl.m_value6;
  return nullptr;
}
template <>
inline std::string* value_variant_type::target()
{
  if(m_type == Type7)
    return &m_impl.m_value7;
  return nullptr;
}
template <>
inline std::vector<ossia::value>* value_variant_type::target()
{
  if(m_type == Type8)
    return &m_impl.m_value8;
  return nullptr;
}
template <>
inline value_map_type* value_variant_type::target()
{
  if(m_type == Type9)
    return &m_impl.m_value9;
  return nullptr;
}
template <>
inline const float& value_variant_type::get() const
{
  if(m_type == Type0)
    return m_impl.m_value0;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const int32_t& value_variant_type::get() const
{
  if(m_type == Type1)
    return m_impl.m_value1;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const ossia::vec2f& value_variant_type::get() const
{
  if(m_type == Type2)
    return m_impl.m_value2;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const ossia::vec3f& value_variant_type::get() const
{
  if(m_type == Type3)
    return m_impl.m_value3;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const ossia::vec4f& value_variant_type::get() const
{
  if(m_type == Type4)
    return m_impl.m_value4;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const ossia::impulse& value_variant_type::get() const
{
  if(m_type == Type5)
    return m_impl.m_value5;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const bool& value_variant_type::get() const
{
  if(m_type == Type6)
    return m_impl.m_value6;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const std::string& value_variant_type::get() const
{
  if(m_type == Type7)
    return m_impl.m_value7;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const std::vector<ossia::value>& value_variant_type::get() const
{
  if(m_type == Type8)
    return m_impl.m_value8;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline const value_map_type& value_variant_type::get() const
{
  if(m_type == Type9)
    return m_impl.m_value9;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline float& value_variant_type::get()
{
  if(m_type == Type0)
    return m_impl.m_value0;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline int32_t& value_variant_type::get()
{
  if(m_type == Type1)
    return m_impl.m_value1;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline ossia::vec2f& value_variant_type::get()
{
  if(m_type == Type2)
    return m_impl.m_value2;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline ossia::vec3f& value_variant_type::get()
{
  if(m_type == Type3)
    return m_impl.m_value3;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline ossia::vec4f& value_variant_type::get()
{
  if(m_type == Type4)
    return m_impl.m_value4;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline ossia::impulse& value_variant_type::get()
{
  if(m_type == Type5)
    return m_impl.m_value5;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline bool& value_variant_type::get()
{
  if(m_type == Type6)
    return m_impl.m_value6;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline std::string& value_variant_type::get()
{
  if(m_type == Type7)
    return m_impl.m_value7;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline std::vector<ossia::value>& value_variant_type::get()
{
  if(m_type == Type8)
    return m_impl.m_value8;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <>
inline value_map_type& value_variant_type::get()
{
  if(m_type == Type9)
    return m_impl.m_value9;
  ossia_do_throw(std::runtime_error, "value_variant: bad type");
}
template <typename Visitor>
auto apply_nonnull(Visitor&& functor, const value_variant_type& var)
{
  switch(var.m_type)
  {
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
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Visitor>
auto apply_nonnull(Visitor&& functor, value_variant_type& var)
{
  switch(var.m_type)
  {
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
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Visitor>
auto apply_nonnull(Visitor&& functor, value_variant_type&& var)
{
  switch(var.m_type)
  {
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
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Visitor>
auto apply(Visitor&& functor, const value_variant_type& var)
{
  switch(var.m_type)
  {
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
    default:
      return functor();
  }
}
template <typename Visitor>
auto apply(Visitor&& functor, value_variant_type& var)
{
  switch(var.m_type)
  {
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
    default:
      return functor();
  }
}
template <typename Visitor>
auto apply(Visitor&& functor, value_variant_type&& var)
{
  switch(var.m_type)
  {
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
    default:
      return functor();
  }
}
template <typename Functor>
auto apply(Functor&& functor, value_variant_type& arg0, const value_variant_type& arg1)
{
  switch(arg0.m_type)
  {
    case value_variant_type::Type::Type0: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type1: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type2: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type3: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type4: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type5: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type6: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type7: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type8: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type9: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Functor>
auto apply(Functor&& functor, const value_variant_type& arg0, value_variant_type& arg1)
{
  switch(arg0.m_type)
  {
    case value_variant_type::Type::Type0: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type1: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type2: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type3: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type4: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type5: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type6: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type7: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type8: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type9: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Functor>
auto apply(
    Functor&& functor, const value_variant_type& arg0, const value_variant_type& arg1)
{
  switch(arg0.m_type)
  {
    case value_variant_type::Type::Type0: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type1: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type2: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type3: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type4: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type5: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type6: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type7: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type8: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type9: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Functor>
auto apply(Functor&& functor, value_variant_type&& arg0, value_variant_type&& arg1)
{
  switch(arg0.m_type)
  {
    case value_variant_type::Type::Type0: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value0), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type1: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value1), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type2: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value2), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type3: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value3), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type4: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value4), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type5: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value5), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type6: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value6), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type7: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value7), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type8: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value8), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type9: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value0));
        }
        case value_variant_type::Type::Type1: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value1));
        }
        case value_variant_type::Type::Type2: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value2));
        }
        case value_variant_type::Type::Type3: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value3));
        }
        case value_variant_type::Type::Type4: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value4));
        }
        case value_variant_type::Type::Type5: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value5));
        }
        case value_variant_type::Type::Type6: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value6));
        }
        case value_variant_type::Type::Type7: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value7));
        }
        case value_variant_type::Type::Type8: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value8));
        }
        case value_variant_type::Type::Type9: {
          return functor(
              std::move(arg0.m_impl.m_value9), std::move(arg1.m_impl.m_value9));
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Functor>
auto apply(Functor&& functor, value_variant_type& arg0, value_variant_type& arg1)
{
  switch(arg0.m_type)
  {
    case value_variant_type::Type::Type0: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value0, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type1: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value1, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type2: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value2, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type3: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value3, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type4: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value4, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type5: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value5, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type6: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value6, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type7: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value7, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type8: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value8, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type9: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value0);
        }
        case value_variant_type::Type::Type1: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value1);
        }
        case value_variant_type::Type::Type2: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value2);
        }
        case value_variant_type::Type::Type3: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value3);
        }
        case value_variant_type::Type::Type4: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value4);
        }
        case value_variant_type::Type::Type5: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value5);
        }
        case value_variant_type::Type::Type6: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value6);
        }
        case value_variant_type::Type::Type7: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value7);
        }
        case value_variant_type::Type::Type8: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value8);
        }
        case value_variant_type::Type::Type9: {
          return functor(arg0.m_impl.m_value9, arg1.m_impl.m_value9);
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Functor>
auto apply(
    Functor&& functor, const value_variant_type& arg0, const value_variant_type& arg1,
    const value_variant_type& arg2)
{
  switch(arg0.m_type)
  {
    case value_variant_type::Type::Type0: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value0, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type1: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value1, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type2: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value2, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type3: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value3, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type4: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value4, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type5: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value5, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type6: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value6, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type7: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value7, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type8: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value8, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type9: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value0, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value1, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value2, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value3, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value4, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value5, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value6, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value7, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value8, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  arg0.m_impl.m_value9, arg1.m_impl.m_value9, arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}
template <typename Functor>
auto apply(
    Functor&& functor, value_variant_type&& arg0, const value_variant_type& arg1,
    const value_variant_type& arg2)
{
  switch(arg0.m_type)
  {
    case value_variant_type::Type::Type0: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value0), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type1: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value1), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type2: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value2), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type3: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value3), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type4: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value4), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type5: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value5), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type6: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value6), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type7: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value7), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type8: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value8), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    case value_variant_type::Type::Type9: {
      switch(arg1.m_type)
      {
        case value_variant_type::Type::Type0: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value0,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type1: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value1,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type2: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value2,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type3: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value3,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type4: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value4,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type5: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value5,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type6: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value6,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type7: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value7,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type8: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value8,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        case value_variant_type::Type::Type9: {
          switch(arg2.m_type)
          {
            case value_variant_type::Type::Type0: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value0);
            }
            case value_variant_type::Type::Type1: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value1);
            }
            case value_variant_type::Type::Type2: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value2);
            }
            case value_variant_type::Type::Type3: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value3);
            }
            case value_variant_type::Type::Type4: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value4);
            }
            case value_variant_type::Type::Type5: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value5);
            }
            case value_variant_type::Type::Type6: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value6);
            }
            case value_variant_type::Type::Type7: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value7);
            }
            case value_variant_type::Type::Type8: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value8);
            }
            case value_variant_type::Type::Type9: {
              return functor(
                  std::move(arg0.m_impl.m_value9), arg1.m_impl.m_value9,
                  arg2.m_impl.m_value9);
            }
            default:
              ossia_do_throw(std::runtime_error, "value_variant: bad type");
          }
        }
        default:
          ossia_do_throw(std::runtime_error, "value_variant: bad type");
      }
    }
    default:
      ossia_do_throw(std::runtime_error, "value_variant: bad type");
  }
}

inline void value_variant_type::destruct_impl()
{
  switch(m_type)
  {
    case Type::Type7:
      m_impl.m_value7.~basic_string();
      break;
    case Type::Type8:
      m_impl.m_value8.~vector<ossia::value>();
      break;
    case Type::Type9:
      m_impl.m_value9.~value_map_type();
      break;
    default:
      break;
  }
}

inline int value_variant_type::which() const
{
  return m_type;
}

inline value_variant_type::operator bool() const
{
  return m_type != npos;
}

inline value_variant_type::value_variant_type()
    : m_type{Npos}
{
}

inline value_variant_type::~value_variant_type()
{
  destruct_impl();
}

inline value_variant_type::value_variant_type(float v)
    : m_type{Type0}
{
  new(&m_impl.m_value0) float{v};
}

inline value_variant_type::value_variant_type(int32_t v)
    : m_type{Type1}
{
  new(&m_impl.m_value1) int32_t{v};
}

inline value_variant_type::value_variant_type(ossia::vec2f v)
    : m_type{Type2}
{
  new(&m_impl.m_value2) ossia::vec2f{v};
}

inline value_variant_type::value_variant_type(ossia::vec3f v)
    : m_type{Type3}
{
  new(&m_impl.m_value3) ossia::vec3f{v};
}

inline value_variant_type::value_variant_type(ossia::vec4f v)
    : m_type{Type4}
{
  new(&m_impl.m_value4) ossia::vec4f{v};
}

inline value_variant_type::value_variant_type(ossia::impulse v)
    : m_type{Type5}
{
  new(&m_impl.m_value5) ossia::impulse{v};
}

inline value_variant_type::value_variant_type(bool v)
    : m_type{Type6}
{
  new(&m_impl.m_value6) bool{v};
}

inline value_variant_type::value_variant_type(const std::string& v)
    : m_type{Type7}
{
  new(&m_impl.m_value7) std::string{v};
}

inline value_variant_type::value_variant_type(std::string&& v)
    : m_type{Type7}
{
  new(&m_impl.m_value7) std::string{std::move(v)};
}

inline value_variant_type::value_variant_type(const std::vector<ossia::value>& v)
    : m_type{Type8}
{
  new(&m_impl.m_value8) std::vector<ossia::value>(v);
}

inline value_variant_type::value_variant_type(std::vector<ossia::value>&& v) noexcept
    : m_type{Type8}
{
  new(&m_impl.m_value8) std::vector<ossia::value>(std::move(v));
}

inline value_variant_type::value_variant_type(const value_map_type& v)
    : m_type{Type9}
{
  new(&m_impl.m_value9) value_map_type(v);
}

inline value_variant_type::value_variant_type(value_map_type&& v)
    : m_type{Type9}
{
  new(&m_impl.m_value9) value_map_type(std::move(v));
}

inline value_variant_type::value_variant_type(const value_variant_type& other)
    : m_type{other.m_type}
{
  switch(m_type)
  {
    case Type::Type0:
      new(&m_impl.m_value0) float{other.m_impl.m_value0};
      break;
    case Type::Type1:
      new(&m_impl.m_value1) int32_t{other.m_impl.m_value1};
      break;
    case Type::Type2:
      new(&m_impl.m_value2) ossia::vec2f{other.m_impl.m_value2};
      break;
    case Type::Type3:
      new(&m_impl.m_value3) ossia::vec3f{other.m_impl.m_value3};
      break;
    case Type::Type4:
      new(&m_impl.m_value4) ossia::vec4f{other.m_impl.m_value4};
      break;
    case Type::Type5:
      new(&m_impl.m_value5) ossia::impulse{other.m_impl.m_value5};
      break;
    case Type::Type6:
      new(&m_impl.m_value6) bool{other.m_impl.m_value6};
      break;
    case Type::Type7:
      new(&m_impl.m_value7) std::string{other.m_impl.m_value7};
      break;
    case Type::Type8:
      new(&m_impl.m_value8) std::vector<ossia::value>(other.m_impl.m_value8);
      break;
    case Type::Type9:
      new(&m_impl.m_value9) value_map_type(other.m_impl.m_value9);
      break;
    default:
      break;
  }
}

inline value_variant_type::value_variant_type(value_variant_type&& other) noexcept
    : m_type{other.m_type}
{
  switch(m_type)
  {
    case Type::Type0:
      new(&m_impl.m_value0) float{std::move(other.m_impl.m_value0)};
      break;
    case Type::Type1:
      new(&m_impl.m_value1) int32_t{std::move(other.m_impl.m_value1)};
      break;
    case Type::Type2:
      new(&m_impl.m_value2) ossia::vec2f{std::move(other.m_impl.m_value2)};
      break;
    case Type::Type3:
      new(&m_impl.m_value3) ossia::vec3f{std::move(other.m_impl.m_value3)};
      break;
    case Type::Type4:
      new(&m_impl.m_value4) ossia::vec4f{std::move(other.m_impl.m_value4)};
      break;
    case Type::Type5:
      new(&m_impl.m_value5) ossia::impulse{std::move(other.m_impl.m_value5)};
      break;
    case Type::Type6:
      new(&m_impl.m_value6) bool{std::move(other.m_impl.m_value6)};
      break;
    case Type::Type7:
      new(&m_impl.m_value7) std::string{std::move(other.m_impl.m_value7)};
      break;
    case Type::Type8:
      new(&m_impl.m_value8) std::vector<ossia::value>(std::move(other.m_impl.m_value8));
      break;
    case Type::Type9:
      new(&m_impl.m_value9) value_map_type{std::move(other.m_impl.m_value9)};
      break;
    default:
      break;
  }
}

inline value_variant_type& value_variant_type::operator=(const value_variant_type& other)
{
  if(m_type != other.m_type)
  {
    destruct_impl();
    m_type = other.m_type;
    switch(m_type)
    {
      case Type::Type0:
        new(&m_impl.m_value0) float{other.m_impl.m_value0};
        break;
      case Type::Type1:
        new(&m_impl.m_value1) int32_t{other.m_impl.m_value1};
        break;
      case Type::Type2:
        new(&m_impl.m_value2) ossia::vec2f{other.m_impl.m_value2};
        break;
      case Type::Type3:
        new(&m_impl.m_value3) ossia::vec3f{other.m_impl.m_value3};
        break;
      case Type::Type4:
        new(&m_impl.m_value4) ossia::vec4f{other.m_impl.m_value4};
        break;
      case Type::Type5:
        new(&m_impl.m_value5) ossia::impulse{other.m_impl.m_value5};
        break;
      case Type::Type6:
        new(&m_impl.m_value6) bool{other.m_impl.m_value6};
        break;
      case Type::Type7:
        new(&m_impl.m_value7) std::string{other.m_impl.m_value7};
        break;
      case Type::Type8:
        new(&m_impl.m_value8) std::vector<ossia::value>(other.m_impl.m_value8);
        break;
      case Type::Type9:
        new(&m_impl.m_value9) value_map_type{other.m_impl.m_value9};
        break;
      default:
        break;
    }
  }
  else
  {
    switch(m_type)
    {
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
      default:
        break;
    }
  }
  return *this;
}

inline value_variant_type&
value_variant_type::operator=(value_variant_type&& other) noexcept
{
  if(m_type != other.m_type)
  {
    destruct_impl();
    m_type = other.m_type;
    switch(m_type)
    {
      case Type::Type0:
        new(&m_impl.m_value0) float{std::move(other.m_impl.m_value0)};
        break;
      case Type::Type1:
        new(&m_impl.m_value1) int32_t{std::move(other.m_impl.m_value1)};
        break;
      case Type::Type2:
        new(&m_impl.m_value2) ossia::vec2f{std::move(other.m_impl.m_value2)};
        break;
      case Type::Type3:
        new(&m_impl.m_value3) ossia::vec3f{std::move(other.m_impl.m_value3)};
        break;
      case Type::Type4:
        new(&m_impl.m_value4) ossia::vec4f{std::move(other.m_impl.m_value4)};
        break;
      case Type::Type5:
        new(&m_impl.m_value5) ossia::impulse{std::move(other.m_impl.m_value5)};
        break;
      case Type::Type6:
        new(&m_impl.m_value6) bool{std::move(other.m_impl.m_value6)};
        break;
      case Type::Type7:
        new(&m_impl.m_value7) std::string{std::move(other.m_impl.m_value7)};
        break;
      case Type::Type8:
        new(&m_impl.m_value8)
            std::vector<ossia::value>(std::move(other.m_impl.m_value8));
        break;
      case Type::Type9:
        new(&m_impl.m_value9) value_map_type{std::move(other.m_impl.m_value9)};
        break;
      default:
        break;
    }
  }
  else
  {
    switch(m_type)
    {
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
        m_impl.m_value7 = std::move(other.m_impl.m_value7);
        break;
      case Type::Type8:
        m_impl.m_value8 = std::move(other.m_impl.m_value8);
        break;
      case Type::Type9:
        m_impl.m_value9 = other.m_impl.m_value9;
        break;
      default:
        break;
    }
  }
  return *this;
}
