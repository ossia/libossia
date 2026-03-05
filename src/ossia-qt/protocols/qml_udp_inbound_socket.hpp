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
class qml_udp_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_udp_inbound_socket)
public:
  struct state
  {
    ossia::net::udp_receive_socket socket;
    std::atomic_bool alive{true};

    state(
        const ossia::net::inbound_socket_configuration& conf,
        boost::asio::io_context& ctx)
        : socket{conf, ctx}
    {
    }
  };

  qml_udp_inbound_socket(
      const ossia::net::inbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : m_state{std::make_shared<state>(conf, ctx)}
  {
  }

  ~qml_udp_inbound_socket() { m_state->alive = false; }

  inline boost::asio::io_context& context() noexcept { return m_state->socket.m_context; }

  void open()
  {
    if(onClose.isCallable())
      m_state->socket.on_close.connect<&qml_udp_inbound_socket::on_close>(*this);

    m_state->socket.open();
    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});

    auto st = m_state;
    auto self = QPointer{this};
    st->socket.receive([st, self](const char* data, std::size_t sz) {
      if(!st->alive)
        return;
      ossia::qt::run_async(
          self.get(),
          [self, arg = QByteArray(data, sz)] {
        if(!self.get())
          return;
        if(self->onMessage.isCallable())
        {
          self->onMessage.call({qjsEngine(self.get())->toScriptValue(arg)});
        }
      },
          Qt::AutoConnection);
    });
  }

  void on_close()
  {
    if(!m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  void close() { m_state->socket.close(); }
  W_SLOT(close)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;

private:
  std::shared_ptr<state> m_state;
};

}
