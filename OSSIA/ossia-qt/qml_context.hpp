#pragma once
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QQmlEngine>
#include <QUrl>
#include <ossia/preset/preset.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/score/qml_util.hpp>
namespace spdlog
{
class logger;
}
namespace ossia
{
namespace qt
{
class qml_device;
class qml_val_type
{
  Q_GADGET
public:
  enum val_type
  {
    Float,   //! \see float
    Int,     //! \see int32_t
    Vec2f,   //! \see ossia::vec2f
    Vec3f,   //! \see ossia::vec3f
    Vec4f,   //! \see ossia::vec4f
    Impulse, //! \see ossia::impulse
    Bool,    //! \see bool
    String,  //! \see std::string
    List,   //! \see std::vector<ossia::value>
    Char     //! \see char
  };
  Q_ENUM(val_type)
};

class qml_access_mode
{
  Q_GADGET
public:
  enum access_mode
  {
    Bi,
    Get,
    Set
  };
  Q_ENUM(access_mode)
};

class qml_bounding_mode
{
  Q_GADGET
public:
  enum bounding_mode
  {
    Free,
    Clip,
    Wrap,
    Fold,
    Low,
    High
  };
  Q_ENUM(bounding_mode)
};

class qml_rep_filter
{
  Q_GADGET
public:
  enum repetition_filter
  {
    Unfiltered,
    Filtered
  };

  Q_ENUM(repetition_filter)
};

class qml_duration
{
  Q_GADGET
public:
  enum duration: qint32
  {
    Infinite = ossia::qt::infinite()
  };

  Q_ENUM(duration)
};
}
}

Q_DECLARE_METATYPE(ossia::qt::qml_val_type::val_type)
Q_DECLARE_METATYPE(ossia::qt::qml_access_mode::access_mode)
Q_DECLARE_METATYPE(ossia::qt::qml_bounding_mode::bounding_mode)
Q_DECLARE_METATYPE(ossia::qt::qml_rep_filter::repetition_filter)
Q_DECLARE_METATYPE(ossia::qt::qml_duration::duration)

W_REGISTER_ARGTYPE(ossia::qt::qml_val_type::val_type)
W_REGISTER_ARGTYPE(ossia::qt::qml_access_mode::access_mode)
W_REGISTER_ARGTYPE(ossia::qt::qml_bounding_mode::bounding_mode)
W_REGISTER_ARGTYPE(ossia::qt::qml_rep_filter::repetition_filter)
W_REGISTER_ARGTYPE(ossia::qt::qml_duration::duration)
