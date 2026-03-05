#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/tcp_socket.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{
class qml_tcp_outbound_socket
    : public QObject
    , public Nano::Observer
    , public protocols_sender
{
  W_OBJECT(qml_tcp_outbound_socket)
public:
  struct state
  {
    ossia::net::tcp_client socket;
    std::atomic_bool alive{true};

    state(
        const ossia::net::outbound_socket_configuration& conf,
        boost::asio::io_context& ctx)
        : socket{conf, ctx}
    {
    }
  };

private:
  std::shared_ptr<state> m_state;

public:
  ossia::net::tcp_client& socket;

  qml_tcp_outbound_socket(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
      : m_state{std::make_shared<state>(conf, ctx)}
      , socket{m_state->socket}
  {
  }

  ~qml_tcp_outbound_socket() { m_state->alive = false; }

  inline boost::asio::io_context& context() noexcept { return m_state->socket.m_context; }

  void open()
  {
    try
    {
      if(onOpen.isCallable())
        m_state->socket.on_open.connect<&qml_tcp_outbound_socket::on_open>(this);
      if(onClose.isCallable())
        m_state->socket.on_close.connect<&qml_tcp_outbound_socket::on_close>(this);
      if(onError.isCallable())
        m_state->socket.on_fail.connect<&qml_tcp_outbound_socket::on_fail>(this);
      m_state->socket.connect();
    }
    catch(const std::exception& e)
    {
      if(onError.isCallable())
      {
        onError.call({QString::fromStdString(e.what())});
      }
    }
  }

  void write(QByteArray buffer)
  {
    auto st = m_state;
    boost::asio::dispatch(st->socket.m_context, [st, buffer] {
      if(st->alive)
        st->socket.write(buffer.data(), buffer.size());
    });
  }
  W_SLOT(write)

  void close() { m_state->socket.close(); }
  W_SLOT(close)

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

  void osc(QByteArray address, QJSValueList values) { this->send_osc(address, values); }
  W_SLOT(osc)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onBytes;
};

}
