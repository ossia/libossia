#pragma once
#define ASIO_STANDALONE 1
#include <boost/config.hpp>
#include <boost/version.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <ossia/detail/logger.hpp>

namespace ossia
{
namespace oscquery
{
class client
{
  public:
    using connection_handler = websocketpp::connection_hdl;

    template<typename MessageHandler>
    client(MessageHandler&& messageHandler) :
      m_open{false}
    {
      m_client.clear_access_channels(websocketpp::log::alevel::all);
      m_client.clear_error_channels(websocketpp::log::elevel::all);
      m_client.init_asio();

      m_client.set_open_handler([=] (connection_handler hdl)
      {
        scoped_lock guard(m_lock);
        m_open = true;
      });

      m_client.set_message_handler(
            [handler=std::move(messageHandler)] (connection_handler hdl, client_t::message_ptr msg)
      { handler(hdl, msg->get_raw_payload()); });

      m_client.set_fail_handler([=] (connection_handler hdl) {
          scoped_lock guard(m_lock);
          m_open = false;
      });
      m_client.set_close_handler([=] (connection_handler hdl) {
          scoped_lock guard(m_lock);
          m_open = false;
      });
    }

    ~client()
    {
      if(m_open)
        stop();
    }

    bool connected() const
    { return m_open; }

    void stop()
    {
      scoped_lock guard(m_lock);
      m_client.close(m_hdl, websocketpp::close::status::normal, "");
      m_open = false;
    }

    void close() { stop(); }

    // This function returns if the connection is stopped / fails.
    void connect(const std::string & uri)
    {
      websocketpp::lib::error_code ec;

      auto con = m_client.get_connection(uri, ec);
      if (ec)
      {
        m_client.get_alog().write(websocketpp::log::alevel::app,
                                  "Get Connection Error: " + ec.message());
        return;
      }

      m_hdl = con->get_handle();
      m_client.connect(con);

      m_client.run();
      m_client.reset(); // In order to be able to reconnect afterwards.
    }

    void send_message(const std::string& request)
    {
      if(!m_open)
        return;

      websocketpp::lib::error_code ec;

      m_client.send(m_hdl, request, websocketpp::frame::opcode::text, ec);

      if (ec)
      {
        m_client.get_alog().write(websocketpp::log::alevel::app,
                                  "Send Error: " + ec.message());
      }
    }

  private:
    using client_t = websocketpp::client<websocketpp::config::asio_client>;
    using scoped_lock = websocketpp::lib::lock_guard<websocketpp::lib::mutex>;

    client_t m_client;
    connection_handler m_hdl;
    websocketpp::lib::mutex m_lock;
    bool m_open;
};
}
}
