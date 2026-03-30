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

class qml_udp_outbound_socket
    : public QObject
    , public Nano::Observer
    , public protocols_sender
{
  W_OBJECT(qml_udp_outbound_socket)
public:
  struct state
  {
    ossia::net::udp_send_socket socket;
    std::atomic_bool alive{true};

    state(
        const ossia::net::outbound_socket_configuration& conf,
        boost::asio::io_context& ctx)
        : socket{conf, ctx}
    {
    }
  };

  ossia::net::udp_send_socket* socket = nullptr;

  qml_udp_outbound_socket() { }

  ~qml_udp_outbound_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      close();
    }
  }

  bool isOpen() const noexcept { return m_state != nullptr; }

  void open(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
  {
    m_state = std::make_shared<state>(conf, ctx);
    socket = &m_state->socket;

    if(onClose.isCallable())
      m_state->socket.on_close.connect<&qml_udp_outbound_socket::on_close>(*this);

    m_state->socket.connect();

    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    if(!m_state)
      return;
    if(!m_state->socket.m_socket.is_open())
      return;
    auto st = m_state;
    boost::asio::post(st->socket.m_context, [st] {
      try
      {
        st->socket.m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
      }
      catch(...)
      {
      }
      st->socket.m_socket.close();
      st->socket.on_close();
    });
  }
  W_SLOT(close)

  void on_close()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  void write(QByteArray buffer)
  {
    if(!m_state)
      return;
    auto st = m_state;
    boost::asio::dispatch(st->socket.m_context, [st, buffer = std::move(buffer)] {
      if(st->alive)
        st->socket.write(buffer.data(), buffer.size());
    });
  }
  W_SLOT(write)

  void osc(QByteArray address, QJSValueList values)
  {
    if(socket)
      this->send_osc(address, values);
  }
  W_SLOT(osc)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;

private:
  std::shared_ptr<state> m_state;
};
}
