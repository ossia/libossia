#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/json.hpp>
#include <ossia/detail/logger.hpp>

#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include <nano_signal_slot.hpp>

namespace ossia::net
{

//! Low-level Websocket client
class websocket_client
{
public:
  using connection_handler = websocketpp::connection_hdl;
  Nano::Signal<void()> on_open;
  Nano::Signal<void()> on_close;
  Nano::Signal<void()> on_fail;

  websocket_client()
      : m_open{false}
  {
    init_client();
  }

  void init_client()
  {
    m_client = std::make_shared<client_t>();
    assert(m_client);
    std::weak_ptr<client_t> weak_client = m_client;
    m_client->clear_access_channels(websocketpp::log::alevel::all);
    m_client->clear_error_channels(websocketpp::log::elevel::all);

    m_client->set_open_handler([this, weak_client](connection_handler hdl) {
      if(!weak_client.lock())
        return;
      scoped_lock guard(m_lock);
      m_open = true;

      on_open();
    });

    m_client->set_close_handler([this, weak_client](connection_handler hdl) {
      if(!weak_client.lock())
        return;
      {
        scoped_lock guard(m_lock);
        m_open = false;
      }
      on_close();
    });

    m_client->set_fail_handler([this, weak_client](connection_handler hdl) {
      if(!weak_client.lock())
        return;
      {
        scoped_lock guard(m_lock);
        m_open = false;
      }
      on_fail();
    });
    assert(m_client);
  }

  //! \tparam Function that will be called when the client receives a server
  //! message.
  template <typename MessageHandler>
  websocket_client(MessageHandler&& onMessage)
      : websocket_client{}
  {
    assert(m_client);
    m_client->init_asio();

    std::weak_ptr<client_t> weak_client = m_client;
    m_client->set_message_handler(
        [handler = std::move(onMessage),
         weak_client](connection_handler hdl, client_t::message_ptr msg) {
      if(!weak_client.lock())
        return;
      handler(hdl, msg->get_opcode(), msg->get_raw_payload());
    });
    assert(m_client);
  }

  template <typename MessageHandler>
  websocket_client(boost::asio::io_context& ctx, MessageHandler&& onMessage)
      : websocket_client{}
  {
    assert(m_client);
    m_client->init_asio(&ctx);
    m_ctx = &ctx;

    std::weak_ptr<client_t> weak_client = m_client;
    m_client->set_message_handler(
        [handler = std::move(onMessage),
         weak_client](connection_handler hdl, client_t::message_ptr msg) {
      if(!weak_client.lock())
        return;
      handler(hdl, msg->get_opcode(), msg->get_raw_payload());
    });
    assert(m_client);
  }

  ~websocket_client()
  {
    if(m_open)
      stop();
  }

  bool connected() const { return m_open; }

  void stop()
  {
    if(!m_open)
    {
      if(m_client)
        m_client->stop();
      m_connected = false;
      return;
    }

    scoped_lock guard(m_lock);
    m_client->close(m_hdl, websocketpp::close::status::normal, "");
    m_client->stop();
    m_open = false;
  }

  auto& client() { return m_client; }
  auto& handle() { return m_hdl; }
  bool after_connect() { return m_connected; }

  void connect(const std::string& uri)
  {
    websocketpp::lib::error_code ec;
    if(!m_client)
    {
      init_client();
      assert(m_client);
      if(m_ctx)
        m_client->init_asio(m_ctx);
      else
        m_client->init_asio();
    }

    auto con = m_client->get_connection(uri, ec);
    if(ec)
    {
      m_client->get_alog().write(
          websocketpp::log::alevel::app, "Get Connection Error: " + ec.message());
      return;
    }

    m_hdl = con->get_handle();
    m_client->connect(con);
    m_connected = true;
  }

  void finish_connection()
  {
    m_connected = false;
    m_client.reset(); // In order to be able to reconnect afterwards.
  }

  // This function returns if the connection is stopped / fails.
  void connect_and_run(const std::string& uri)
  {
    connect(uri);

    m_client->run();

    finish_connection();
  }

  void send_message(const std::string& request)
  {
    if(!m_open || !m_client)
      return;

    websocketpp::lib::error_code ec;

    m_client->send(m_hdl, request, websocketpp::frame::opcode::text, ec);

    if(ec)
    {
      m_client->get_alog().write(
          websocketpp::log::alevel::app, "Send Error: " + ec.message());
    }
  }

  void send_message(const rapidjson::StringBuffer& request)
  {
    if(!m_open || !m_client)
      return;

    websocketpp::lib::error_code ec;

    m_client->send(
        m_hdl, request.GetString(), request.GetSize(), websocketpp::frame::opcode::text,
        ec);

    if(ec)
    {
      m_client->get_alog().write(
          websocketpp::log::alevel::app, "Send Error: " + ec.message());
    }
  }

  void send_binary_message(std::string_view request)
  {
    if(!m_open || !m_client)
      return;

    websocketpp::lib::error_code ec;

    m_client->send(
        m_hdl, request.data(), request.size(), websocketpp::frame::opcode::binary, ec);

    if(ec)
    {
      m_client->get_alog().write(
          websocketpp::log::alevel::app, "Send Error: " + ec.message());
    }
  }

  boost::asio::io_context& context() noexcept { return *m_ctx; }

protected:
  using client_t = websocketpp::client<websocketpp::config::asio_client>;
  using scoped_lock = websocketpp::lib::lock_guard<websocketpp::lib::mutex>;

  boost::asio::io_context* m_ctx{};
  std::shared_ptr<client_t> m_client;
  connection_handler m_hdl;
  websocketpp::lib::mutex m_lock;
  std::atomic_bool m_open{false};
  std::atomic_bool m_connected{false};
};
}
