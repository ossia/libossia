#pragma once
#include <oscpack/osc/OscDebug.h>
#include <oscpack/ip/UdpSocket.h>
#include <oscpack/osc/OscPacketListener.h>
#include <memory>
#include <thread>
#include <functional>
#include <iostream>

namespace impl
{
namespace osc
{
template<typename MessageHandler>
/**
 * @brief The listener class
 *
 * Listens to OSC messages and handles them.
 */
class listener: public oscpack::OscPacketListener
{
  public:
    listener(MessageHandler msg):
      m_messageHandler{msg}
    {
    }

  protected:
    void ProcessMessage(
        const oscpack::ReceivedMessage& m,
        const oscpack::IpEndpointName& ip) override
    {
      try
      {
        m_messageHandler(m, ip);
      }
      catch( std::exception& e )
      {
        std::cerr << "OSC Parse Error: '";
        oscpack::debug(std::cerr, m);
        std::cerr << "'" << e.what() << std::endl;
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
    receiver() = default;
    receiver(receiver&& other)
    {
      other.stop();
      m_impl = std::move(other.m_impl);
      setPort(other.m_port);
    }

    template<typename Handler>
    receiver(unsigned int port, Handler msg):
      m_impl{std::make_unique<listener<Handler> >(msg)}
    {
      setPort(port);
    }

    receiver& operator=(receiver&& other)
    {
      stop();

      m_socket = std::move(other.m_socket);
      m_impl = std::move(other.m_impl);

      return *this;
    }

    ~receiver()
    {
      stop();
    }

    void run()
    {
      m_runThread = std::thread(&oscpack::UdpListeningReceiveSocket::Run, m_socket.get());
    }

    void stop()
    {
      if(m_socket)
        m_socket->AsynchronousBreak();

      std::this_thread::sleep_for(std::chrono::milliseconds(50));

      if(m_runThread.joinable())
        m_runThread.detach(); // TODO why not join()?

      m_socket.reset();
    }

    unsigned int port() const
    {
      return m_port;
    }

    unsigned int setPort(unsigned int port)
    {
      m_port = port;

      bool ok = false;
      while(!ok)
      {
        try
        {
          m_socket = std::make_unique<oscpack::UdpListeningReceiveSocket>
                     (oscpack::IpEndpointName(oscpack::IpEndpointName::ANY_ADDRESS, m_port),
                      m_impl.get());
          ok = true;
        }
        catch(std::runtime_error& e)
        {
          m_port++;
        }
      }

      return m_port;
    }

  private:
    unsigned int m_port = 0;
    std::unique_ptr<oscpack::UdpListeningReceiveSocket> m_socket;
    std::unique_ptr<oscpack::OscPacketListener> m_impl;

    std::thread m_runThread;
};
}
}
