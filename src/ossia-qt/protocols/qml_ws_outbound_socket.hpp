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
  struct state
  {
    std::string url;
    std::unique_ptr<ossia::net::websocket_client> client;
    std::atomic_bool alive{true};
  };

  qml_websocket_outbound_socket(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
      : m_state{std::make_shared<state>()}
  {
    m_state->url = "ws://" + conf.host + ":" + std::to_string(conf.port); // FIXME wss
    auto st = m_state;
    auto self = QPointer{this};
    m_state->client = std::make_unique<ossia::net::websocket_client>(
        ctx, [st, self](auto hdl, auto opcode, const std::string& msg) {
      if(!st->alive)
        return;
      if(auto* ptr = self.get())
        ptr->on_message(hdl, opcode, msg);
    });

    if(onOpen.isCallable())
      m_state->client->on_open.connect<&qml_websocket_outbound_socket::on_open>(this);
    if(onClose.isCallable())
      m_state->client->on_close.connect<&qml_websocket_outbound_socket::on_close>(this);
    if(onError.isCallable())
      m_state->client->on_fail.connect<&qml_websocket_outbound_socket::on_fail>(this);
  }

  ~qml_websocket_outbound_socket() { m_state->alive = false; }

  inline boost::asio::io_context& context() noexcept
  {
    return m_state->client->context();
  }

  void open() { m_state->client->connect(m_state->url); }

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
    if(!m_state->alive)
      return;
    ossia::qt::run_async(
        this, [=, this] { onOpen.call({qjsEngine(this)->newQObject(this)}); },
        Qt::AutoConnection);
  }
  void on_fail()
  {
    if(!m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onError.call(); }, Qt::AutoConnection);
  }
  void on_close()
  {
    if(!m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  void write(QString message)
  {
    auto st = m_state;
    boost::asio::dispatch(
        st->client->context(),
        [st, msg = message.toStdString()] {
      if(st->alive)
        st->client->send_message(msg);
    });
  }
  W_SLOT(write)

  void writeBinary(QByteArray buffer)
  {
    auto st = m_state;
    boost::asio::dispatch(
        st->client->context(),
        [st, buf = std::string(buffer.data(), buffer.size())] {
      if(st->alive)
        st->client->send_binary_message(buf);
    });
  }
  W_SLOT(writeBinary)

  void close()
  {
    m_state->client->stop();
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
  std::shared_ptr<state> m_state;
};

}
