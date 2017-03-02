#pragma once
#include <ossia/network/oscquery/detail/client.hpp>
#include <ossia/detail/json.hpp>

namespace ossia
{
struct websocket_log_sink final : public spdlog::sinks::sink
{
  rapidjson::StringBuffer buffer;

  ossia::oscquery::websocket_client s;
  std::thread thread;

  websocket_log_sink(std::string ip):
    s([] (auto&&...) {})
  {
    thread = std::thread( [=] { s.connect(ip); } );
  }

  ~websocket_log_sink()
  {
    s.stop();
    thread.join();
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

    s.send_message(buffer);
  }

  void flush() override
  {
  }
};

}
