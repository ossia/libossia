#pragma once
#include <ossia/detail/buffer_pool.hpp>
#include <ossia/network/osc/detail/osc_1_0_policy.hpp>
#include <ossia/network/osc/detail/osc_messages.hpp>
#include <ossia/network/osc/detail/osc_value_write_visitor.hpp>
#include <ossia/network/sockets/encoding.hpp>
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
struct buffer_writer
{
  QByteArray& buffer;
  void operator()(const char* data, std::size_t sz) const
  {
    buffer.append(data, sz);
  }
};

inline QByteArray
apply_encoding(ossia::net::encoding enc, const QByteArray& data)
{
  if(enc == ossia::net::encoding::none)
    return data;
  QByteArray out(int(ossia::net::max_encoded_size(enc, data.size())), '\0');
  auto actual = ossia::net::encode_to(enc, data.data(), data.size(), out.data());
  out.resize(int(actual));
  return out;
}

inline QByteArray
apply_decoding(ossia::net::encoding enc, const unsigned char* data, std::size_t sz)
{
  if(enc == ossia::net::encoding::none)
    return QByteArray((const char*)data, sz);
  QByteArray out(int(ossia::net::max_decoded_size(enc, sz)), '\0');
  auto actual = ossia::net::decode_to(enc, (const char*)data, sz, out.data());
  out.resize(int(actual));
  return out;
}

inline QByteArray
apply_decoding(ossia::net::encoding enc, const char* data, std::size_t sz)
{
  if(enc == ossia::net::encoding::none)
    return QByteArray(data, sz);
  QByteArray out(int(ossia::net::max_decoded_size(enc, sz)), '\0');
  auto actual = ossia::net::decode_to(enc, data, sz, out.data());
  out.resize(int(actual));
  return out;
}

//! Appends one argument to an OSC message with the type the caller asked for.
/**
 * A JavaScript number carries no width and no integer-ness: `1`, `1.0` and
 * `2/2` are the same value, and QJSValue::toNumber() is the only honest
 * reading of it. So the *value* cannot say which OSC type tag belongs on the
 * wire -- only the protocol can, and the script is the only thing that knows
 * the protocol. This is where it says so.
 *
 * Supported tags are the ones oscpack can emit: i (int32), h (int64),
 * f (float32), d (float64), s (string), S (symbol), c (char), b (blob),
 * T / F (true / false, which carry no payload), I (impulse / infinitum).
 * Anything else -- including a tag string shorter than the argument list --
 * falls back to value_from_js(), i.e. the untyped default.
 *
 * \return false when \p tag was not one of the above.
 */
inline bool append_osc_argument(
    oscpack::OutboundPacketStream& p, char tag, const QJSValue& v)
{
  switch(tag)
  {
    case 'i':
      // ToInt32 semantics, as everywhere else a script hands QJSValue an int.
      p << (int32_t)v.toInt();
      return true;
    case 'h':
      p << (int64_t)v.toNumber();
      return true;
    case 'f':
      p << (float)v.toNumber();
      return true;
    case 'd':
      p << (double)v.toNumber();
      return true;
    case 's': {
      const auto str = v.toString().toUtf8();
      p << oscpack::string_view{str.constData(), (std::size_t)str.size()};
      return true;
    }
    case 'S': {
      const auto str = v.toString().toUtf8();
      p << oscpack::Symbol{str.constData()};
      return true;
    }
    case 'c': {
      const auto str = v.toString();
      p << (char)(str.isEmpty() ? '\0' : str[0].toLatin1());
      return true;
    }
    case 'b': {
      const auto blob = v.toVariant().toByteArray();
      p << oscpack::Blob{
          blob.constData(), (oscpack::osc_bundle_element_size_t)blob.size()};
      return true;
    }
    case 'T':
      p << true;
      return true;
    case 'F':
      p << false;
      return true;
    case 'I':
      p << oscpack::Infinitum();
      return true;
    default:
      return false;
  }
}

//! Encodes an OSC message whose argument types the script spelled out, and
//! hands the bytes to \p write.
/**
 * `sock.osc("/spat/serv", ["deg", 1, az, el, r, h, v], "sifffff")` -- the type
 * tag string of liblo's oscsend, and of OSC itself.
 *
 * Without it every number goes out as a float, because that is the only thing
 * a JavaScript number can be read as without guessing (see
 * append_osc_argument). Guessing is not an option here: inferring the tag from
 * the value would make the *shape* of the packet depend on the data, so
 * `/spat/serv deg 1 -90 0 1 0.4 0.6` would go out as "siiiiff" and the same
 * message a moment later, with the source off-axis, as "sifffff". A receiver
 * with a fixed grammar -- SpatGRIS's /spat/serv wants an int index and floats
 * after it -- would then work or not depending on where the source happens to
 * be, which is worse than always being wrong.
 *
 * A tag string shorter than the argument list, or carrying a tag this build
 * cannot emit, leaves those arguments to the untyped conversion rather than
 * dropping them.
 */
template <typename Writer>
void write_osc_message(
    Writer&& write, const QByteArray& address, const QJSValueList& values,
    const QByteArray& typetags)
{
  const std::string addr = address.toStdString();
  // No parameter and no unit here: this is a raw socket, not a device tree
  // address, so nothing dataspace-dependent can apply.
  const ossia::unit_t no_unit{};

  auto& pool = ossia::buffer_pool::instance();
  auto buf = pool.acquire();

  while(buf.size() < ossia::net::max_osc_message_size)
  {
    try
    {
      oscpack::OutboundPacketStream str{buf.data(), buf.size()};
      str << oscpack::BeginMessageN(addr);

      const ossia::net::osc_1_0_outbound_dynamic_policy untyped{{str, no_unit}};
      for(int i = 0; i < values.size(); i++)
      {
        const char tag = i < typetags.size() ? typetags[i] : '\0';
        if(!append_osc_argument(str, tag, values[i]))
          ossia::qt::value_from_js(values[i]).apply(untyped);
      }
      str << oscpack::EndMessage();

      write(str.Data(), str.Size());
      break;
    }
    catch(...)
    {
      // oscpack throws when the buffer is too small: grow and retry, as the
      // untyped list path does, and give up at max_osc_message_size.
      const auto n = buf.size();
      buf.clear();
      buf.resize(n * 2 + 1);
    }
  }

  pool.release(std::move(buf));
}

struct protocols_sender
{
  //! Sends an OSC message with the type tags the script asked for.
  //! See write_osc_message() for what the tag string means and why it exists.
  void send_osc(
      this auto&& self, QByteArray address, const QJSValueList& values,
      const QByteArray& typetags)
  {
    if(typetags.isEmpty())
      return self.send_osc(std::move(address), values);

    auto& sock = *self.socket;
    using socket_type = std::remove_cvref_t<decltype(sock)>;
    using writer_type = ossia::net::socket_writer<socket_type>;

    write_osc_message(writer_type{sock}, address, values, typetags);
  }

  void send_osc(this auto&& self, QByteArray address, const QJSValueList& values)
  {
    auto& sock = *self.socket;
    using socket_type = std::remove_cvref_t<decltype(sock)>;
    using writer_type = ossia::net::socket_writer<socket_type>;
    using send_visitor = ossia::net::osc_value_send_visitor<
        ossia::net::full_parameter_data, ossia::net::osc_1_0_policy, writer_type>;

    ossia::net::full_parameter_data p;
    const std::string addr = address.toStdString();

    switch(values.size())
    {
      case 0: {
        ossia::value{ossia::impulse{}}.apply(
            send_visitor{p, addr, writer_type{sock}});
        break;
      }
      case 1: {
        auto v = ossia::qt::value_from_js(values[0]);
        v.apply(send_visitor{p, addr, writer_type{sock}});
        break;
      }
      default: {
        std::vector<ossia::value> vec;
        vec.reserve(values.size());
        for(const auto& v : values)
          vec.push_back(ossia::qt::value_from_js(v));
        ossia::value vvec(std::move(vec));
        vvec.apply(send_visitor{p, addr, writer_type{sock}});
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
