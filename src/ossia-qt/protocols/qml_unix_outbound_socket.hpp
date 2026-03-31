#pragma once
#include <ossia/network/context.hpp>
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
#include <ossia/detail/variant.hpp>
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/cobs_framing.hpp>
#include <ossia/network/sockets/fixed_length_framing.hpp>
#include <ossia/network/sockets/line_framing.hpp>
#include <ossia/network/sockets/no_framing.hpp>
#include <ossia/network/sockets/size_prefix_framing.hpp>
#include <ossia/network/sockets/slip_framing.hpp>
#include <ossia/network/sockets/stx_etx_framing.hpp>
#include <ossia/network/sockets/unix_socket.hpp>
#include <ossia/network/sockets/var_size_prefix_framing.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{
class qml_unix_datagram_outbound_socket
    : public QObject
    , public Nano::Observer
    , public protocols_sender
{
  W_OBJECT(qml_unix_datagram_outbound_socket)
public:
  struct state
  {
    ossia::net::unix_datagram_socket socket;
    std::atomic_bool alive{true};
    ossia::net::encoding enc{ossia::net::encoding::none};

    state(
        const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx,
        ossia::net::encoding e = ossia::net::encoding::none)
        : socket{conf, ctx}
    {
      enc = e;
    }
  };

  ossia::net::unix_datagram_socket* socket = nullptr;

  qml_unix_datagram_outbound_socket() { }

  ~qml_unix_datagram_outbound_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      close();
    }
  }

  bool isOpen() const noexcept { return m_state != nullptr; }

  void open(
      const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx,
      ossia::net::encoding e = ossia::net::encoding::none)
  {
    m_state = std::make_shared<state>(conf, ctx, e);
    socket = &m_state->socket;

    if(onClose.isCallable())
      m_state->socket.on_close.connect<&qml_unix_datagram_outbound_socket::on_close>(this);

    m_state->socket.connect();

    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void close()
  {
    if(!m_state)
      return;
    if(!m_state->socket.m_socket.is_open())
      return;
    auto st = m_state;
    boost::asio::post(st->socket.m_context, [st] {
      try
      {
        st->socket.m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
      }
      catch(...)
      {
      }
      st->socket.m_socket.close();
      st->socket.on_close();
    });
  }
  W_SLOT(close)

  void on_close()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  void write(QByteArray buffer)
  {
    if(!m_state)
      return;
    auto st = m_state;
    if(st->enc != ossia::net::encoding::none)
      buffer = apply_encoding(st->enc, buffer);
    boost::asio::dispatch(st->socket.m_context, [st, buffer = std::move(buffer)] {
      if(st->alive)
        st->socket.write(buffer.data(), buffer.size());
    });
  }
  W_SLOT(write)

  void osc(QByteArray address, QJSValueList values)
  {
    if(socket)
      this->send_osc(address, values);
  }
  W_SLOT(osc)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;

private:
  std::shared_ptr<state> m_state;
};

class qml_unix_stream_outbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_unix_stream_outbound_socket)
public:
  using socket_t = boost::asio::local::stream_protocol::socket;
  using decoder_type = ossia::slow_variant<
      ossia::net::no_framing::decoder<socket_t>,
      ossia::net::slip_decoder<socket_t>,
      ossia::net::size_prefix_decoder<socket_t>,
      ossia::net::line_framing_decoder<socket_t>,
      ossia::net::cobs_decoder<socket_t>,
      ossia::net::stx_etx_framing::decoder<socket_t>,
      ossia::net::size_prefix_1byte_framing::decoder<socket_t>,
      ossia::net::size_prefix_2byte_be_framing::decoder<socket_t>,
      ossia::net::size_prefix_2byte_le_framing::decoder<socket_t>,
      ossia::net::size_prefix_4byte_le_framing::decoder<socket_t>,
      ossia::net::fixed_length_decoder<socket_t>>;

  struct state
  {
    ossia::net::unix_stream_client socket;
    std::atomic_bool alive{true};
    ossia::net::framing framing{ossia::net::framing::none};
    ossia::net::encoding enc{ossia::net::encoding::none};
    char line_delimiter[8] = {};
    decoder_type decoder;

    state(
        const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx,
        ossia::net::framing f = ossia::net::framing::none,
        const std::string& delim = {},
        ossia::net::encoding e = ossia::net::encoding::none)
        : socket{conf, ctx}
        , decoder{ossia::in_place_index<0>, socket.m_socket}
    {
      framing = f;
      enc = e;
      if(!delim.empty())
      {
        auto sz = std::min(delim.size(), (size_t)7);
        std::copy_n(delim.begin(), sz, line_delimiter);
      }
      switch(f)
      {
        default:
        case ossia::net::framing::none:
          break;
        case ossia::net::framing::slip:
          decoder.template emplace<1>(socket.m_socket);
          break;
        case ossia::net::framing::size_prefix:
          decoder.template emplace<2>(socket.m_socket);
          break;
        case ossia::net::framing::line_delimiter:
          decoder.template emplace<3>(socket.m_socket);
          {
            auto& dec = ossia::get<3>(decoder);
            std::copy_n(line_delimiter, 8, dec.delimiter);
          }
          break;
        case ossia::net::framing::cobs:
          decoder.template emplace<4>(socket.m_socket);
          break;
        case ossia::net::framing::stx_etx:
          decoder.template emplace<5>(socket.m_socket);
          break;
        case ossia::net::framing::size_prefix_1byte:
          decoder.template emplace<6>(socket.m_socket);
          break;
        case ossia::net::framing::size_prefix_2byte_be:
          decoder.template emplace<7>(socket.m_socket);
          break;
        case ossia::net::framing::size_prefix_2byte_le:
          decoder.template emplace<8>(socket.m_socket);
          break;
        case ossia::net::framing::size_prefix_4byte_le:
          decoder.template emplace<9>(socket.m_socket);
          break;
        case ossia::net::framing::fixed_length:
          decoder.template emplace<10>(socket.m_socket);
          if(!delim.empty())
            ossia::get<10>(decoder).frame_size = std::stoul(delim);
          break;
      }
    }

    void write_encoded(const char* data, std::size_t sz)
    {
      switch(framing)
      {
        default:
        case ossia::net::framing::none:
          socket.write(data, sz);
          break;
        case ossia::net::framing::slip:
          ossia::net::slip_encoder<socket_t>{socket.m_socket}.write(data, sz);
          break;
        case ossia::net::framing::size_prefix:
          ossia::net::size_prefix_encoder<socket_t>{socket.m_socket}.write(data, sz);
          break;
        case ossia::net::framing::line_delimiter: {
          ossia::net::line_framing_encoder<socket_t> enc{socket.m_socket};
          std::copy_n(line_delimiter, 8, enc.delimiter);
          enc.write(data, sz);
          break;
        }
        case ossia::net::framing::cobs:
          ossia::net::cobs_encoder<socket_t>{socket.m_socket}.write(data, sz);
          break;
        case ossia::net::framing::stx_etx:
          ossia::net::stx_etx_framing::encoder<socket_t>{socket.m_socket}.write(data, sz);
          break;
        case ossia::net::framing::size_prefix_1byte:
          ossia::net::size_prefix_1byte_framing::encoder<socket_t>{socket.m_socket}.write(
              data, sz);
          break;
        case ossia::net::framing::size_prefix_2byte_be:
          ossia::net::size_prefix_2byte_be_framing::encoder<socket_t>{socket.m_socket}
              .write(data, sz);
          break;
        case ossia::net::framing::size_prefix_2byte_le:
          ossia::net::size_prefix_2byte_le_framing::encoder<socket_t>{socket.m_socket}
              .write(data, sz);
          break;
        case ossia::net::framing::size_prefix_4byte_le:
          ossia::net::size_prefix_4byte_le_framing::encoder<socket_t>{socket.m_socket}
              .write(data, sz);
          break;
        case ossia::net::framing::fixed_length:
          ossia::net::fixed_length_encoder<socket_t>{socket.m_socket}.write(data, sz);
          break;
      }
    }
  };

  struct receive_callback
  {
    std::shared_ptr<state> st;
    QPointer<qml_unix_stream_outbound_socket> self;
    QJSValue* target;

    void operator()(const unsigned char* data, std::size_t sz) const
    {
      if(!st->alive)
        return;
      auto buf = apply_decoding(st->enc, data, sz);
      auto cb = target;
      ossia::qt::run_async(
          self.get(),
          [self = self, buf, cb] {
        if(!self.get())
          return;
        if(cb->isCallable())
        {
          auto engine = qjsEngine(self.get());
          if(engine)
            cb->call({engine->toScriptValue(buf)});
        }
      },
          Qt::AutoConnection);
    }

    bool validate_stream(boost::system::error_code ec) const
    {
      if(ec == boost::asio::error::operation_aborted)
        return false;
      if(ec == boost::asio::error::eof)
        return false;
      return true;
    }
  };

  qml_unix_stream_outbound_socket() { }

  ~qml_unix_stream_outbound_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      close();
    }
  }

  bool isOpen() const noexcept { return m_state != nullptr; }

  void open(
      const ossia::net::fd_configuration& conf, boost::asio::io_context& ctx,
      ossia::net::framing f = ossia::net::framing::none,
      const std::string& delim = {},
      ossia::net::encoding e = ossia::net::encoding::none)
  {
    m_state = std::make_shared<state>(conf, ctx, f, delim, e);

    try
    {
      if(onOpen.isCallable())
        m_state->socket.on_open.connect<&qml_unix_stream_outbound_socket::on_open>(this);
      if(onClose.isCallable())
        m_state->socket.on_close.connect<&qml_unix_stream_outbound_socket::on_close>(
            this);
      if(onError.isCallable())
        m_state->socket.on_fail.connect<&qml_unix_stream_outbound_socket::on_fail>(this);
      m_state->socket.connect();
    }
    catch(const std::exception& e)
    {
      if(onError.isCallable())
      {
        onError.call({QString::fromStdString(e.what())});
      }
    }
  }

  void write(QByteArray buffer)
  {
    if(!m_state)
      return;
    auto st = m_state;
    if(st->enc != ossia::net::encoding::none)
      buffer = apply_encoding(st->enc, buffer);
    boost::asio::dispatch(st->socket.m_context, [st, buffer = std::move(buffer)] {
      if(st->alive)
        st->write_encoded(buffer.data(), buffer.size());
    });
  }
  W_SLOT(write)

  void close()
  {
    if(!m_state)
      return;
    auto st = m_state;
    boost::asio::post(st->socket.m_context, [st] {
      try
      {
        st->socket.m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
      }
      catch(...)
      {
      }
      st->socket.m_socket.close();
      st->socket.on_close();
    });
  }
  W_SLOT(close)

  void on_open()
  {
    if(!m_state || !m_state->alive)
      return;

    auto st = m_state;
    auto self = QPointer{this};
    if(onMessage.isCallable())
    {
      ossia::visit(
          [cb = receive_callback{st, self, &self.data()->onMessage}](
              auto& decoder) mutable { decoder.receive(std::move(cb)); },
          st->decoder);
    }
    else if(onBytes.isCallable())
    {
      st->decoder.template emplace<0>(st->socket.m_socket);
      ossia::get<0>(st->decoder)
          .receive(receive_callback{st, self, &self.data()->onBytes});
    }

    ossia::qt::run_async(
        this, [=, this] { onOpen.call({qjsEngine(this)->newQObject(this)}); },
        Qt::AutoConnection);
  }
  void on_fail()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onError.call(); }, Qt::AutoConnection);
  }
  void on_close()
  {
    if(!m_state || !m_state->alive)
      return;
    ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  void osc(QByteArray address, QJSValueList values)
  {
    if(!m_state)
      return;
    QByteArray packet;
    buffer_writer bw{packet};
    using send_visitor = ossia::net::osc_value_send_visitor<
        ossia::net::full_parameter_data, ossia::net::osc_1_0_policy, buffer_writer>;

    ossia::net::full_parameter_data p;
    const std::string addr = address.toStdString();

    switch(values.size())
    {
      case 0: {
        ossia::value{ossia::impulse{}}.apply(send_visitor{p, addr, bw});
        break;
      }
      case 1: {
        auto v = ossia::qt::value_from_js(values[0]);
        v.apply(send_visitor{p, addr, bw});
        break;
      }
      default: {
        std::vector<ossia::value> vec;
        vec.reserve(values.size());
        for(const auto& v : values)
          vec.push_back(ossia::qt::value_from_js(v));
        ossia::value vvec(std::move(vec));
        vvec.apply(send_visitor{p, addr, bw});
      }
    }
    write(packet);
  }
  W_SLOT(osc)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;
  QJSValue onBytes; // raw bytes, ignores Framing (backward compatible)

private:
  std::shared_ptr<state> m_state;
};

}
#endif
