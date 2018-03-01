#pragma once
#include <ossia-qt/score/qml_autom.hpp>
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
    Q_GADGET
    Q_PROPERTY(qml_device* device READ device WRITE setDevice)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QVariant value READ value WRITE setValue)

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
};
class qml_utils: public QObject
{
    Q_OBJECT

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
    Q_OBJECT
    Q_PROPERTY(QQmlScriptString script READ script WRITE setScript NOTIFY scriptChanged)
    Q_PROPERTY(qint32 date READ date NOTIFY dateChanged)
    Q_PROPERTY(double position READ position NOTIFY positionChanged)
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
  Q_SIGNALS:
    void dateChanged(qint32);
    void positionChanged(double);
    void scriptChanged(QQmlScriptString);

  private:
    void reset_impl() override;
    std::shared_ptr<qml_script_process> m_impl;

    QQmlScriptString m_script;
    time_value m_date{};
    double m_pos{};
};
}
}

Q_DECLARE_METATYPE(ossia::qt::qml_message)
Q_DECLARE_METATYPE(ossia::qt::qml_device*)
