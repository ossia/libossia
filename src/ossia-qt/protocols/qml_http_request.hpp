#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/http/http_client.hpp>

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

}
