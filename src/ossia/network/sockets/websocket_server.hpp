#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/json.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/sockets/websocket_reply.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/http/request.hpp>
#include <websocketpp/server.hpp>
#if defined(OSSIA_BENCHMARK)
#include <chrono>
#endif
namespace ossia::net
{

//! Low-level websocket & http server for oscquery
class websocket_server
{
public:
  using config = websocketpp::config::asio;
  using transport_type = websocketpp::config::asio::transport_type;
  using server_t = websocketpp::server<websocketpp::config::asio>;
  using type = server_t;
  using connection_handler = websocketpp::connection_hdl;

  typedef typename config::concurrency_type concurrency_type;
  typedef websocketpp::connection<config> connection_type;
  typedef typename connection_type::ptr connection_ptr;
  typedef typename transport_type::transport_con_type transport_con_type;
  typedef typename transport_con_type::ptr transport_con_ptr;
  typedef websocketpp::endpoint<connection_type, config> endpoint_type;

  websocket_server()
      : m_server{std::make_shared<server_t>()}
      , m_owns_context{true}
  {
    m_server->init_asio();
    m_server->set_reuse_addr(true);
    m_server->clear_access_channels(websocketpp::log::alevel::all);
    m_server->set_socket_init_handler(init_handler);
  }

  websocket_server(ossia::net::network_context_ptr ctx)
      : m_server{std::make_shared<server_t>()}
      , m_context{ctx}
      , m_owns_context{false}
  {
    m_server->init_asio(&ctx->context);
    m_server->set_reuse_addr(true);
    m_server->clear_access_channels(websocketpp::log::alevel::all);
    m_server->set_socket_init_handler(init_handler);
  }

  static void init_handler(websocketpp::connection_hdl, boost::asio::ip::tcp::socket& s)
  {
    boost::asio::ip::tcp::no_delay option(true);
    try
    {
      s.set_option(option);
    }
    catch(...)
    {
      ossia::logger().trace("Could not set TCP nodelay option");
    }
  }

  template <typename Handler>
  void set_open_handler(Handler h)
  {
    m_server->set_open_handler(h);
  }

  template <typename Handler>
  void set_close_handler(Handler h)
  {
    m_server->set_close_handler(h);
  }

  template <typename Handler>
  void set_message_handler(Handler h)
  {
    m_server->set_message_handler(
        [this, h](connection_handler hdl, server_t::message_ptr msg) {
#if defined OSSIA_BENCHMARK
      auto t1 = std::chrono::high_resolution_clock::now();
#endif
      try
      {
        auto res = h(hdl, msg->get_opcode(), msg->get_raw_payload());
        if(res.data.size() > 0)
        {
          send_message(hdl, res);
        }
      }
      catch(const ossia::node_not_found_error& e)
      {
        auto con = m_server->get_con_from_hdl(hdl);
        ossia::logger().error(
            "Node not found: {} ==> {}", con->get_uri()->get_resource(), e.what());
      }
      catch(const ossia::bad_request_error& e)
      {
        auto con = m_server->get_con_from_hdl(hdl);
        ossia::logger().error(
            "Error in request: {} ==> {}", con->get_uri()->get_resource(), e.what());
      }
      catch(const std::exception& e)
      {
        auto con = m_server->get_con_from_hdl(hdl);
        ossia::logger().error("Error in request: {}", e.what());
      }
      catch(...)
      {
        auto con = m_server->get_con_from_hdl(hdl);
        ossia::logger().error("Error in request");
      }

#if defined OSSIA_BENCHMARK
      auto t2 = std::chrono::high_resolution_clock::now();
      ossia::logger().info(
          "Time taken: {}",
          std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
#endif
    });

    m_server->set_http_handler([this, h](connection_handler hdl) {
      auto con = m_server->get_con_from_hdl(hdl);

      // enable cross origin requests from anywhere
      con->append_header("Access-Control-Allow-Origin", "*");

      try
      {
        ossia::net::server_reply str
            = h(hdl, websocketpp::frame::opcode::TEXT, con->get_uri()->get_resource());

        switch(str.type)
        {
          case server_reply::data_type::json: {
            con->replace_header("Content-Type", "application/json; charset=utf-8");
            str.data += "\0";
            break;
          }
          case server_reply::data_type::html: {
            con->replace_header("Content-Type", "text/html; charset=utf-8");
            break;
          }
          default:
            break;
        }
        con->replace_header("Connection", "close");
        con->set_body(std::move(str.data));
        con->set_status(websocketpp::http::status_code::ok);
        return;
      }
      catch(const ossia::node_not_found_error& e)
      {
        con->set_status(websocketpp::http::status_code::not_found);
      }
      catch(const ossia::bad_request_error& e)
      {
        ossia::logger().error(
            "Error in request: {} ==> {}", con->get_uri()->get_resource(), e.what());
        con->set_status(websocketpp::http::status_code::bad_request);
      }
      catch(const std::exception& e)
      {
        ossia::logger().error("Error in request: {}", e.what());
      }
      catch(...)
      {
        ossia::logger().error("Error in request");
      }
      con->set_status(websocketpp::http::status_code::internal_server_error);
    });
  }

  void listen(uint16_t port = 9002)
  {
    m_server->listen(boost::asio::ip::tcp::v4(), port);

    boost::system::error_code ec;
    start_accept(ec);
  }

  void start_accept(boost::system::error_code& ec)
  {
    ec = boost::system::error_code();
    auto con = m_server->get_connection(ec);

    if(!con)
    {
      ///ec = boost::asio::error::make_error_code(boost::asio::error::con_creation_failed);
      return;
    }

    namespace lib = websocketpp::lib;
    m_server->transport_type::async_accept(
        lib::static_pointer_cast<transport_con_type>(con),
        [server = m_server, con](lib::error_code ec) {
      if(server)
      {
        if(ec)
          ec = websocketpp::error::http_connection_ended;
        server->handle_accept_legacy(con, ec);
      }
    }, ec);

    if(ec && con)
    {
      // If the connection was constructed but the accept failed,
      // terminate the connection to prevent memory leaks
      con->terminate(lib::error_code());
    }
  }

  void run() { m_server->run(); }

  void stop()
  {
    // this change was undone because of OSSIA/libossia#416 :

    // // (temporarily?) changed to stop_listening()
    // // "Straight up stop forcibly stops a bunch of things
    // // in a way that bypasses most, if not all, of the cleanup routines"

    try
    {
      boost::system::error_code ec;
      if(m_server->is_listening())
        m_server->stop_listening(ec);
    }
    catch(...)
    {
    }

    if(m_owns_context)
    {
      m_server->stop();
    }
    else
    {
      boost::asio::post(
          m_context->context, [s = m_server]() mutable { new decltype(s){s}; });
    }
  }

  void close(connection_handler hdl)
  {
    auto con = m_server->get_con_from_hdl(hdl);
    con->close(websocketpp::close::status::going_away, "Server shutdown");
  }

  void send_message(connection_handler hdl, const std::string& message)
  {
    auto con = m_server->get_con_from_hdl(hdl);
    con->send(message);
  }

  void send_message(connection_handler hdl, const ossia::net::server_reply& message)
  {
    auto con = m_server->get_con_from_hdl(hdl);
    switch(message.type)
    {
      case server_reply::data_type::json:
      case server_reply::data_type::html:
        con->send(message.data, websocketpp::frame::opcode::TEXT);
        break;
      default:
        con->send(message.data, websocketpp::frame::opcode::BINARY);
        break;
    }
  }

  void send_message(connection_handler hdl, const rapidjson::StringBuffer& message)
  {
    auto con = m_server->get_con_from_hdl(hdl);
    con->send(message.GetString(), message.GetSize(), websocketpp::frame::opcode::text);
  }

  void send_binary_message(connection_handler hdl, const std::string& message)
  {
    auto con = m_server->get_con_from_hdl(hdl);
    con->send(message.data(), message.size(), websocketpp::frame::opcode::binary);
  }

  void send_binary_message(connection_handler hdl, std::string_view message)
  {
    auto con = m_server->get_con_from_hdl(hdl);
    con->send(message.data(), message.size(), websocketpp::frame::opcode::binary);
  }

  server_t& impl() { return *m_server; }

protected:
  std::shared_ptr<server_t> m_server;
  ossia::net::network_context_ptr m_context;
  bool m_owns_context{};
};
}
