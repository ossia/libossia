#pragma once
#include <ossia/network/osc/detail/osc_value_write_visitor.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <ossia-qt/invoke.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <boost/asio/io_context.hpp>

#include <QPointer>

#define run_on_qt_thread(...) \
  ossia::qt::run_async(this, [=, this] { __VA_ARGS__; }, Qt::AutoConnection);
#define run_on_asio_thread(...) \
  boost::asio::dispatch(this->context(), [=, self = QPointer{this}] { __VA_ARGS__ });

namespace ossia::qt
{
struct protocols_sender
{
  void send_osc(this auto&& self, QByteArray address, QJSValueList values)
  {
    using socket_type = std::remove_cvref_t<decltype(self.socket)>;
    using writer_type = ossia::net::socket_writer<socket_type>;
    using send_visitor = ossia::net::osc_value_send_visitor<
        ossia::net::full_parameter_data, ossia::net::osc_1_0_policy, writer_type>;

    ossia::net::full_parameter_data p;

    switch(values.size())
    {
      case 0: {
        ossia::value{ossia::impulse{}}.apply(
            send_visitor{p, address.toStdString(), writer_type{self.socket}});
        break;
      }
      case 1: {
        auto v = ossia::qt::value_from_js(values[0]);
        v.apply(send_visitor{p, address.toStdString(), writer_type{self.socket}});
        break;
      }
      default: {
        std::vector<ossia::value> vec;
        vec.reserve(values.size());
        for(const auto& v : values)
          vec.push_back(ossia::qt::value_from_js(v));
        ossia::value vvec(std::move(vec));
        vvec.apply(send_visitor{p, address.toStdString(), writer_type{self.socket}});
      }
    }
  }
};
}
