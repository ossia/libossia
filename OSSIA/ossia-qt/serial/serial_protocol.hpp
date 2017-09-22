#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/network/base/protocol.hpp>
#include <QObject>
#include <QSerialPort>
#include <ossia-qt/js_utilities.hpp>
#include <ossia-qt/serial/serial_parameter_data.hpp>

class QQmlEngine;
class QQmlComponent;

namespace ossia
{
namespace net
{
class OSSIA_EXPORT serial_wrapper : public QObject
{
  Q_OBJECT

  QSerialPort mSerialPort;

public:
  serial_wrapper(const QSerialPortInfo& port) : mSerialPort{port}
  {
    mSerialPort.open(QIODevice::ReadWrite);
    ossia::logger().info(
        "Opened serial port: {}", mSerialPort.errorString().toStdString());
    connect(
        this, &serial_wrapper::write, this, &serial_wrapper::on_write,
        Qt::QueuedConnection);

    connect(
        &mSerialPort, &QSerialPort::readyRead, this, &serial_wrapper::on_read,
        Qt::QueuedConnection);
  }
  ~serial_wrapper();

signals:
  void write(QByteArray);

public slots:
  void on_write(QByteArray b)
  {
    mSerialPort.write(b);
  }
  void on_read()
  {
    qDebug() << mSerialPort.readAll();
  }
};


class serial_device;
class OSSIA_EXPORT serial_protocol final : public QObject, public ossia::net::protocol_base
{
public:
  // Param : the name of the serial port
  serial_protocol(const QByteArray& code, const QSerialPortInfo& bot);
  virtual ~serial_protocol();

  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base&) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  void set_device(ossia::net::device_base& dev) override;

  static serial_parameter_data read_data(const QJSValue& js)
  {
    return js;
  }

private:
  QQmlEngine* mEngine{};
  QQmlComponent* mComponent{};

  serial_device* mDevice{};
  mutable serial_wrapper mSerialPort;
  QByteArray mCode;
};
}
}
