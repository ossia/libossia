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

  qml_websocket_inbound_socket() { }

  ~qml_websocket_inbound_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      close();
    }
  }

  bool isOpen() const noexcept { return m_state != nullptr; }

  void open(
      const ossia::net::inbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
  {
    m_state = std::make_shared<state>(ctx);

    // FIXME
    // WebSocket server implementation simplified for now
    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    if(!m_state)
      return;
    if(onClose.isCallable())
      onClose.call();
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
