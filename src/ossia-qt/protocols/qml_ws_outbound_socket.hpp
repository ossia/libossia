#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/websocket_client.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <mutex>

#include <verdigris>

namespace ossia::qt
{
class qml_websocket_outbound_socket
    : public QObject
    , public protocols_sender
{
  W_OBJECT(qml_websocket_outbound_socket)
public:
  //! Owns the transport and every websocketpp-side callback.
  //!
  //! The Nano::Observer lives here and not on the QObject on purpose: the
  //! signals it listens to belong to `client`, a member of this very object, so
  //! the unsynchronized slot lists can only be mutated while the whole state is
  //! being destroyed, at which point no handler can still be running. The
  //! QObject, destroyed on the Qt thread while the websocketpp thread may be
  //! firing, is only ever reached under qt_mutex.
  struct state : Nano::Observer
  {
    std::string url;
    std::unique_ptr<ossia::net::websocket_client> client;
    std::atomic_bool alive{true};
    //! The script's onClose is reported at most once, be it a peer-initiated
    //! close or an explicit one.
    std::atomic_bool closed{false};

    //! Guards `self`, which is cleared before the QObject starts dying. Held
    //! for the whole asio -> Qt handover.
    std::mutex qt_mutex;
    qml_websocket_outbound_socket* self{};

    void detach()
    {
      std::lock_guard g{qt_mutex};
      self = nullptr;
    }

    //! Nano slots, always on the websocketpp thread.
    void fire_open();
    void fire_fail();
    void fire_close();

    //! Message delivery, always on the websocketpp thread.
    void fire_message(websocketpp::frame::opcode::value opcode, std::string msg);
  };

  qml_websocket_outbound_socket() { }

  ~qml_websocket_outbound_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      // No websocketpp -> Qt call may start from here on. shutdown() then only
      // touches the transport: the QQmlEngine that owns our QJSValues is being
      // destroyed, so calling into the script here would be a use-after-free.
      m_state->detach();
      shutdown();
    }
  }

  bool isOpen() const noexcept { return m_state && m_state->client; }

  void open(
      const ossia::net::outbound_socket_configuration& conf,
      boost::asio::io_context& ctx)
  {
    m_state = std::make_shared<state>();
    m_state->url = "ws://" + conf.host + ":" + std::to_string(conf.port); // FIXME wss
    m_state->self = this;
    auto st = m_state;
    m_state->client = std::make_unique<ossia::net::websocket_client>(
        ctx, [st](auto hdl, auto opcode, const std::string& msg) {
      if(!st->alive)
        return;
      st->fire_message(opcode, msg);
    });

    if(onOpen.isCallable())
      m_state->client->on_open.connect<&state::fire_open>(m_state.get());
    if(onClose.isCallable())
      m_state->client->on_close.connect<&state::fire_close>(m_state.get());
    if(onError.isCallable())
      m_state->client->on_fail.connect<&state::fire_fail>(m_state.get());

    m_state->client->connect(m_state->url);
  }

  //! Always called on the Qt thread, by state::fire_message.
  void deliver_message(
      websocketpp::frame::opcode::value opcode, const std::string& msg)
  {
    if(opcode == websocketpp::frame::opcode::text && onTextMessage.isCallable())
    {
      onTextMessage.call({QString::fromStdString(msg)});
    }
    else if(opcode == websocketpp::frame::opcode::binary && onBinaryMessage.isCallable())
    {
      if(auto engine = qjsEngine(this))
        onBinaryMessage.call({engine->toScriptValue(QByteArray(msg.data(), msg.size()))});
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
    if(m_state->closed.exchange(true))
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  void write(QString message)
  {
    if(!m_state)
      return;
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
    if(!m_state)
      return;
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
    if(!m_state)
      return;
    shutdown();
    // onClose reaches the script exactly once, whichever side closed first.
    if(!m_state->closed.exchange(true) && onClose.isCallable())
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
  //! Stops the transport only. Touches no QJSValue, so it is also usable from
  //! the destructor, when the QQmlEngine is already being torn down.
  void shutdown()
  {
    if(m_state && m_state->client)
      m_state->client->stop();
  }

  std::shared_ptr<state> m_state;
};

inline void qml_websocket_outbound_socket::state::fire_open()
{
  std::lock_guard g{qt_mutex};
  if(self)
    self->on_open();
}

inline void qml_websocket_outbound_socket::state::fire_fail()
{
  std::lock_guard g{qt_mutex};
  if(self)
    self->on_fail();
}

inline void qml_websocket_outbound_socket::state::fire_close()
{
  std::lock_guard g{qt_mutex};
  if(self)
    self->on_close();
}

inline void qml_websocket_outbound_socket::state::fire_message(
    websocketpp::frame::opcode::value opcode, std::string msg)
{
  // The QJSValue callbacks must run on the Qt thread: nothing but plain data is
  // touched here.
  std::lock_guard g{qt_mutex};
  if(!self)
    return;
  ossia::qt::run_async(
      self,
      [target = self, opcode, msg = std::move(msg)] {
    target->deliver_message(opcode, msg);
  },
      Qt::AutoConnection);
}

}
