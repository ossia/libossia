#pragma once
#include <ossia/network/oscquery/detail/client.hpp>
#include <ossia/detail/json.hpp>

namespace ossia
{
struct websocket_threaded_connection
{
  ossia::oscquery::websocket_client socket;
  std::thread thread;

  websocket_threaded_connection(const std::string& ip):
    socket([] (auto&&...) {})
  {
    thread = std::thread( [=] { socket.connect(ip); } );
  }

  ~websocket_threaded_connection()
  {
    socket.stop();
    thread.join();
  }
};

struct websocket_log_sink final : public spdlog::sinks::sink
{
  rapidjson::StringBuffer buffer;
  std::shared_ptr<websocket_threaded_connection> socket;

  websocket_log_sink(std::shared_ptr<websocket_threaded_connection> s):
    socket{std::move(s)}
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

    writer.Key("message");
    writer.String(msg.raw.data(), msg.raw.size());

    writer.EndObject();

    socket->socket.send_message(buffer);
  }

  void flush() override
  {
  }
};

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
