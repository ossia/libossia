#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/can_socket.hpp>

#if defined(__linux__)
#include <ossia-qt/protocols/utils.hpp>

#include <boost/asio/dispatch.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>
#include <QByteArray>
#include <QVariant>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{
/*
 var ifaces = Protocols.canInterfaces();

 var bus = Protocols.can({
   Transport: {
     Interface: "vcan0",
     FD: false,               // CAN FD: payloads of up to 64 bytes
     Loopback: true,          // let the other local sockets see our frames
     ReceiveOwnMessages: false,
     ErrorFrames: false       // deliver the kernel's bus error reports
   },
   // Omitting the mask means "exactly this id"; omitting Filters means
   // "every frame". Filters are per-socket, so several buses may share one
   // interface with different filters.
   Filters: [ { id: 0x123, mask: 0x7FF } ],
   onOpen: function(socket) { console.log("bus up"); },
   onMessage: function(frame) {
     console.log(frame.id.toString(16), frame.extended, frame.rtr, frame.fd, frame.bytes);
   },
   // Also called when a write fails, e.g. ENOBUFS when the interface's
   // transmit queue is full -- that frame was not sent.
   onError: function(e) { console.log("CAN error:", e); },
   onClose: function() { }
 });

 bus.write({ id: 0x123, bytes: [1, 2, 3, 4] });
 bus.write({ id: 0x18DAF110, extended: true, bytes: [0x02, 0x10, 0x01] });
 bus.write({ id: 0x456, fd: true, brs: true, bytes: [ ... up to 64 ... ] });
 // A remote transmission request carries no data, only a requested length:
 bus.write({ id: 0x321, rtr: true, length: 8 });
*/
class qml_can_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_can_socket)
public:
  struct state
  {
    ossia::net::can_socket socket;
    std::atomic_bool alive{true};

    state(const ossia::net::can_configuration& conf, boost::asio::io_context& ctx)
        : socket{conf, ctx}
    {
    }
  };

  //! Converts a received frame to a JS object and hands it to onMessage.
  struct receive_callback
  {
    std::shared_ptr<state> st;
    QPointer<qml_can_socket> self;

    void operator()(const ossia::net::can_message& msg) const
    {
      if(!st->alive)
        return;

      QVariantMap frame;
      frame["id"] = QVariant::fromValue(msg.id);
      frame["extended"] = msg.extended;
      frame["rtr"] = msg.remote;
      frame["fd"] = msg.fd;
      frame["error"] = msg.error;
      frame["brs"] = msg.bitrate_switch;
      frame["esi"] = msg.error_state;

      // A QByteArray reaches the script as an ArrayBuffer, the way the serial
      // protocol passes its payloads: one allocation instead of a QVariant per
      // byte, which matters at bus rates with 64-byte FD frames. Scripts read
      // it with `new Uint8Array(frame.bytes)`.
      //
      // An RTR frame requests data, it does not carry any: leave it empty
      // rather than exposing `size` bytes of zeroes.
      frame["bytes"] = msg.remote
                           ? QByteArray{}
                           : QByteArray{
                                 reinterpret_cast<const char*>(msg.data),
                                 qsizetype(msg.size)};
      frame["length"] = int(msg.size);

      ossia::qt::run_async(self.get(), [self = self, frame] {
        if(!self.get())
          return;
        if(self->onMessage.isCallable())
          if(auto engine = qjsEngine(self.get()))
            self->onMessage.call({engine->toScriptValue(frame)});
      }, Qt::AutoConnection);
    }
  };

  qml_can_socket() { }

  ~qml_can_socket()
  {
    if(m_state)
    {
      m_state->alive = false;
      close();
    }
  }

  bool isOpen() const noexcept { return m_state != nullptr; }

  void open(const ossia::net::can_configuration& conf, boost::asio::io_context& ctx)
  {
    m_state = std::make_shared<state>(conf, ctx);

    auto& sock = m_state->socket;
    sock.on_open.connect<&qml_can_socket::on_open>(this);
    sock.on_close.connect<&qml_can_socket::on_close>(this);
    sock.on_fail.connect<&qml_can_socket::on_fail>(this);
    sock.on_write_error.connect<&qml_can_socket::on_write_error>(this);

    try
    {
      sock.connect();
    }
    catch(...)
    {
      // Typically: the interface does not exist, or is not up. Leave the object
      // unusable and let the caller report it.
      m_state->alive = false;
      m_state.reset();
      throw;
    }
  }

  //! write({id, extended, rtr, fd, brs, bytes})
  void write(QVariant frame)
  {
    if(!m_state)
      return;

    const auto map = frame.toMap();
    ossia::net::can_message msg;
    msg.id = map["id"].toUInt();
    msg.extended = map["extended"].toBool();
    msg.remote = map["rtr"].toBool();
    msg.fd = map["fd"].toBool();
    msg.bitrate_switch = map["brs"].toBool();

    const auto max = msg.fd ? CANFD_MAX_DLEN : CAN_MAX_DLEN;
    int n = 0;
    const auto bytes = map["bytes"];
    // QML gives us either a JS array of numbers or, if the user built one, a
    // string / ArrayBuffer that Qt converts to a QByteArray.
    if(bytes.canConvert<QVariantList>() && bytes.typeId() != QMetaType::QByteArray)
    {
      for(const auto& b : bytes.toList())
      {
        if(n >= max)
          break;
        msg.data[n++] = uint8_t(b.toUInt() & 0xFF);
      }
    }
    else
    {
      const auto arr = bytes.toByteArray();
      for(char b : arr)
      {
        if(n >= max)
          break;
        msg.data[n++] = uint8_t(b);
      }
    }

    // For an RTR frame there is no payload, so `length` carries the requested
    // data length instead.
    if(auto len = map["length"]; len.isValid())
      msg.size = uint8_t(std::min(len.toInt(), max));
    else
      msg.size = uint8_t(n);

    auto st = m_state;
    boost::asio::dispatch(context(), [st, msg] {
      if(st->alive)
        st->socket.write(msg);
    });
  }
  W_SLOT(write)

  void close()
  {
    if(!m_state)
      return;
    auto st = m_state;
    boost::asio::dispatch(context(), [st] { st->socket.close(); });
  }
  W_SLOT(close)

  void on_open()
  {
    if(!m_state || !m_state->alive)
      return;

    auto self = QPointer{this};
    if(onMessage.isCallable())
      m_state->socket.receive(receive_callback{m_state, self});

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
      ossia::qt::run_async(this, [=, this] {
        onError.call({QStringLiteral("CAN socket failed")});
      }, Qt::AutoConnection);
  }

  //! A failed write. ENOBUFS is the interesting one: it means the netdev
  //! transmit queue is full (CAN interfaces often have txqueuelen 10) and the
  //! frame was *not* sent. It is reported rather than retried, as retrying at
  //! this level would reorder the bus traffic.
  void on_write_error(boost::system::error_code ec)
  {
    if(!m_state || !m_state->alive)
      return;
    if(!onError.isCallable())
      return;

    const auto err = QString::fromStdString(ec.message());
    ossia::qt::run_async(this, [self = QPointer{this}, err] {
      if(self && self->onError.isCallable())
        self->onError.call({QStringLiteral("write: ") + err});
    }, Qt::AutoConnection);
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

private:
  boost::asio::io_context& context() const noexcept { return m_state->socket.m_context; }

  std::shared_ptr<state> m_state;
};

}
#endif
