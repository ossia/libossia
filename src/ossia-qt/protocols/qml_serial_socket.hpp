#pragma once
#include <ossia/detail/variant.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/cobs_framing.hpp>
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/encoding.hpp>
#include <ossia/network/sockets/fixed_length_framing.hpp>
#include <ossia/network/sockets/line_framing.hpp>
#include <ossia/network/sockets/no_framing.hpp>
#include <ossia/network/sockets/serial_socket.hpp>
#include <ossia/network/sockets/size_prefix_framing.hpp>
#include <ossia/network/sockets/slip_framing.hpp>
#include <ossia/network/sockets/stx_etx_framing.hpp>
#include <ossia/network/sockets/var_size_prefix_framing.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>

#include <nano_observer.hpp>

#include <algorithm>
#include <mutex>
#include <verdigris>

namespace ossia::qt
{

class qml_serial_socket : public QObject
{
  W_OBJECT(qml_serial_socket)
public:
  // Unlike the TCP/UDP sockets, where the decoder is a separate object that can
  // be swapped after construction, a serial_socket owns its encoder and decoder:
  // the framing is therefore part of the socket's type and is chosen when the
  // port is opened.
  using socket_type = ossia::slow_variant<
      ossia::net::serial_socket<ossia::net::no_framing>,
      ossia::net::serial_socket<ossia::net::size_prefix_framing>,
      ossia::net::serial_socket<ossia::net::slip_framing>,
      ossia::net::serial_socket<ossia::net::line_framing>,
      ossia::net::serial_socket<ossia::net::cobs_framing>,
      ossia::net::serial_socket<ossia::net::stx_etx_framing>,
      ossia::net::serial_socket<ossia::net::size_prefix_1byte_framing>,
      ossia::net::serial_socket<ossia::net::size_prefix_2byte_be_framing>,
      ossia::net::serial_socket<ossia::net::size_prefix_2byte_le_framing>,
      ossia::net::serial_socket<ossia::net::size_prefix_4byte_le_framing>,
      ossia::net::serial_socket<ossia::net::fixed_length_framing>>;

  //! Owns the transport and every asio-side callback. As on the TCP outbound
  //! socket, the Nano::Observer lives here and not on the QObject: the signals
  //! are members of this very object, so their unsynchronized slot lists can
  //! only be mutated while the whole state is being destroyed, and the QObject
  //! - destroyed on the Qt thread while the asio thread may be firing - is
  //! only ever reached under qt_mutex.
  struct state : Nano::Observer
  {
    socket_type socket;
    std::atomic_bool alive{true};
    ossia::net::encoding enc{ossia::net::encoding::none};

    //! Guards `self`, which is cleared before the QObject starts dying. Held
    //! for the whole asio -> Qt handover.
    std::mutex qt_mutex;
    qml_serial_socket* self{};

    void detach()
    {
      std::lock_guard g{qt_mutex};
      self = nullptr;
    }

    template <typename F>
    void post_to_qt(F&& f)
    {
      std::lock_guard g{qt_mutex};
      if(self)
        ossia::qt::run_async(self, std::forward<F>(f), Qt::AutoConnection);
    }

    //! Nano slots, always on the asio thread.
    void fire_open();
    void fire_fail();
    void fire_close();

    state(
        const ossia::net::serial_configuration& conf, boost::asio::io_context& ctx,
        ossia::net::framing f, const std::string& delim, std::size_t frame_size,
        ossia::net::encoding e)
        // The alternatives hold references into their own serial_port, so the
        // variant must be built in place and never moved afterwards.
        : socket{make_socket(conf, ctx, f)}
        , enc{e}
    {
      switch(f)
      {
        case ossia::net::framing::line_delimiter: {
          char delimiter[8] = {};
          std::copy_n(delim.begin(), std::min(delim.size(), std::size_t(7)), delimiter);

          auto& sock = ossia::get<3>(socket);
          std::copy_n(delimiter, 8, sock.m_encoder.delimiter);
          std::copy_n(delimiter, 8, sock.m_decoder.delimiter);
          break;
        }
        case ossia::net::framing::fixed_length:
          if(frame_size > 0)
            ossia::get<10>(socket).m_decoder.frame_size = frame_size;
          break;
        default:
          break;
      }
    }

    static socket_type make_socket(
        const ossia::net::serial_configuration& conf, boost::asio::io_context& ctx,
        ossia::net::framing f)
    {
      switch(f)
      {
        default:
        case ossia::net::framing::none:
          return socket_type{ossia::in_place_index<0>, conf, ctx};
        case ossia::net::framing::size_prefix:
          return socket_type{ossia::in_place_index<1>, conf, ctx};
        case ossia::net::framing::slip:
          return socket_type{ossia::in_place_index<2>, conf, ctx};
        case ossia::net::framing::line_delimiter:
          return socket_type{ossia::in_place_index<3>, conf, ctx};
        case ossia::net::framing::cobs:
          return socket_type{ossia::in_place_index<4>, conf, ctx};
        case ossia::net::framing::stx_etx:
          return socket_type{ossia::in_place_index<5>, conf, ctx};
        case ossia::net::framing::size_prefix_1byte:
          return socket_type{ossia::in_place_index<6>, conf, ctx};
        case ossia::net::framing::size_prefix_2byte_be:
          return socket_type{ossia::in_place_index<7>, conf, ctx};
        case ossia::net::framing::size_prefix_2byte_le:
          return socket_type{ossia::in_place_index<8>, conf, ctx};
        case ossia::net::framing::size_prefix_4byte_le:
          return socket_type{ossia::in_place_index<9>, conf, ctx};
        case ossia::net::framing::fixed_length:
          return socket_type{ossia::in_place_index<10>, conf, ctx};
      }
    }
  };

  struct receive_callback
  {
    std::shared_ptr<state> st;
    QPointer<qml_serial_socket> self;
    // Points to onMessage or onBytes on the QObject; null when no callback is
    // set, the read loop then only keeping the port's error and close
    // notifications flowing. Dereferenced on the Qt thread only: a QJSValue
    // must not be touched from asio.
    QJSValue* target;

    void operator()(const unsigned char* data, std::size_t sz) const
    {
      if(!st->alive || !target)
        return;
      auto buf = apply_decoding(st->enc, data, sz);
      auto cb = target;
      st->post_to_qt([self = self, buf, cb] {
        if(!self.get())
          return;
        if(cb->isCallable())
        {
          if(auto engine = qjsEngine(self.get()))
            cb->call({engine->toScriptValue(buf)});
        }
      });
    }
  };

  qml_serial_socket() { }

  ~qml_serial_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      // No asio -> Qt call may start from here on.
      m_state->detach();
      close();
    }
  }

  bool isOpen() const noexcept { return m_state != nullptr; }

  void open(
      const ossia::net::serial_configuration& conf, boost::asio::io_context& ctx,
      ossia::net::framing f = ossia::net::framing::none, const std::string& delim = {},
      std::size_t frame_size = 0, ossia::net::encoding e = ossia::net::encoding::none)
  {
    // As on the other sockets, onBytes means "raw bytes, whatever the framing".
    // The framing cannot be changed once the port is open, so the choice has to
    // be made here rather than when the receive loop starts.
    if(!onMessage.isCallable() && onBytes.isCallable())
      f = ossia::net::framing::none;

    m_state = std::make_shared<state>(conf, ctx, f, delim, frame_size, e);
    m_state->self = this;

    ossia::visit([st = m_state.get()](auto& sock) {
      sock.on_open.template connect<&state::fire_open>(st);
      sock.on_close.template connect<&state::fire_close>(st);
      sock.on_fail.template connect<&state::fire_fail>(st);
    }, m_state->socket);

    try
    {
      ossia::visit([](auto& sock) { sock.connect(); }, m_state->socket);
    }
    catch(...)
    {
      // Opening a port that does not exist is the common failure here; leave
      // the object unusable and let the caller report it.
      m_state->alive = false;
      m_state->detach();
      m_state.reset();
      throw;
    }
  }

  void write(QByteArray buffer)
  {
    if(!m_state)
      return;
    auto st = m_state;
    if(st->enc != ossia::net::encoding::none)
      buffer = apply_encoding(st->enc, buffer);
    boost::asio::dispatch(context(), [st, buffer = std::move(buffer)] {
      if(st->alive)
        ossia::visit(
            [&](auto& sock) { sock.write(buffer.data(), buffer.size()); }, st->socket);
    });
  }
  W_SLOT(write)

  void close()
  {
    if(!m_state)
      return;
    auto st = m_state;
    boost::asio::dispatch(
        context(), [st] { ossia::visit([](auto& sock) { sock.close(); }, st->socket); });
  }
  W_SLOT(close)

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

  void on_open()
  {
    if(!m_state || !m_state->alive)
      return;

    auto st = m_state;
    auto self = QPointer{this};
    // The read loop is always armed: it is what makes the port's own stream
    // notifications (errors, close) observable, even with no callback set.
    // Which decoder runs was decided in open(): a serial_socket owns its
    // framing.
    QJSValue* target = onMessage.isCallable() ? &self.data()->onMessage
                       : onBytes.isCallable() ? &self.data()->onBytes
                                              : nullptr;
    ossia::visit([cb = receive_callback{st, self, target}](auto& sock) mutable {
      sock.receive(std::move(cb));
    }, st->socket);

    if(onOpen.isCallable())
      ossia::qt::run_async(this, [=, this] {
        onOpen.call({qjsEngine(this)->newQObject(this)});
      }, Qt::AutoConnection);
  }

  void on_fail()
  {
    if(!m_state || !m_state->alive)
      return;
    if(onError.isCallable())
      ossia::qt::run_async(this, [=, this] { onError.call(); }, Qt::AutoConnection);
  }

  void on_close()
  {
    if(!m_state || !m_state->alive)
      return;
    if(onClose.isCallable())
      ossia::qt::run_async(this, [=, this] { onClose.call(); }, Qt::AutoConnection);
  }

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;
  QJSValue onBytes; // raw bytes, ignores Framing (as on the other sockets)

private:
  boost::asio::io_context& context() const noexcept
  {
    return ossia::visit([](auto& sock) -> boost::asio::io_context& {
      return sock.m_context;
    }, m_state->socket);
  }

  std::shared_ptr<state> m_state;
};

inline void qml_serial_socket::state::fire_open()
{
  std::lock_guard g{qt_mutex};
  if(self)
    self->on_open();
}

inline void qml_serial_socket::state::fire_fail()
{
  std::lock_guard g{qt_mutex};
  if(self)
    self->on_fail();
}

inline void qml_serial_socket::state::fire_close()
{
  std::lock_guard g{qt_mutex};
  if(self)
    self->on_close();
}

}
