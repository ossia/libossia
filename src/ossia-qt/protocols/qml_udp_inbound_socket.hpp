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

// State extracted so both qml_udp_sender and qml_udp_inbound_socket can reference it
struct qml_udp_inbound_state
{
  ossia::net::udp_receive_socket socket;
  std::atomic_bool alive{true};

  qml_udp_inbound_state(
      const ossia::net::inbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
      : socket{conf, ctx}
  {
  }
};

class qml_udp_inbound_socket;

// Cached sender object — reused across onMessage calls (no per-message allocation).
// Updated with the sender's endpoint before each onMessage dispatch.
class qml_udp_sender : public QObject
{
  W_OBJECT(qml_udp_sender)
public:
  explicit qml_udp_sender(QObject* parent = nullptr)
      : QObject(parent)
  {
  }

  QString host() const { return m_host; }
  W_PROPERTY(QString, host READ host)

  int port() const { return m_port; }
  W_PROPERTY(int, port READ port)

  void reply(QByteArray data)
  {
    if(!m_state || !m_state->alive)
      return;
    auto st = m_state;
    auto ep = m_endpoint;
    boost::asio::dispatch(st->socket.m_context, [st, ep, data] {
      if(st->alive)
      {
        boost::system::error_code ec;
        st->socket.m_socket.send_to(
            boost::asio::const_buffer(data.data(), data.size()), ep, 0, ec);
      }
    });
  }
  W_SLOT(reply)

private:
  friend class qml_udp_inbound_socket;
  boost::asio::ip::udp::endpoint m_endpoint;
  std::shared_ptr<qml_udp_inbound_state> m_state;
  QString m_host;
  int m_port = 0;
};

class qml_udp_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_udp_inbound_socket)
public:
  using state = qml_udp_inbound_state;

  qml_udp_inbound_socket()
  {
    QQmlEngine::setObjectOwnership(&m_sender, QQmlEngine::CppOwnership);
  }

  ~qml_udp_inbound_socket()
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
    m_state = std::make_shared<state>(conf, ctx);

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
      // Capture sender endpoint on asio thread (valid right now,
      // m_endpoint was populated by async_receive_from)
      auto sender_ep = st->socket.m_endpoint;
      ossia::qt::run_async(
          self.get(),
          [self, st, arg = QByteArray(data, sz), sender_ep] {
        if(!self.get())
          return;

        // Update cached sender object (Qt thread, no race)
        self->m_sender.m_endpoint = sender_ep;
        self->m_sender.m_state = st;
        self->m_sender.m_host
            = QString::fromStdString(sender_ep.address().to_string());
        self->m_sender.m_port = sender_ep.port();

        if(self->onMessage.isCallable())
        {
          auto engine = qjsEngine(self.get());
          if(engine)
            self->onMessage.call(
                {engine->toScriptValue(arg), engine->newQObject(&self->m_sender)});
        }
      },
          Qt::AutoConnection);
    });
  }

  void on_close()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
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

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;

private:
  std::shared_ptr<state> m_state;
  qml_udp_sender m_sender{this};
};

}
