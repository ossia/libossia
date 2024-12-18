#pragma once
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/writers.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/write.hpp>

#include <nano_signal_slot.hpp>
#if defined(__linux__)
#include <linux/serial.h>
#include <linux/tty_flags.h>
#include <sys/ioctl.h>

#include <termios.h>
#elif defined(__APPLE__)
#include <sys/ioctl.h>
#endif

namespace ossia::net
{

template <typename Framing>
class serial_socket
{
public:
  using proto = boost::asio::serial_port;
  using socket = boost::asio::serial_port;
  using encoder = typename Framing::template encoder<boost::asio::serial_port>;
  using decoder = typename Framing::template decoder<boost::asio::serial_port>;

  serial_socket(const serial_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_conf{std::move(conf)}
      , m_port{ctx}
      , m_encoder{this->m_port}
      , m_decoder{this->m_port}
  {
  }

  int set_custom_baud_rate(int rate)
  {
#if defined(__linux__)
    // https://stackoverflow.com/questions/9366249/boostasioserialport-alternative-that-supports-non-standard-baud-rates
    auto fd = m_port.native_handle();
    struct serial_struct serinfo;
    memset(&serinfo, 0, sizeof(serinfo));

    // Get the current serial status
    if(ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
      return -1;

    // Device can handle any rate e.g. it's using straight USB packetization
    if(serinfo.baud_base == 0)
      return 0;

    // Approximate the closest baud rate
    serinfo.flags &= ~ASYNC_SPD_MASK;
    serinfo.flags |= ASYNC_SPD_CUST;
    serinfo.custom_divisor = (serinfo.baud_base + (rate / 2)) / rate;
    if(serinfo.custom_divisor < 1)
    {
      serinfo.baud_base = rate;
      serinfo.custom_divisor = 1;
    }

    // Apply and check
    if(ioctl(fd, TIOCSSERIAL, &serinfo) < 0)
      return -1;
    if(ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
      return -1;

    fcntl(fd, F_SETFL, 0);

    // Apply terminal options
    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B38400);
    cfsetospeed(&options, B38400);
    cfmakeraw(&options);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CRTSCTS;
    if(tcsetattr(fd, TCSANOW, &options) != 0)
      return -1;

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) \
    || defined(__OpenBSD__) || defined(__DragonFly__)
    auto fd = m_port.native_handle();
#define IOSSIOSPEED _IOW('T', 2, speed_t)
    if(ioctl(fd, IOSSIOSPEED, &rate, 1) < 0)
      return -1;
#undef IOSSIOSPEED
#endif

    return 0;
  }

  void connect()
  {
    m_port.open(m_conf.port);

    try
    {
      m_port.set_option(proto::baud_rate(m_conf.baud_rate));
    }
    catch(...)
    {
      m_port.set_option(proto::baud_rate(38400));

      int new_baud = static_cast<int>(m_conf.baud_rate);
      set_custom_baud_rate(new_baud);
    }
    m_port.set_option(proto::character_size(m_conf.character_size));
    m_port.set_option(proto::flow_control(
        static_cast<proto::flow_control::type>(m_conf.flow_control)));
    m_port.set_option(proto::parity(static_cast<proto::parity::type>(m_conf.parity)));
    m_port.set_option(
        proto::stop_bits(static_cast<proto::stop_bits::type>(m_conf.stop_bits)));

    boost::asio::post(m_context, [this] { on_open(); });
  }

  template <typename F>
  void receive(F f)
  {
    struct proc : stream_processor<serial_socket, F>
    {
    };
    m_decoder.receive(proc{*this, std::move(f)});
  }

  void close()
  {
    if(m_port.is_open())
    {
      m_port.cancel();
      boost::asio::post(m_context, [this] {
        m_port.close();
        on_close();
      });
    }
  }

  void write(const char* data, std::size_t sz) { m_encoder.write(data, sz); }

  bool connected() const noexcept { return true; }

  Nano::Signal<void()> on_open;
  Nano::Signal<void()> on_close;
  Nano::Signal<void()> on_fail;

  boost::asio::io_context& m_context;
  serial_configuration m_conf;
  boost::asio::serial_port m_port;
  encoder m_encoder;
  decoder m_decoder;
};
}
