#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/websocket_server.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <verdigris>

#include <vector>

namespace ossia::qt
{
//! Shared between the server object and its connections: the websocketpp server
//! lives on the mapper's asio context, the QJSValue callbacks on the Qt thread.
//! The asio handlers keep a strong reference to this state so that the server
//! stays valid until websocketpp is done with it.
struct websocket_server_state
{
  boost::asio::io_context& context;
  std::shared_ptr<ossia::net::websocket_server> server;
  std::atomic_bool alive{true};

  explicit websocket_server_state(const ossia::net::network_context_ptr& ctx)
      : context{ctx->context}
      , server{std::make_shared<ossia::net::websocket_server>(ctx)}
  {
  }
};

class qml_websocket_connection
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_websocket_connection)
public:
  using handler = ossia::net::websocket_server::connection_handler;

  qml_websocket_connection(std::shared_ptr<websocket_server_state> st, handler hdl)
      : m_state{std::move(st)}
      , m_hdl{std::move(hdl)}
  {
  }

  bool isOpen() const noexcept { return m_state && !m_hdl.expired(); }

  const handler& handle() const noexcept { return m_hdl; }

  void write(QString message)
  {
    if(!m_state)
      return;
    auto st = m_state;
    boost::asio::dispatch(st->context, [st, hdl = m_hdl, msg = message.toStdString()] {
      if(!st->alive)
        return;
      try
      {
        st->server->send_message(hdl, msg);
      }
      catch(...)
      {
      }
    });
  }
  W_SLOT(write)

  void writeBinary(QByteArray buffer)
  {
    if(!m_state)
      return;
    auto st = m_state;
    boost::asio::dispatch(
        st->context,
        [st, hdl = m_hdl, msg = std::string(buffer.data(), buffer.size())] {
      if(!st->alive)
        return;
      try
      {
        st->server->send_binary_message(hdl, msg);
      }
      catch(...)
      {
      }
    });
  }
  W_SLOT(writeBinary)

  void close()
  {
    if(!m_state)
      return;
    auto st = m_state;
    boost::asio::dispatch(st->context, [st, hdl = m_hdl] {
      if(!st->alive)
        return;
      try
      {
        st->server->close(hdl);
      }
      catch(...)
      {
      }
    });
  }
  W_SLOT(close)

  //! Always called on the Qt thread by the server object.
  void on_message(
      websocketpp::frame::opcode::value opcode, const std::string& payload) const
  {
    if(opcode == websocketpp::frame::opcode::text && onTextMessage.isCallable())
      onTextMessage.call({QString::fromUtf8(payload.data(), payload.size())});
    else if(opcode == websocketpp::frame::opcode::binary && onBinaryMessage.isCallable())
    {
      if(auto engine = qjsEngine(this))
        onBinaryMessage.call(
            {engine->toScriptValue(QByteArray(payload.data(), payload.size()))});
    }

    if(onBytes.isCallable())
    {
      if(auto engine = qjsEngine(this))
        onBytes.call({engine->toScriptValue(QByteArray(payload.data(), payload.size()))});
    }
  }

  QJSValue onBytes;
  W_PROPERTY(QJSValue, onBytes W_MEMBER onBytes);
  QJSValue onTextMessage;
  W_PROPERTY(QJSValue, onTextMessage W_MEMBER onTextMessage);
  QJSValue onBinaryMessage;
  W_PROPERTY(QJSValue, onBinaryMessage W_MEMBER onBinaryMessage);
  QJSValue onClose;
  W_PROPERTY(QJSValue, onClose W_MEMBER onClose);

private:
  std::shared_ptr<websocket_server_state> m_state;
  handler m_hdl;
};

class qml_websocket_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_websocket_inbound_socket)
public:
  using state = websocket_server_state;
  using handler = ossia::net::websocket_server::connection_handler;

  qml_websocket_inbound_socket() { }

  ~qml_websocket_inbound_socket() { shutdown(); }

  bool isOpen() const noexcept { return m_state && m_state->alive; }

  void open(
      const ossia::net::inbound_socket_configuration& conf,
      const ossia::net::network_context_ptr& ctx)
  {
    auto st = std::make_shared<state>(ctx);
    auto self = QPointer{this};

    st->server->set_open_handler([st, self](handler hdl) {
      if(!st->alive)
        return;
      if(auto* ptr = self.get())
        ossia::qt::run_async(
            ptr,
            [self, st, hdl] {
          if(auto* ptr = self.get())
            ptr->on_connection(st, hdl);
        },
            Qt::AutoConnection);
    });

    st->server->set_close_handler([st, self](handler hdl) {
      if(!st->alive)
        return;
      if(auto* ptr = self.get())
        ossia::qt::run_async(
            ptr,
            [self, hdl] {
          if(auto* ptr = self.get())
            ptr->on_disconnection(hdl);
        },
            Qt::AutoConnection);
    });

    st->server->set_message_handler(
        [st, self](
            handler hdl, websocketpp::frame::opcode::value opcode,
            const std::string& payload) -> ossia::net::server_reply {
      if(!st->alive)
        return {};
      if(auto* ptr = self.get())
        ossia::qt::run_async(
            ptr,
            [self, hdl, opcode, msg = payload] {
          if(auto* ptr = self.get())
          {
            if(auto* conn = ptr->find_connection(hdl))
              conn->on_message(opcode, msg);
          }
        },
            Qt::AutoConnection);
      return {};
    });

    m_state = std::move(st);
    m_state->server->listen(conf.port);

    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    if(!m_state)
      return;
    shutdown();
    if(onClose.isCallable())
      onClose.call();
  }
  W_SLOT(close)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onConnection;

private:
  //! Releases the listener and every client so that the port is immediately
  //! rebindable. Does not touch the QJSValue callbacks: also used at destruction.
  void shutdown()
  {
    if(!m_state)
      return;
    auto st = std::move(m_state);
    st->alive = false;

    std::vector<handler> handles;
    handles.reserve(m_connections.size());
    for(auto* conn : m_connections)
    {
      handles.push_back(conn->handle());
      // A script may reach here from inside one of this connection's own
      // callbacks (server.close() called from onTextMessage): deleting it
      // synchronously would destroy the object whose method is on the stack.
      conn->deleteLater();
    }
    m_connections.clear();

    // Closing the clients and the listener walks websocketpp state that the
    // asio thread is concurrently using to re-arm async_accept, so it has to
    // run on the context like every other operation on the server.
    boost::asio::dispatch(st->context, [st, handles = std::move(handles)] {
      for(const auto& hdl : handles)
      {
        try
        {
          st->server->close(hdl);
        }
        catch(...)
        {
        }
      }

      try
      {
        st->server->stop();
      }
      catch(...)
      {
      }
    });
  }

  void on_connection(const std::shared_ptr<state>& st, const handler& hdl)
  {
    if(!m_state || m_state != st)
      return;

    auto conn = new qml_websocket_connection{st, hdl};
    // Parent to the server so that Qt uses CppOwnership (prevents QML GC)
    conn->setParent(this);
    m_connections.push_back(conn);

    if(onConnection.isCallable())
      onConnection.call({qjsEngine(this)->newQObject(conn)});
  }

  void on_disconnection(const handler& hdl)
  {
    for(auto it = m_connections.begin(); it != m_connections.end(); ++it)
    {
      auto* conn = *it;
      if(!same(conn->handle(), hdl))
        continue;

      m_connections.erase(it);
      if(conn->onClose.isCallable())
        conn->onClose.call();
      conn->deleteLater();
      return;
    }
  }

  qml_websocket_connection* find_connection(const handler& hdl) const noexcept
  {
    for(auto* conn : m_connections)
      if(same(conn->handle(), hdl))
        return conn;
    return nullptr;
  }

  static bool same(const handler& lhs, const handler& rhs) noexcept
  {
    return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
  }

  std::shared_ptr<state> m_state;
  std::vector<qml_websocket_connection*> m_connections;
};
}
