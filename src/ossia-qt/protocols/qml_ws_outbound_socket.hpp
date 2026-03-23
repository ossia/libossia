#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/websocket_client_beast.hpp>
#include <ossia/network/sockets/websocket_common.hpp>

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
    std::unique_ptr<ossia::net::websocket_client_beast> client;
    std::atomic_bool alive{true};
  };

  qml_websocket_outbound_socket() { }

  ~qml_websocket_outbound_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      close();
    }
  }

  bool isOpen() const noexcept { return m_state && m_state->client; }

  void open(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
  {
    m_state = std::make_shared<state>();
    m_state->url = "ws://" + conf.host + ":" + std::to_string(conf.port); // FIXME wss
    auto st = m_state;
    auto self = QPointer{this};
    m_state->client = std::make_unique<ossia::net::websocket_client_beast>(
        ctx, ossia::net::ws_client_message_handler{
            [st, self](const ossia::net::ws_connection_handle&, ossia::net::ws_opcode opcode, std::string& msg) {
      if(!st->alive)
        return;
      if(auto* ptr = self.get())
        ptr->on_message(opcode, msg);
    }});

    if(onOpen.isCallable())
      m_state->client->on_open.connect<&qml_websocket_outbound_socket::on_open>(this);
    if(onClose.isCallable())
      m_state->client->on_close.connect<&qml_websocket_outbound_socket::on_close>(this);
    if(onError.isCallable())
      m_state->client->on_fail.connect<&qml_websocket_outbound_socket::on_fail>(this);

    m_state->client->connect(m_state->url);
  }
  ~qml_websocket_outbound_socket() { m_state->alive = false; }
  ~qml_websocket_outbound_socket() { m_state->alive = false; }

  void open() { m_state->client->connect(m_state->url); }
  void open() { m_state->client->connect(m_state->url); }

  inline boost::asio::io_context& context() noexcept
  {
    return m_state->client->context();
  }
  
  void on_message(ossia::net::ws_opcode opcode, const std::string& msg)
  {
    if(opcode == ossia::net::ws_opcode::text && onTextMessage.isCallable())
    {
      onTextMessage.call({QString::fromStdString(msg)});
    }
    else if(opcode == ossia::net::ws_opcode::binary && onBinaryMessage.isCallable())
    {
      onBinaryMessage.call(
          {qjsEngine(this)->toScriptValue(QByteArray(msg.data(), msg.size()))});
    }
  }

  void on_open()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(
        this, [=, this] { onOpen.call({qjsEngine(this)->newQObject(this)}); },
        Qt::AutoConnection);
  }
  void on_fail()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onError.call(); }, Qt::AutoConnection);
  }
  void on_close()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  void write(QString message)
  {
    if(!m_state)
      return;
    auto st = m_state;
    if(st->alive)
      st->client->send_message(message.toStdString());
  }
  W_SLOT(write)

  void writeBinary(QByteArray buffer)
  {
    if(!m_state)
      return;
    auto st = m_state;
    if(st->alive)
      st->client->send_binary_message(std::string_view(buffer.data(), buffer.size()));
  }
  W_SLOT(writeBinary)

  void close()
  {
    if(!m_state)
      return;
    m_state->client->stop();
    if(onClose.isCallable())
      onClose.call();
  }
  W_SLOT(close)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onTextMessage;
  QJSValue onBinaryMessage;

private:
  std::shared_ptr<state> m_state;
};

}
