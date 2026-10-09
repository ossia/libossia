#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/detail/thread.hpp>

#include <oscpack/ip/UdpSocket.h>
#include <oscpack/osc/OscDebug.h>
#include <oscpack/osc/OscPacketListener.h>

#include <functional>
#include <memory>
#include <sstream>
#include <thread>

namespace oscpack
{

namespace detail
{

template <typename Impl_T>
struct ClearListener : public oscpack::TimerListener
{
  ClearListener(UdpSocket<Impl_T>& s)
      : socket{s}
  {
  }
  UdpSocket<Impl_T>& socket;

  void TimerExpired() override { socket.AsynchronousBreak(); }
};

template <typename Impl_T>
class ReceiveSocket : public UdpSocket<Impl_T>
{
  SocketReceiveMultiplexer<Impl_T> mux_;
  PacketListener* listener_;

public:
  ReceiveSocket(const IpEndpointName& localEndpoint, PacketListener* listener)
      : listener_(listener)
  {
    this->Bind(localEndpoint);
    mux_.AttachSocketListener(&this->impl_, listener_);
  }

  ~ReceiveSocket() { mux_.DetachSocketListener(&this->impl_, listener_); }

  // see SocketReceiveMultiplexer above for the behaviour of these methods...
  void Run() { mux_.Run(); }
  void Break()
  {
    ClearListener<Impl_T> l{*this};
    mux_.AttachPeriodicTimerListener(0, &l);
    mux_.Break();
  }
  void AsynchronousBreak()
  {
    ClearListener<Impl_T> l{*this};
    mux_.AttachPeriodicTimerListener(0, &l);
    mux_.AsynchronousBreak();
  }
};
}
class ReceiveSocket
    : public detail::UdpListeningReceiveSocket<detail::Implementation>
{
public:
  using UdpListeningReceiveSocket::UdpListeningReceiveSocket;

  //! The port the socket is bound to: the one the system picked when it was
  //! bound to port 0.
  unsigned int BoundPort()
  {
    sockaddr_in addr{};
#if defined(_WIN32)
    int len = sizeof(addr);
#else
    socklen_t len = sizeof(addr);
#endif
    if(getsockname(this->impl_.Socket(), reinterpret_cast<sockaddr*>(&addr), &len) != 0)
      return 0;
    return ntohs(addr.sin_port);
  }
};
}
namespace osc
{

template <typename MessageHandler>
/**
 * @brief The listener class
 *
 * Listens to OSC messages and handles them.
 */
class listener final : public oscpack::OscPacketListener
{
public:
  listener(MessageHandler msg)
      : m_messageHandler{msg}
  {
  }

  void ProcessMessage(
      const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip) override
  {
    try
    {
      m_messageHandler(m, ip);
    }
    catch(std::exception& e)
    {
      std::stringstream s;
      oscpack::debug(s, m);

      ossia::logger().error(
          "osc::listener::ProcessMessage error: '{}': {}", s.str(), e.what());
    }
    catch(...)
    {
      std::stringstream s;
      oscpack::debug(s, m);
      ossia::logger().error("osc::listener::ProcessMessage error: '{}'", s.str());
    }
  }

  void ProcessPacket(
      const char* data, int size, const oscpack::IpEndpointName& remoteEndpoint) override
  {
    try
    {
      oscpack::ReceivedPacket p(data, size);
      if(p.IsBundle())
        this->ProcessBundle(oscpack::ReceivedBundle(p), remoteEndpoint);
      else
        this->ProcessMessage(oscpack::ReceivedMessage(p), remoteEndpoint);
    }
    catch(std::exception& e)
    {
      ossia::logger().error("osc::listener::ProcessPacket error: {}", e.what());
    }
    catch(...)
    {
      ossia::logger().error("osc::listener::ProcessPacket error");
    }
  }

private:
  MessageHandler m_messageHandler;
};

/**
 * @brief The receiver class
 *
 * A OSC server.
 * Note : if a port cannot be opened, it will be incremented.
 */
class receiver
{
public:
  template <typename Handler>
  receiver(unsigned int port, Handler msg)
      : m_impl{std::make_unique<listener<Handler>>(msg)}
  {
    setPort(port);
  }

  receiver() = default;
  receiver(receiver&& other) noexcept
  {
    other.stop();
    m_impl = std::move(other.m_impl);
    m_socket = std::move(other.m_socket);
    setPort(other.m_port);
  }

  receiver& operator=(receiver&& other) noexcept
  {
    stop();

    m_impl = std::move(other.m_impl);
    m_socket = std::move(other.m_socket);

    setPort(other.m_port);

    return *this;
  }

  ~receiver() { stop(); }

  void run()
  {
    if(m_runThread.joinable())
      stop();

    m_runThread = ossia::thread([this] {
      ossia::set_thread_name("ossia osc");
      run_impl();
    });
    while(!m_running)
      std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  void run_impl()
  {
    m_running = true;
    // Retried while running only: once stop() has begun, a failure is the
    // socket being shut down under the loop.
    while(m_running)
    {
      try
      {
        m_socket->Run();
        return;
      }
      catch(...)
      {
      }
    }
  }

  void stop()
  {
    m_running = false;
    if(m_socket)
    {
      if(m_runThread.joinable())
      {
        // The packet is a fallback wake-up only: the break pipe already ends
        // the loop, and sending can fail (macOS refuses 127.0.0.1:0).
        try
        {
          oscpack::UdpTransmitSocket send_socket(
              oscpack::IpEndpointName("127.0.0.1", port()));
          send_socket.Send("__stop_", 8);
        }
        catch(...)
        {
        }
        m_socket->AsynchronousBreak();

        try
        {
          m_runThread.join();
        }
        catch(...)
        {
          // The thread may still be in Run(): leak the socket rather than
          // free it under the thread.
          m_runThread.detach();
          (void)m_socket.release();
          return;
        }
      }

      m_socket.reset();
    }
    else
    {
      if(m_runThread.joinable())
      {
        // Error somewhere: the thread is joinable, but there's no socket...
        m_runThread.detach();
      }
    }
  }

  unsigned int port() const { return m_port; }

  unsigned int setPort(unsigned int port)
  {
    m_port = port;

    bool ok = false;
    while(!ok)
    {
      try
      {
        m_socket = std::make_unique<oscpack::ReceiveSocket>(
            oscpack::IpEndpointName(oscpack::IpEndpointName::ANY_ADDRESS, m_port),
            m_impl.get());
        if(m_port == 0)
          m_port = m_socket->BoundPort();
        ok = true;
      }
      catch(std::runtime_error&)
      {
        m_port++;
      }
    }

    return m_port;
  }

private:
  unsigned int m_port = 0;
  std::unique_ptr<oscpack::OscPacketListener> m_impl;
  std::unique_ptr<oscpack::ReceiveSocket> m_socket;

  ossia::thread m_runThread;
  std::atomic_bool m_running = false;
};
}
