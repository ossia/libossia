#pragma once
#define ASIO_STANDALONE 1
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/http/request.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/detail/json.hpp>
namespace ossia
{
namespace oscquery
{
class websocket_server
{
  public:
    using server_t = websocketpp::server<websocketpp::config::asio>;
    using connection_handler = websocketpp::connection_hdl;

    websocket_server()
    {
      m_server.init_asio();
      m_server.set_reuse_addr(true);
    }

    template<typename Handler>
    void set_open_handler(Handler h)
    {
        m_server.set_open_handler(h);
    }

    template<typename Handler>
    void set_close_handler(Handler h)
    {
        m_server.set_close_handler(h);
    }

    template<typename Handler>
    void set_message_handler(Handler h)
    {
        m_server.set_message_handler([=] (connection_handler hdl, server_t::message_ptr msg)
        {
          try
          {
            send_message(hdl, h(hdl, msg->get_payload()));
          }
          catch(const ossia::node_not_found_error& e)
          {
            auto con = m_server.get_con_from_hdl(hdl);
            con->set_status(websocketpp::http::status_code::not_found);
          }
          catch(const ossia::bad_request_error& e)
          {
            auto con = m_server.get_con_from_hdl(hdl);
            ossia::logger().error("Error in request: {} ==> {}",
                  con->get_uri()->get_resource(), e.what());

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
        });

        m_server.set_http_handler([=] (connection_handler hdl)
        {
          auto con = m_server.get_con_from_hdl(hdl);

          con->replace_header("Content-Type", "application/json; charset=utf-8");
          try
          {
            rapidjson::StringBuffer base_str = h(hdl, con->get_uri()->get_resource());
            std::string str{base_str.GetString(), base_str.GetSize()};
            str += "\0";
            con->set_body(std::move(str));
            con->set_status(websocketpp::http::status_code::ok);
          }
          catch(const ossia::node_not_found_error& e)
          {
            con->set_status(websocketpp::http::status_code::not_found);
          }
          catch(const ossia::bad_request_error& e)
          {
            ossia::logger().error("Error in request: {} ==> {}",
                  con->get_uri()->get_resource(), e.what());
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
        });
    }

    void run(uint16_t port = 9002)
    {
      m_server.listen(port);
      m_server.start_accept();
      m_server.run();
    }

    void stop() { m_server.stop(); }

    void send_message(connection_handler hdl, const std::string& message)
    {
      auto con = m_server.get_con_from_hdl(hdl);
      con->send(message);
    }

    void send_message(connection_handler hdl, const rapidjson::StringBuffer& message)
    {
      auto con = m_server.get_con_from_hdl(hdl);
      con->send(message.GetString(), message.GetSize(), websocketpp::frame::opcode::text);
    }

  private:
    server_t m_server;
};

}
}
