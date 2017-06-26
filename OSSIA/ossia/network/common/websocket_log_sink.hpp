#pragma once
#include <ossia/network/oscquery/detail/client.hpp>
#include <ossia/detail/json.hpp>
#include <atomic>

namespace ossia
{
struct websocket_threaded_connection
{
  ossia::oscquery::websocket_client socket;
  std::atomic_bool running{};
  std::thread thread;

  websocket_threaded_connection(const std::string& ip):
    socket([] (auto&&...) {})
  {
    running = true;
    thread = std::thread( [=] {
        while(running) {
          socket.connect(ip);
          ossia::logger().critical("Logger could not connect to {}", ip);
          if(running) {
              // Try to reconnect
              std::this_thread::sleep_for(std::chrono::seconds(1));
          }
        }
    } );
  }

  ~websocket_threaded_connection()
  {
    running = false;
    socket.stop();
    if(thread.joinable())
        thread.join();
  }
};

//! A sink to use with spdlog, that will send its log messages over websockets.
struct websocket_log_sink final : public spdlog::sinks::sink
{
  rapidjson::StringBuffer buffer;
  std::shared_ptr<websocket_threaded_connection> socket;
  std::string sender;

  websocket_log_sink(std::shared_ptr<websocket_threaded_connection> s,
                     std::string send):
    socket{std::move(s)}
  , sender{std::move(send)}
  {
  }

  void log(const spdlog::details::log_msg& msg) override
  {
    buffer.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};

    writer.StartObject();

    writer.Key("operation");
    writer.String("log");

    writer.Key("level");
    writer.String(spdlog::level::level_names[msg.level]);

    writer.Key("sender");
    writer.String(sender.data(), sender.size());

    writer.Key("message");
    writer.String(msg.raw.data(), msg.raw.size());

    writer.EndObject();

    socket->socket.send_message(buffer);
  }

  void flush() override
  {
  }
};

//! Sends websocket "alive" messages at regular intervals.
struct websocket_heartbeat
{
  rapidjson::StringBuffer buffer;
  std::thread thread;
  std::atomic_bool running{true};
  websocket_heartbeat(std::shared_ptr<websocket_threaded_connection> t, std::chrono::seconds dur)
  {
    thread = std::thread([=] {
      while(running)
      {
        std::this_thread::sleep_for(dur);

        buffer.Clear();
        rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};
        writer.StartObject();

        writer.Key("operation");
        writer.String("alive");

        writer.EndObject();

        t->socket.send_message(buffer);
      }
    });
  }

  ~websocket_heartbeat()
  {
    running = false;
    thread.join();
  }
};

}
