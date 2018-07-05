#pragma once
#include <ossia-qt/score/qml_autom.hpp>
#include <wobjectdefs.h>
#include <ossia-qt/device/qml_device.hpp>
#include <QQmlScriptString>
namespace ossia
{
namespace qt
{
class qml_script_process : public ossia::time_process
{
  public:
    void state(
        ossia::time_value date, double pos,
        ossia::time_value tick_offset, double gspeed) override;

    QQmlExpression* expr{};
};

class qml_message
{
    W_OBJECT(qml_message)
    
    
    

  public:
    qml_message() = default;
    qml_message(const qml_message&) = default;
    qml_message(qml_message&&) = default;
    qml_message& operator=(const qml_message&) = default;
    qml_message& operator=(qml_message&&) = default;
    qml_message(qml_device* d, QString addr, QVariant v)
    : m_dev{d}
    , m_addr{addr}
    , m_value{v}
    {

    }
    qml_device* device() const { return m_dev; }
    void setDevice(qml_device* d) { m_dev = d; }

    QString address() const { return m_addr; }
    void setAddress(QString d) { m_addr = d; }

    QVariant value() const { return m_value; }
    void setValue(QVariant d) { m_value = d; }

    ossia::state_element toState() const;
  private:
    qml_device* m_dev{};
    QString m_addr;
    QVariant m_value;

W_PROPERTY(QVariant, value READ value WRITE setValue)

W_PROPERTY(QString, address READ address WRITE setAddress)

W_PROPERTY(qml_device*, device READ device WRITE setDevice)
};
class qml_utils: public QObject
{
    W_OBJECT(qml_utils)

  public:
    using QObject::QObject;
    Q_INVOKABLE
    ossia::qt::qml_message message(qml_device* dev, QString addr, QVariant val)
    {
      return qml_message{dev, addr, val};
    }

};
class qml_script : public qml_process
{
    W_OBJECT(qml_script)
    
    
    
  public:
    qml_script(QQuickItem* parent = nullptr);
    ~qml_script() override;

    void setup() override;
    std::shared_ptr<ossia::time_process> process() const override;

    void setDate(time_value date) { m_date = date; }
    void setPosition(double pos) { m_pos = pos; }

    int32_t date() const { return m_date.impl; }
    double position() const { return m_pos; }
    QQmlScriptString script() const { return m_script; }
    void setScript(QQmlScriptString s) { m_script = s; }
  public:
    void dateChanged(qint32 arg_1) E_SIGNAL(OSSIA_EXPORT, dateChanged, arg_1);
    void positionChanged(double arg_1) E_SIGNAL(OSSIA_EXPORT, positionChanged, arg_1);
    void scriptChanged(QQmlScriptString arg_1) E_SIGNAL(OSSIA_EXPORT, scriptChanged, arg_1);

  private:
    void reset_impl() override;
    std::shared_ptr<qml_script_process> m_impl;

    QQmlScriptString m_script;
    time_value m_date{};
    double m_pos{};

W_PROPERTY(double, position READ position NOTIFY positionChanged)

W_PROPERTY(qint32, date READ date NOTIFY dateChanged)

W_PROPERTY(QQmlScriptString, script READ script WRITE setScript NOTIFY scriptChanged)
};
}
}

Q_DECLARE_METATYPE(ossia::qt::qml_message)
Q_DECLARE_METATYPE(ossia::qt::qml_device*)
