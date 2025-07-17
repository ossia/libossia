#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/websocket_client.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{
class qml_websocket_outbound_socket
    : public QObject
    , public Nano::Observer
    , public protocols_sender
{
  W_OBJECT(qml_websocket_outbound_socket)
public:
  qml_websocket_outbound_socket(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
      : m_url{"ws://" + conf.host + ":" + std::to_string(conf.port)} // FIXME wss
      , m_client{ctx, [this](auto&&... args) { on_message(args...); }}
  {
    if(onOpen.isCallable())
      m_client.on_open.connect<&qml_websocket_outbound_socket::on_open>(this);
    if(onClose.isCallable())
      m_client.on_close.connect<&qml_websocket_outbound_socket::on_close>(this);
    if(onError.isCallable())
      m_client.on_fail.connect<&qml_websocket_outbound_socket::on_fail>(this);
  }
  inline boost::asio::io_context& context() noexcept { return m_client.context(); }

  void open() { m_client.connect(m_url); }

  void on_message(auto hdl, auto opcode, const std::string& msg)
  {
    if(opcode == websocketpp::frame::opcode::text && onTextMessage.isCallable())
    {
      onTextMessage.call({QString::fromStdString(msg)});
    }
    else if(opcode == websocketpp::frame::opcode::binary && onBinaryMessage.isCallable())
    {
      onBinaryMessage.call(
          {qjsEngine(this)->toScriptValue(QByteArray(msg.data(), msg.size()))});
    }
  }

  void on_open()
  {
    run_on_qt_thread({ onOpen.call({qjsEngine(this)->newQObject(this)}); });
  }
  void on_fail()
  {
    run_on_qt_thread({ onError.call(); });
  }
  void on_close()
  {
    run_on_qt_thread({ onClose.call(); });
  }

  void write(QString message)
  {
    run_on_asio_thread({ m_client.send_message(message.toStdString()); });
  }
  W_SLOT(write)

  void writeBinary(QByteArray buffer)
  {
    run_on_asio_thread({
      m_client.send_binary_message(std::string_view(buffer.data(), buffer.size()));
    });
  }
  W_SLOT(writeBinary)

  void close()
  {
    m_client.stop();
    if(onClose.isCallable())
      onClose.call();
  }
  W_SLOT(close)

  // FIXME
  // void osc(QByteArray address, QJSValueList values) { this->send_osc(address, values); }
  // W_SLOT(osc)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onTextMessage;
  QJSValue onBinaryMessage;

private:
  std::string m_url;
  ossia::net::websocket_client m_client;
};

}
