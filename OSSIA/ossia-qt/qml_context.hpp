#pragma once
#include <QObject>
#include <QUrl>
#include <QFile>
#include <QQmlEngine>
#include <QDebug>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-c/preset/preset.hpp>

namespace ossia
{
namespace qt
{
class qml_device;
/**
 * @brief The qml_context struct
 *
 * This class is used to allow QML scripts to access common enums
 * in Ossia.
 */
class qml_context : public QObject
{
  Q_OBJECT

public:
  // QML enums have to begin with a capital
  virtual ~qml_context();
  enum val_type
  {
    Float, //! \see float
    Int, //! \see int32_t
    Vec2f, //! \see ossia::vec2f
    Vec3f, //! \see ossia::vec3f
    Vec4f, //! \see ossia::vec4f
    Impulse, //! \see ossia::impulse
    Bool, //! \see bool
    String, //! \see std::string
    Tuple, //! \see std::vector<ossia::value>
    Char, //! \see char
    Destination //! \see ossia::Destination
  };

  enum access_mode
  {
    Get,
    Set,
    Bi
  };

  enum bounding_mode
  {
    Free,
    Clip,
    Wrap,
    Fold,
    Low,
    High
  };

  enum repetition_filter
  {
    On,
    Off
  };

  Q_ENUM(val_type)
  Q_ENUM(access_mode)
  Q_ENUM(bounding_mode)
  Q_ENUM(repetition_filter)

public slots:
  void logTrace(const QString& s);
  void logInfo(const QString& s);
  void logDebug(const QString& s);
  void logWarning(const QString& s);
  void logError(const QString& s);
  void logCritical(const QString& s);
};
}
}

Q_DECLARE_METATYPE(ossia::qt::qml_context::val_type)
Q_DECLARE_METATYPE(ossia::qt::qml_context::access_mode)
Q_DECLARE_METATYPE(ossia::qt::qml_context::bounding_mode)
Q_DECLARE_METATYPE(ossia::qt::qml_context::repetition_filter)
