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
 var devices = Protocols.inboundMIDIDevices();
 JSON.stringify(devices[0]);
 var midi = Protocols.inboundMIDI({
    Transport: { PortName: "My MIDI Controller" },
    onOpen: function(socket) {
      console.log("MIDI port opened!");
    },
    onMessage: function(msg) {
      console.log("MIDI message:", msg.bytes);
    },
    onError: function(error) {
      console.log("MIDI error:", error);
    }
  });
*/
class qml_midi_inbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_midi_inbound_socket)
public:
  struct state
  {
    std::atomic_bool alive{true};
  };

  qml_midi_inbound_socket()
      : m_state{std::make_shared<state>()}
  {
  }

  ~qml_midi_inbound_socket()
  {
    m_state->alive = false;
    close();
  }

  void open(const libremidi::port_information& pi)
  {
    try
    {
      // Configure MIDI input
      auto st = m_state;
      auto self = QPointer{this};

      libremidi::input_configuration config;
      config.on_message
          = [st, self](const libremidi::message& message) {
        if(!st->alive)
          return;

        // Convert MIDI message to QVariantMap for QML
        QVariantMap msg;
        msg["timestamp"] = QVariant::fromValue(message.timestamp);

        // Convert bytes to QVariantList
        QVariantList bytes;
        for(const auto& byte : message.bytes)
        {
          bytes.append(static_cast<int>(byte));
        }
        msg["bytes"] = bytes;

        // Send to QML in Qt thread
        ossia::qt::run_async(
            self.get(),
            [self, msg]() {
          if(!self.get())
            return;
          if(self->onMessage.isCallable())
          {
            self->onMessage.call({qjsEngine(self.get())->toScriptValue(msg)});
          }
        },
            Qt::AutoConnection);
      };
      config.ignore_sysex = false;
      config.ignore_timing = false;
      config.ignore_sensing = true;

      // Create MIDI input
      m_midi_in = std::make_unique<libremidi::midi_in>(config);

      // Open the port
      if(auto err = m_midi_in->open_port(static_cast<const libremidi::input_port&>(pi));
         err != stdx::error{})
      {
        if(onError.isCallable())
        {
          const auto& msg = err.message();
          onError.call({QString::fromUtf8(msg.data(), msg.size())});
        }
        return;
      }

      m_is_open = true;

      // Call onOpen callback
      if(onOpen.isCallable())
      {
        onOpen.call({qjsEngine(this)->newQObject(this)});
      }
    }
    catch(const std::exception& e)
    {
      if(onError.isCallable())
      {
        onError.call({QString::fromUtf8(e.what())});
      }
    }
  }

  void open()
  {
    try
    {
      // Create MIDI observer to enumerate ports
      m_observer = std::make_unique<libremidi::observer>();

      // Get available MIDI input ports
      auto ports = m_observer->get_input_ports();

      if(ports.empty())
      {
        if(onError.isCallable())
        {
          onError.call({"No MIDI input ports available"});
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
        onError.call({QString::fromUtf8(e.what())});
      }
    }
  }

  void close()
  {
    if(m_is_open)
    {
      m_is_open = false;
      m_midi_in.reset();
      m_observer.reset();

      if(onClose.isCallable())
      {
        onClose.call();
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
  std::shared_ptr<state> m_state;
  std::unique_ptr<libremidi::observer> m_observer;
  std::unique_ptr<libremidi::midi_in> m_midi_in;
  std::atomic_bool m_is_open{false};
};

}
