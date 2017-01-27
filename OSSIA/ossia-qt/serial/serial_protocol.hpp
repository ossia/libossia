#pragma once
#include <QSerialPort>
#include <ossia/network/base/protocol.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <ossia/network/serial/serial_address_data.hpp>

#include <QObject>

class QQmlEngine;
class QQmlComponent;

namespace ossia
{
namespace net
{
class serial_wrapper:
    public QObject
{
  Q_OBJECT

  QSerialPort mSerialPort;
public:
  serial_wrapper(const QSerialPortInfo& port):
    mSerialPort{port}
  {
    mSerialPort.open(QIODevice::WriteOnly);
    connect(this, &serial_wrapper::write,
            this, &serial_wrapper::on_write,
            Qt::QueuedConnection);
  }

signals:
  void write(QByteArray);

public slots:
  void on_write(QByteArray b)
  {
    mSerialPort.write(b);
  }
};

class serial_device;
class serial_protocol final :
    public QObject,
    public ossia::net::protocol_base
{
public:
  // Param : the name of the serial port
  serial_protocol(const QByteArray& code, const QSerialPortInfo& bot);
  virtual ~serial_protocol();

  bool pull(ossia::net::address_base&) override;
  bool push(const ossia::net::address_base&) override;
  bool observe(ossia::net::address_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  static serial_address_data read_data(const QJSValue& js) { return js; }

private:
  QQmlEngine *mEngine{};
  QQmlComponent* mComponent{};

  QByteArray mCode;
  serial_device* mDevice{};
  mutable serial_wrapper mSerialPort;
};



}
}
