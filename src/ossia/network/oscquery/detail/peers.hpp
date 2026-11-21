#pragma once
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/json.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

/**
 * PEERS and CLIENT_NAME, an extension of OSCQuery for servers that say who
 * reaches them, off by default (set_peers_extension on a server). When on,
 * the server declares both in the EXTENSIONS of HOST_INFO and:
 *
 * - answers `/?PEERS`, at the root only, with
 *   { "CLIENTS": [ { "IP", "PORT", "NAME", "OSC_PORT", "SENDER_PORT",
 *                    "LISTEN": [ paths ] } ],
 *     "SENDERS": [ { "IP", "PORT", "LAST_MS" } ] }
 *   where what the server cannot fill is left out. CLIENTS are its WebSocket
 *   clients; SENDERS the UDP sources that are none of them, the last 256,
 *   most recent first, LAST_MS being the time since the last message.
 * - takes the name a client gives itself with the text command
 *   { "COMMAND": "CLIENT_NAME", "DATA": "<name>" }.
 *
 * A client that does not know the extension sees two more keys in
 * HOST_INFO and nothing else.
 */
namespace ossia::oscquery::detail
{
constexpr std::string_view peers() noexcept
{
  return "PEERS";
}
constexpr std::string_view client_name() noexcept
{
  return "CLIENT_NAME";
}

//! The UDP sources a server receives from, the last max_senders
class peer_senders
{
public:
  static constexpr std::size_t max_senders = 256;

  struct sender
  {
    std::string ip;
    uint16_t port{};
    std::chrono::steady_clock::time_point last{};
  };

  void record(std::string ip, uint16_t port)
  {
    auto key = ip + ":" + std::to_string(port);
    std::lock_guard lock{m_mutex};
    auto [it, added] = m_senders.try_emplace(std::move(key));
    auto& s = it->second;
    if(added)
    {
      s.ip = std::move(ip);
      s.port = port;
    }
    s.last = std::chrono::steady_clock::now();

    // A source that changes its port at each datagram does not grow the list
    if(added && m_senders.size() > max_senders)
    {
      m_senders.erase(std::min_element(
          m_senders.begin(), m_senders.end(),
          [](const auto& a, const auto& b) { return a.second.last < b.second.last; }));
    }
  }

  std::vector<sender> recent_first() const
  {
    std::vector<sender> res;
    {
      std::lock_guard lock{m_mutex};
      res.reserve(m_senders.size());
      for(const auto& [key, s] : m_senders)
        res.push_back(s);
    }
    std::sort(res.begin(), res.end(), [](const sender& a, const sender& b) {
      return a.last > b.last;
    });
    return res;
  }

private:
  mutable std::mutex m_mutex;
  ossia::hash_map<std::string, sender> m_senders;
};

//! The answer to ?PEERS. The caller holds the lock of the clients; senders
//! is null for a server that does not know where its UDP messages come from.
template <typename Clients>
std::string write_peers(const Clients& clients, const peer_senders* senders)
{
  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> wr{buf};
  wr.StartObject();
  wr.Key("CLIENTS");
  wr.StartArray();
  for(const auto& client_p : clients)
  {
    auto& c = *client_p;
    wr.StartObject();
    wr.Key("IP");
    wr.String(c.client_ip.c_str());
    if(c.client_port)
    {
      wr.Key("PORT");
      wr.Int(c.client_port);
    }
    if(!c.name.empty())
    {
      wr.Key("NAME");
      wr.String(c.name.c_str());
    }
    if(c.osc_port)
    {
      wr.Key("OSC_PORT");
      wr.Int(c.osc_port);
    }
    if(c.remote_sender_port)
    {
      wr.Key("SENDER_PORT");
      wr.Int(c.remote_sender_port);
    }
    wr.Key("LISTEN");
    wr.StartArray();
    {
      std::vector<std::string> paths;
      {
        std::lock_guard lck{c.listeningMutex};
        for(const auto& [path, param] : c.listening)
          paths.push_back(path);
      }
      std::sort(paths.begin(), paths.end());
      for(const auto& path : paths)
        wr.String(path.c_str());
    }
    wr.EndArray();
    wr.EndObject();
  }
  wr.EndArray();

  if(senders)
  {
    const auto now = std::chrono::steady_clock::now();
    wr.Key("SENDERS");
    wr.StartArray();
    for(const auto& s : senders->recent_first())
    {
      // A client may have declared that sender since it was recorded
      const bool is_client
          = std::any_of(clients.begin(), clients.end(), [&](const auto& c) {
        return c->client_ip == s.ip && c->remote_sender_port == s.port;
      });
      if(is_client)
        continue;
      wr.StartObject();
      wr.Key("IP");
      wr.String(s.ip.c_str());
      wr.Key("PORT");
      wr.Int(s.port);
      wr.Key("LAST_MS");
      wr.Int64(
          std::chrono::duration_cast<std::chrono::milliseconds>(now - s.last).count());
      wr.EndObject();
    }
    wr.EndArray();
  }
  wr.EndObject();
  return std::string(buf.GetString(), buf.GetSize());
}

//! The CLIENT_NAME command: true if doc is one, and then name is the name
//! it gives (empty if it gives none)
inline bool parse_client_name(const rapidjson::Document& doc, std::string& name)
{
  if(!doc.IsObject())
    return false;
  auto cmd = doc.FindMember("COMMAND");
  if(cmd == doc.MemberEnd() || !cmd->value.IsString()
     || std::string_view{cmd->value.GetString(), cmd->value.GetStringLength()}
            != client_name())
    return false;
  auto data = doc.FindMember("DATA");
  if(data != doc.MemberEnd() && data->value.IsString())
    name.assign(data->value.GetString(), data->value.GetStringLength());
  return true;
}
}
