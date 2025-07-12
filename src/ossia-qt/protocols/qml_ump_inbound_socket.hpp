#pragma once
#include <ossia/network/context.hpp>

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>
#include <QVariant>

#include <libremidi/libremidi.hpp>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{
/*
 var devices = Protocols.inboundUMPDevices();
 var ump = Protocols.inboundUMP({
    Transport: devices[0],
    onOpen: function(socket) {
      console.log("UMP port opened!");
    },
    onMessage: function(msg) {
      console.log("UMP message:", msg);
      // msg contains parsed UMP data
    },
    onError: function(error) {
      console.log("UMP error:", error);
    }
  });
*/
class qml_ump_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_ump_inbound_socket)
public:
  qml_ump_inbound_socket() { }

  ~qml_ump_inbound_socket() { close(); }

  void open(const libremidi::port_information& pi)
  {
    try
    {
      // Configure UMP input
      libremidi::ump_input_configuration config;
      config.on_message
          = [this](const libremidi::ump& message) { handleUMPMessage(message); };

      // Create UMP input with MIDI 2.0 API
      m_ump_in = std::make_unique<libremidi::midi_in>(
          config, libremidi::midi2::default_api());

      // Open the port
      if(auto err = m_ump_in->open_port(static_cast<const libremidi::input_port&>(pi));
         err != stdx::error{})
      {
        if(onError.isCallable())
        {
          const auto& msg = err.message();
          run_on_qt_thread(
              { onError.call({QString::fromUtf8(msg.data(), msg.size())}); });
        }
        return;
      }

      m_is_open = true;

      // Call onOpen callback
      if(onOpen.isCallable())
      {
        run_on_qt_thread({ onOpen.call({qjsEngine(this)->newQObject(this)}); });
      }
    }
    catch(const std::exception& e)
    {
      if(onError.isCallable())
      {
        run_on_qt_thread({ onError.call({QString::fromUtf8(e.what())}); });
      }
    }
  }

  void open()
  {
    try
    {
      // Create UMP observer to enumerate ports
      m_observer = std::make_unique<libremidi::observer>(
          libremidi::observer_configuration{}, libremidi::midi2::default_api());

      // Get available UMP input ports
      auto ports = m_observer->get_input_ports();

      if(ports.empty())
      {
        if(onError.isCallable())
        {
          run_on_qt_thread({ onError.call({"No UMP input ports available"}); });
        }
        return;
      }

      // Default to first available port
      open(ports[0]);
    }
    catch(const std::exception& e)
    {
      if(onError.isCallable())
      {
        run_on_qt_thread({ onError.call({QString::fromUtf8(e.what())}); });
      }
    }
  }

  void close()
  {
    if(m_is_open)
    {
      m_is_open = false;
      m_ump_in.reset();
      m_observer.reset();

      if(onClose.isCallable())
      {
        run_on_qt_thread({ onClose.call(); });
      }
    }
  }
  W_SLOT(close)

  // Callbacks
  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;

private:
  void handleUMPMessage(const libremidi::ump& message)
  {
    if(!onMessage.isCallable())
      return;

    // Convert UMP message to QVariantMap for QML
    QVariantMap msg;
    msg["timestamp"] = QVariant::fromValue(message.timestamp);

    // Convert UMP data (4 32-bit words)
    QVariantList words;
    for(int i = 0; i < 4; ++i)
    {
      words.append(static_cast<quint32>(message.data[i]));
    }
    msg["words"] = words;

    // Send to QML in Qt thread
    ossia::qt::run_async(this, [this, msg]() {
      if(onMessage.isCallable())
      {
        onMessage.call({qjsEngine(this)->toScriptValue(msg)});
      }
    }, Qt::AutoConnection);
  }

  std::unique_ptr<libremidi::observer> m_observer;
  std::unique_ptr<libremidi::midi_in> m_ump_in;
  std::atomic_bool m_is_open{false};
};

}
