#pragma once
#include <ossia/detail/config.hpp>
#include <ossia/detail/json.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/oscquery/detail/server_reply.hpp>
#include <ossia/network/exceptions.hpp>
#include <boost/config.hpp>
#include <boost/version.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/http/request.hpp>
#include <websocketpp/server.hpp>
#if defined(OSSIA_BENCHMARK)
#include <chrono>
#endif
namespace ossia
{
namespace oscquery
{

//! Low-level websocket & http server for oscquery
class websocket_server
{
public:
  using server_t = websocketpp::server<websocketpp::config::asio>;
  using connection_handler = websocketpp::connection_hdl;

  websocket_server()
  {
    m_server.init_asio();
    m_server.set_reuse_addr(true);
    m_server.clear_access_channels(websocketpp::log::alevel::all);
    m_server.set_socket_init_handler(
        [](websocketpp::connection_hdl, asio::ip::tcp::socket& s) {
          asio::ip::tcp::no_delay option(true);
          try {
          s.set_option(option);
          } catch(...) {
            ossia::logger().warn("Could not set TCP nodelay option");
          }
        });
  }

  template <typename Handler>
  void set_open_handler(Handler h)
  {
    m_server.set_open_handler(h);
  }

  template <typename Handler>
  void set_close_handler(Handler h)
  {
    m_server.set_close_handler(h);
  }

  template <typename Handler>
  void set_message_handler(Handler h)
  {
    m_server.set_message_handler(
        [=](connection_handler hdl, server_t::message_ptr msg) {
#if defined OSSIA_BENCHMARK
          auto t1 = std::chrono::high_resolution_clock::now();
#endif
          try
          {
            auto res = h(hdl, msg->get_raw_payload());
            if (res.data.size() > 0)
            {
              send_message(hdl, res);
            }
          }
          catch (const ossia::node_not_found_error& e)
          {
            auto con = m_server.get_con_from_hdl(hdl);
            ossia::logger().error(
                "Node not found: {} ==> {}", con->get_uri()->get_resource(),
                e.what());
          }
          catch (const ossia::bad_request_error& e)
          {
            auto con = m_server.get_con_from_hdl(hdl);
            ossia::logger().error(
                "Error in request: {} ==> {}", con->get_uri()->get_resource(),
                e.what());
          }
          catch (const std::exception& e)
          {
            auto con = m_server.get_con_from_hdl(hdl);
            ossia::logger().error("Error in request: {}", e.what());
          }
          catch (...)
          {
            auto con = m_server.get_con_from_hdl(hdl);
            ossia::logger().error("Error in request");
          }

#if defined OSSIA_BENCHMARK
          auto t2 = std::chrono::high_resolution_clock::now();
          ossia::logger().info(
              "Time taken: {}",
              std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                  .count());
#endif
        });

    m_server.set_http_handler([=](connection_handler hdl) {
      auto con = m_server.get_con_from_hdl(hdl);

      try
      {
        ossia::oscquery::server_reply str = h(hdl, con->get_uri()->get_resource());

        switch(str.type)
        {
          case server_reply::data_type::json:
          {
            con->replace_header("Content-Type", "application/json; charset=utf-8");
            str.data += "\0";
            break;
          }
          case server_reply::data_type::html:
          {
            con->replace_header("Content-Type", "text/html; charset=utf-8");
            break;
          }
          default:
            break;
        }
        con->replace_header("Connection", "close");
        con->set_body(std::move(str.data));
        con->set_status(websocketpp::http::status_code::ok);
      }
      catch (const ossia::node_not_found_error& e)
      {
        con->set_status(websocketpp::http::status_code::not_found);
      }
      catch (const ossia::bad_request_error& e)
      {
        ossia::logger().error(
            "Error in request: {} ==> {}", con->get_uri()->get_resource(),
            e.what());
        con->set_status(websocketpp::http::status_code::bad_request);
      }
      catch (const std::exception& e)
      {
        ossia::logger().error("Error in request: {}", e.what());
      }
      catch (...)
      {
        ossia::logger().error("Error in request");
      }
    });
  }

  void listen(uint16_t port = 9002)
  {
    m_server.listen(asio::ip::tcp::v4(), port);
    m_server.start_accept();
  }

  void run()
  {
    m_server.run();
  }

  void stop()
  {
      // (temporarily?) changed to stop_listening()
      // "Straight up stop forcibly stops a bunch of things
      // in a way that bypasses most, if not all, of the cleanup routines"

      //m_server.stop();
      if(m_server.is_listening())
        m_server.stop_listening();
  }

  void close(connection_handler hdl)
  {
    auto con = m_server.get_con_from_hdl(hdl);
    con->close(websocketpp::close::status::going_away, "Server shutdown");
  }

  void send_message(connection_handler hdl, const std::string& message)
  {
    auto con = m_server.get_con_from_hdl(hdl);
    con->send(message);
  }

  void send_message(connection_handler hdl, const ossia::oscquery::server_reply& message)
  {
    auto con = m_server.get_con_from_hdl(hdl);
    switch (message.type)
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

  void
  send_message(connection_handler hdl, const rapidjson::StringBuffer& message)
  {
    auto con = m_server.get_con_from_hdl(hdl);
    con->send(
        message.GetString(), message.GetSize(),
        websocketpp::frame::opcode::text);
  }

  server_t& impl()
  {
    return m_server;
  }

private:
  server_t m_server;
};
}
}
