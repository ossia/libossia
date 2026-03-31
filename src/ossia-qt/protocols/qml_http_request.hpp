#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/http/http_client.hpp>
#include <ossia/network/http/http_client_request.hpp>

#include <ossia-qt/protocols/utils.hpp>
#include <ossia-qt/qml_protocols.hpp>

#include <verdigris>

namespace ossia::qt
{

struct qml_protocols_http_answer;
struct qml_protocols_http_error;
using request_type
    = ossia::net::http_get_request<qml_protocols_http_answer, qml_protocols_http_error>;

struct qml_protocols_http_answer
{
  static constexpr int reserve_expect = 65536 * 8;
  QPointer<qml_protocols> self{};
  QJSValue v;
  void operator()(auto& req, std::string_view str)
  {
    ossia::qt::run_async(self.get(), [self = self, v = v, s = QString::fromUtf8(str)] {
      if(self)
        if(v.isCallable())
          v.call({s});
    });
  }
};
struct qml_protocols_http_error
{
  void operator()(auto& self) { }
};

// --- New fetch() types (full HTTP client) ---

struct qml_protocols_fetch_answer;
struct qml_protocols_fetch_error;
using fetch_request_type = ossia::net::http_client_request<
    qml_protocols_fetch_answer, qml_protocols_fetch_error>;

struct qml_protocols_fetch_answer
{
  static constexpr int reserve_expect = 65536 * 8;
  QPointer<qml_protocols> self{};
  QJSValue onResponse;
  void operator()(auto& req, int status, std::string_view str)
  {
    ossia::qt::run_async(
        self.get(),
        [self = self, v = onResponse, status, s = QString::fromUtf8(str)] {
      if(self)
        if(v.isCallable())
        {
          auto engine = qjsEngine(self.get());
          if(engine)
            v.call({QJSValue(status), engine->toScriptValue(s)});
        }
    });
  }
};

struct qml_protocols_fetch_error
{
  QPointer<qml_protocols> self{};
  QJSValue onError;
  void operator()(auto& req, std::string_view msg)
  {
    ossia::qt::run_async(
        self.get(), [self = self, v = onError, m = QString::fromUtf8(msg)] {
      if(self)
        if(v.isCallable())
          v.call({m});
    });
  }
};

}
