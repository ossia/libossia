#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/udp_socket.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{
class qml_websocket_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_websocket_inbound_socket)
public:
  struct state
  {
    boost::asio::io_context& context;
    std::atomic_bool alive{true};

    state(boost::asio::io_context& ctx)
        : context{ctx}
    {
    }
  };

  qml_websocket_inbound_socket(
      const ossia::net::inbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : m_state{std::make_shared<state>(ctx)}
  {
    // FIXME
    // WebSocket server implementation simplified for now
  }

  ~qml_websocket_inbound_socket() { m_state->alive = false; }

  inline boost::asio::io_context& context() noexcept { return m_state->context; }

  void open()
  {
    if(onOpen.isCallable())

      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    if(!m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }
  W_SLOT(close)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onConnection;
  QJSValue onBytes;
  QJSValue onTextMessage;
  QJSValue onBinaryMessage;

private:
  std::shared_ptr<state> m_state;
};
}
