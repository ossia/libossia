#pragma once
#include <ossia/network/osc/detail/osc_value_write_visitor.hpp>
#include <ossia/network/osc/detail/osc_packet_processor.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <ossia-qt/invoke.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <boost/asio/io_context.hpp>

#include <QPointer>
#include <QJSEngine>

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

class qml_osc_processor : public QObject
{
  W_OBJECT(qml_osc_processor)
public:
  explicit qml_osc_processor() { }

  QJSValue onOsc;

  void processMessage(const QByteArray& bv)
  {
    if(onOsc.isCallable())
    {
      ossia::net::osc_packet_processor<decltype(*this)> processor{*this};
      processor(bv.data(), bv.size());
    }
  }
  W_SLOT(processMessage)

  void operator()(const oscpack::ReceivedMessage& msg)
  {
    QString address = QString::fromUtf8(msg.AddressPattern());

    QJSValueList values;
    auto* engine = qjsEngine(this);
    if(!engine)
      return;

    for(auto it = msg.ArgumentsBegin(); it != msg.ArgumentsEnd(); ++it)
      values.append(oscArgToQJSValue(*it, *engine));

    onOsc.call({engine->toScriptValue(address), engine->toScriptValue(values)});
  }

  static QJSValue
  oscArgToQJSValue(const oscpack::ReceivedMessageArgument& arg, QJSEngine& engine)
  {
    switch(arg.TypeTag())
    {
      case oscpack::INT32_TYPE_TAG:
        return QJSValue(arg.AsInt32Unchecked());
      case oscpack::INT64_TYPE_TAG:
        return QJSValue((int)arg.AsInt64Unchecked());
      case oscpack::FLOAT_TYPE_TAG:
        return QJSValue(arg.AsFloatUnchecked());
      case oscpack::DOUBLE_TYPE_TAG:
        return QJSValue(arg.AsDoubleUnchecked());
      case oscpack::TIME_TAG_TYPE_TAG:
        return QJSValue((int)arg.AsTimeTagUnchecked());
      case oscpack::CHAR_TYPE_TAG:
        return QJSValue(QString(QChar(arg.AsCharUnchecked())));
      case oscpack::TRUE_TYPE_TAG:
        return QJSValue(true);
      case oscpack::FALSE_TYPE_TAG:
        return QJSValue(false);
      case oscpack::STRING_TYPE_TAG:
        return QJSValue(QString::fromUtf8(arg.AsStringUnchecked()));
      case oscpack::SYMBOL_TYPE_TAG:
        return QJSValue(QString::fromUtf8(arg.AsSymbolUnchecked()));
      case oscpack::BLOB_TYPE_TAG: {
        const void* data{};
        oscpack::osc_bundle_element_size_t size{};
        arg.AsBlobUnchecked(data, size);
        QByteArray blob((const char*)data, size);
        return engine.toScriptValue(blob);
      }
      case oscpack::RGBA_COLOR_TYPE_TAG: {
        auto c = arg.AsRgbaColorUnchecked();
        auto array = engine.newArray(4);
        array.setProperty(0, uint8_t(c >> 24 & 0xFF));
        array.setProperty(1, uint8_t(c >> 16 & 0xFF));
        array.setProperty(2, uint8_t(c >> 8 & 0xFF));
        array.setProperty(3, uint8_t(c & 0xFF));
        return array;
      }
      default:
        return QJSValue(QJSValue::NullValue);
    }
  }
};
}
