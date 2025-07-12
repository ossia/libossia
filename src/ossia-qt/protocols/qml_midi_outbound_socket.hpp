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
 var devices = Protocols.outboundMIDIDevices();
 var midi = Protocols.outboundMIDI({
    Transport: devices[0],
    onOpen: function(socket) {
      console.log("MIDI port opened!");
      socket.sendNoteOn(1, 60, 100);  // channel 1, middle C, velocity 100
      socket.sendNoteOff(1, 60, 0);   // channel 1, middle C, velocity 0
      socket.sendControlChange(1, 7, 64); // channel 1, volume, value 64
      socket.sendMessage([0x90, 60, 100]); // raw MIDI bytes
    },
    onError: function(error) {
      console.log("MIDI error:", error);
    }
  });
*/
class qml_midi_outbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_midi_outbound_socket)
public:
  qml_midi_outbound_socket() { }

  ~qml_midi_outbound_socket() { close(); }

  void open(const libremidi::port_information& pi)
  {
    try
    {
      // Configure MIDI output
      libremidi::output_configuration config;

      // Create MIDI output
      m_midi_out = std::make_unique<libremidi::midi_out>(config);

      // Open the port
      if(auto err = m_midi_out->open_port(static_cast<const libremidi::output_port&>(pi));
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
      // Create MIDI observer to enumerate ports
      m_observer = std::make_unique<libremidi::observer>();

      // Get available MIDI output ports
      auto ports = m_observer->get_output_ports();

      if(ports.empty())
      {
        if(onError.isCallable())
        {
          run_on_qt_thread({ onError.call({"No MIDI output ports available"}); });
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
      m_midi_out.reset();
      m_observer.reset();

      if(onClose.isCallable())
      {
        run_on_qt_thread({ onClose.call(); });
      }
    }
  }
  W_SLOT(close)

  // Send raw MIDI message
  void sendMessage(QVariantList bytes)
  {
    if(!m_is_open || !m_midi_out)
      return;

    libremidi::message msg;
    for(const auto& byte : bytes)
    {
      msg.bytes.push_back(static_cast<uint8_t>(byte.toInt()));
    }

    if(auto err = m_midi_out->send_message(msg); err != stdx::error{})
    {
      if(onError.isCallable())
      {
        const auto& errmsg = err.message();
        run_on_qt_thread(
            { onError.call({QString::fromUtf8(errmsg.data(), errmsg.size())}); });
      }
    }
  }
  W_SLOT(sendMessage)

  // Convenience methods for common MIDI messages
  void sendNoteOn(int channel, int note, int velocity)
  {
    if(!m_is_open || !m_midi_out)
      return;
    
    if(channel < 1 || channel > 16 || note < 0 || note > 127 || velocity < 0 || velocity > 127)
      return;
    
    m_midi_out->send_message(0x90 | (channel - 1), note, velocity);
  }
  W_SLOT(sendNoteOn)

  void sendNoteOff(int channel, int note, int velocity)
  {
    if(!m_is_open || !m_midi_out)
      return;
    
    if(channel < 1 || channel > 16 || note < 0 || note > 127 || velocity < 0 || velocity > 127)
      return;
    
    m_midi_out->send_message(0x80 | (channel - 1), note, velocity);
  }
  W_SLOT(sendNoteOff)

  void sendControlChange(int channel, int controller, int value)
  {
    if(!m_is_open || !m_midi_out)
      return;
    
    if(channel < 1 || channel > 16 || controller < 0 || controller > 127 || value < 0 || value > 127)
      return;
    
    m_midi_out->send_message(0xB0 | (channel - 1), controller, value);
  }
  W_SLOT(sendControlChange)

  void sendProgramChange(int channel, int program)
  {
    if(!m_is_open || !m_midi_out)
      return;
    
    if(channel < 1 || channel > 16 || program < 0 || program > 127)
      return;
    
    m_midi_out->send_message(0xC0 | (channel - 1), program);
  }
  W_SLOT(sendProgramChange)

  void sendPitchBend(int channel, int value)
  {
    if(!m_is_open || !m_midi_out)
      return;
    
    if(channel < 1 || channel > 16 || value < 0 || value > 16383)
      return;
    
    m_midi_out->send_message(0xE0 | (channel - 1), value & 0x7F, (value >> 7) & 0x7F);
  }
  W_SLOT(sendPitchBend)

  void sendSysex(QVariantList data)
  {
    if(!m_is_open || !m_midi_out)
      return;

    libremidi::message msg;
    msg.bytes.push_back(0xF0); // Start of SysEx
    for(const auto& byte : data)
    {
      int val = byte.toInt();
      if(val >= 0 && val <= 127) // SysEx data bytes must be 7-bit
        msg.bytes.push_back(static_cast<uint8_t>(val));
    }
    msg.bytes.push_back(0xF7); // End of SysEx

    if(auto err = m_midi_out->send_message(msg); err != stdx::error{})
    {
      if(onError.isCallable())
      {
        const auto& errmsg = err.message();
        run_on_qt_thread(
            { onError.call({QString::fromUtf8(errmsg.data(), errmsg.size())}); });
      }
    }
  }
  W_SLOT(sendSysex)

  // Callbacks
  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;

private:
  std::unique_ptr<libremidi::observer> m_observer;
  std::unique_ptr<libremidi::midi_out> m_midi_out;
  std::atomic_bool m_is_open{false};
};

}