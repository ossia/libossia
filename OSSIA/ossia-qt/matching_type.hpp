#pragma once
#include <ossia/network/value/value.hpp>
#include <QString>
#include <QChar>

namespace ossia
{
template <typename>
struct qt_property_converter;

template <>
struct qt_property_converter<float>
{
  static constexpr const auto val = ossia::val_type::FLOAT;
  using type = float;
  static auto convert(float f)
  {
    return f;
  }
};
template <>
struct qt_property_converter<double>
{
  static constexpr const auto val = ossia::val_type::FLOAT;
  using type = float;
  static auto convert(double f)
  {
    return f;
  }
};
template <>
struct qt_property_converter<int>
{
  static constexpr const auto val = ossia::val_type::INT;
  using type = int32_t;
  static auto convert(int f)
  {
    return f;
  }
};
template <>
struct qt_property_converter<bool>
{
  static constexpr const auto val = ossia::val_type::BOOL;
  using type = bool;
  static auto convert(bool f)
  {
    return f;
  }
};
template <>
struct qt_property_converter<ossia::impulse>
{
  static constexpr const auto val = ossia::val_type::IMPULSE;
  using type = ossia::impulse;
  static auto convert(ossia::impulse)
  {
    return ossia::impulse{};
  }
};
template <>
struct qt_property_converter<std::string>
{
  static constexpr const auto val = ossia::val_type::STRING;
  using type = std::string;
  static auto convert(const std::string& f)
  {
    return f;
  }
  static auto convert(std::string&& f)
  {
    return std::move(f);
  }
};

template <>
struct qt_property_converter<QString>
{
  static constexpr const auto val = ossia::val_type::STRING;
  using type = std::string;
  static auto convert(const QString& f)
  {
    return f.toStdString();
  }
};

template <>
struct qt_property_converter<char>
{
  static constexpr const auto val = ossia::val_type::CHAR;
  using type = char;
  static auto convert(char f)
  {
    return f;
  }
};

template <>
struct qt_property_converter<QChar>
{
  static constexpr const auto val = ossia::val_type::CHAR;
  using type = char;
  static auto convert(QChar f)
  {
    return f.toLatin1();
  }
};

template <>
struct qt_property_converter<ossia::vec2f>
{
  static constexpr const auto val = ossia::val_type::VEC2F;
  using type = ossia::vec2f;
  static auto convert(const ossia::vec2f& t)
  {
    return t;
  }
};

template <>
struct qt_property_converter<ossia::vec3f>
{
  static constexpr const auto val = ossia::val_type::VEC3F;
  using type = ossia::vec3f;
  static auto convert(const ossia::vec3f& t)
  {
    return t;
  }
};

template <>
struct qt_property_converter<ossia::vec4f>
{
  static constexpr const auto val = ossia::val_type::VEC4F;
  using type = ossia::vec4f;
  static auto convert(const ossia::vec4f& t)
  {
    return t;
  }
};

template <>
struct qt_property_converter<std::vector<ossia::value>>
{
  static constexpr const auto val = ossia::val_type::LIST;
  using type = std::vector<ossia::value>;
  static auto convert(const std::vector<ossia::value>& t)
  {
    return t;
  }
  static auto convert(std::vector<ossia::value>&& t)
  {
    return std::move(t);
  }
};
}
