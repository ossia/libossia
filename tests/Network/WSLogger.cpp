// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/detail/thread.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>
#include <ossia/network/sockets/websocket_server.hpp>

#include "include_catch.hpp"
using namespace ossia;

TEST_CASE("test_websockets_log_no_connection", "test_websockets_log_no_connection")
{
  ossia::string_view host = "127.0.0.1";
  std::string appname = "foo";

  auto con = std::make_shared<ossia::websocket_threaded_connection>(std::string(host));
  auto sink = std::make_shared<websocket_log_sink>(con, appname);
  auto log = std::make_shared<spdlog::logger>("max_logger", sink);

  boost::asio::io_context ctx;
  ctx.run_for(std::chrono::milliseconds(100));
}

TEST_CASE("test_websockets_log_connection", "test_websockets_log_connection")
{
  bool opened = false;
  bool message = false;
  bool closed = false;
  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::websocket_server srv{ctx};
  srv.set_open_handler([&](auto&&...) { opened = true; });
  srv.set_message_handler([&](auto&&...) {
    message = true;
    return ossia::net::server_reply{"<html></html>"};
  });
  srv.set_close_handler([&](auto&&...) { closed = true; });
  srv.listen(5567);

  {
    ossia::string_view host = "ws://127.0.0.1:5567";
    std::string appname = "foo";

    auto con = std::make_shared<ossia::websocket_threaded_connection>(std::string(host));
    auto sink = std::make_shared<websocket_log_sink>(con, appname);
    auto log = std::make_shared<spdlog::logger>("max_logger", sink);
    ctx->context.run_for(std::chrono::milliseconds(100));
    log->info("helo");
    ctx->context.run_for(std::chrono::milliseconds(100));
  }
  ctx->context.run_for(std::chrono::milliseconds(100));

  REQUIRE(opened);
  REQUIRE(message);
  REQUIRE(closed);
}
