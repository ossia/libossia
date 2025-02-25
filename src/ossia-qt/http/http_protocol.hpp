#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>

#include <ossia-qt/js_utilities.hpp>

#include <QByteArray>
#include <QJSValue>
#include <QList>
#include <QObject>

#include <verdigris>

class QQmlEngine;
class QQmlComponent;
class QNetworkAccessManager;
class QNetworkReply;

namespace ossia::net
{

struct http_parameter_data_base
{
  http_parameter_data_base() = default;
  http_parameter_data_base(const http_parameter_data_base&) = default;
  http_parameter_data_base(http_parameter_data_base&&) = default;
  http_parameter_data_base& operator=(const http_parameter_data_base&) = default;
  http_parameter_data_base& operator=(http_parameter_data_base&&) = default;
  http_parameter_data_base(const QJSValue& val)
  {
    if(auto r = val.property("request"); r.isString() || r.isCallable())
    {
      request = r;
    }

    if(requestIsValid())
    {
      auto a = val.property("answer");
      if(a.isCallable())
      {
        answer = a;
      }
    }

    if(auto data = val.property("requestData"); data.isString() || data.isCallable())
    {
      requestData = data;
    }
    if(auto post = val.property("method"); post.toString().toLower() == "post")
    {
      is_post = true;
    }
    if(auto r = val.property("onRead"); r.isCallable())
    {
      onRead = r;
    }
    if(auto r = val.property("onReadString"); r.isCallable())
    {
      onReadString = r;
    }
  }

  bool requestIsValid() const noexcept
  {
    return !request.isUndefined() && !request.isNull();
  }
  QJSValue request;
  QJSValue answer;
  QJSValue requestData;
  QJSValue onRead;
  QJSValue onReadString;
  bool is_post{};
};

struct http_parameter_data
    : public parameter_data
    , public http_parameter_data_base
{
  using base_data_type = http_parameter_data_base;
  http_parameter_data() = default;
  http_parameter_data(const http_parameter_data&) = default;
  http_parameter_data(http_parameter_data&&) = default;
  http_parameter_data& operator=(const http_parameter_data&) = default;
  http_parameter_data& operator=(http_parameter_data&&) = default;

  http_parameter_data(const std::string& name)
      : parameter_data{name}
  {
  }

  http_parameter_data(const QJSValue& val)
      : parameter_data{ossia::qt::make_parameter_data(val)}
      , http_parameter_data_base{val}
  {
  }

  bool valid() const noexcept { return requestIsValid() || type; }
};

using http_parameter = wrapped_parameter<http_parameter_data>;
using http_node = ossia::net::wrapped_node<http_parameter_data, http_parameter>;
class OSSIA_EXPORT http_protocol final
    : public QObject
    , public ossia::net::protocol_base
{
  W_OBJECT(http_protocol)

public:
  explicit http_protocol(QByteArray code);

  http_protocol(const http_protocol&) = delete;
  http_protocol(http_protocol&&) = delete;
  http_protocol& operator=(const http_protocol&) = delete;
  http_protocol& operator=(http_protocol&&) = delete;

  ~http_protocol() override;

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::parameter_base& parameter_base) override;

  bool
  push(const ossia::net::parameter_base& parameter_base, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override;

  void set_device(ossia::net::device_base& dev) override;

  static http_parameter_data read_data(const QJSValue& js) { return js; }

public:
  void sig_push(http_parameter* arg_1, const ossia::value& v)
      E_SIGNAL(OSSIA_EXPORT, sig_push, arg_1, v)

private:
  void slot_push(http_parameter*, const ossia::value& v);
  W_SLOT(slot_push);

private:
  QUrl requestUrl(const http_parameter* addr_p, const ossia::value& v);
  QByteArray requestData(const http_parameter* addr_p, const ossia::value& v);
  void apply_reply(QJSValue);

  QQmlEngine* m_engine{};
  QQmlComponent* m_component{};

  QNetworkAccessManager* m_access{};

  QByteArray m_code;
  ossia::net::device_base* m_device{};
  ossia::hash_map<QNetworkReply*, http_parameter*> m_replies;
};
using http_device = ossia::net::wrapped_device<http_node, http_protocol>;
class OSSIA_EXPORT HTTP : public QObject
{
};
}

Q_DECLARE_METATYPE(ossia::net::http_parameter*)
W_REGISTER_ARGTYPE(ossia::net::http_parameter*)
W_REGISTER_ARGTYPE(const ossia::net::http_parameter*)
