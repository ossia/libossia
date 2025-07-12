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
 var devices = Protocols.outboundUMPDevices();
 var ump = Protocols.outboundUMP({
    Transport: devices[0],
    onOpen: function(socket) {
      console.log("UMP port opened!");
      // Send MIDI 2.0 note on with velocity and attribute
      socket.sendNoteOn(1, 60, 32768, 0, 0); // group 1, middle C, velocity 50%, no attribute
      
      // Send raw UMP message (4 32-bit words)
      socket.sendMessage([0x40902C00, 0x80000000, 0, 0]); // MIDI 2.0 Note On
    },
    onError: function(error) {
      console.log("UMP error:", error);
    }
  });
*/
class qml_ump_outbound_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_ump_outbound_socket)
public:
  qml_ump_outbound_socket() { }

  ~qml_ump_outbound_socket() { close(); }

  void open(const libremidi::port_information& pi)
  {
    try
    {
      // Configure UMP output
      libremidi::output_configuration config;

      // Create UMP output with MIDI 2.0 API
      m_ump_out = std::make_unique<libremidi::midi_out>(
          config, libremidi::midi2::default_api());

      // Open the port
      if(auto err = m_ump_out->open_port(static_cast<const libremidi::output_port&>(pi));
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

      // Get available UMP output ports
      auto ports = m_observer->get_output_ports();

      if(ports.empty())
      {
        if(onError.isCallable())
        {
          run_on_qt_thread({ onError.call({"No UMP output ports available"}); });
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
      m_ump_out.reset();
      m_observer.reset();

      if(onClose.isCallable())
      {
        run_on_qt_thread({ onClose.call(); });
      }
    }
  }
  W_SLOT(close)

  // Send raw UMP message (expects 4 32-bit words)
  void sendMessage(QVariantList words)
  {
    if(!m_is_open || !m_ump_out)
      return;

    if(words.size() != 4)
    {
      if(onError.isCallable())
      {
        run_on_qt_thread({ onError.call({"UMP message must contain exactly 4 words"}); });
      }
      return;
    }

    libremidi::ump msg;
    msg.timestamp = 0;
    for(int i = 0; i < 4; ++i)
    {
      msg.data[i] = static_cast<uint32_t>(words[i].toUInt());
    }

    if(auto err = m_ump_out->send_ump(msg); err != stdx::error{})
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

  // MIDI 2.0 convenience methods
  void sendNoteOn(int group, int channel, int note, int velocity, int attributeType, int attribute)
  {
    if(!m_is_open || !m_ump_out)
      return;
    
    if(group < 1 || group > 16 || channel < 1 || channel > 16 || 
       note < 0 || note > 127 || velocity < 0 || velocity > 65535)
      return;
    
    libremidi::ump msg;
    msg.timestamp = 0;
    // Message Type 4 (MIDI 2.0), Group, Status 0x9 (Note On), Channel
    msg.data[0] = (0x4 << 28) | ((group - 1) << 24) | (0x90 << 16) | ((channel - 1) << 16) | (note << 8) | attributeType;
    // Velocity (16-bit) and Attribute (16-bit)
    msg.data[1] = (velocity << 16) | (attribute & 0xFFFF);
    msg.data[2] = 0;
    msg.data[3] = 0;
    
    m_ump_out->send_ump(msg);
  }
  W_SLOT(sendNoteOn)

  void sendNoteOff(int group, int channel, int note, int velocity, int attributeType, int attribute)
  {
    if(!m_is_open || !m_ump_out)
      return;
    
    if(group < 1 || group > 16 || channel < 1 || channel > 16 || 
       note < 0 || note > 127 || velocity < 0 || velocity > 65535)
      return;
    
    libremidi::ump msg;
    msg.timestamp = 0;
    // Message Type 4 (MIDI 2.0), Group, Status 0x8 (Note Off), Channel
    msg.data[0] = (0x4 << 28) | ((group - 1) << 24) | (0x80 << 16) | ((channel - 1) << 16) | (note << 8) | attributeType;
    // Velocity (16-bit) and Attribute (16-bit)
    msg.data[1] = (velocity << 16) | (attribute & 0xFFFF);
    msg.data[2] = 0;
    msg.data[3] = 0;
    
    m_ump_out->send_ump(msg);
  }
  W_SLOT(sendNoteOff)

  void sendControlChange(int group, int channel, int controller, int value)
  {
    if(!m_is_open || !m_ump_out)
      return;
    
    if(group < 1 || group > 16 || channel < 1 || channel > 16 || 
       controller < 0 || controller > 127)
      return;
    
    libremidi::ump msg;
    msg.timestamp = 0;
    // Message Type 4 (MIDI 2.0), Group, Status 0xB (Control Change), Channel, Controller
    msg.data[0] = (0x4 << 28) | ((group - 1) << 24) | (0xB0 << 16) | ((channel - 1) << 16) | (controller << 8);
    // 32-bit value
    msg.data[1] = static_cast<uint32_t>(value);
    msg.data[2] = 0;
    msg.data[3] = 0;
    
    m_ump_out->send_ump(msg);
  }
  W_SLOT(sendControlChange)

  void sendProgramChange(int group, int channel, int program, int bankMSB, int bankLSB)
  {
    if(!m_is_open || !m_ump_out)
      return;
    
    if(group < 1 || group > 16 || channel < 1 || channel > 16 || 
       program < 0 || program > 127 || bankMSB < 0 || bankMSB > 127 || bankLSB < 0 || bankLSB > 127)
      return;
    
    libremidi::ump msg;
    msg.timestamp = 0;
    // Message Type 4 (MIDI 2.0), Group, Status 0xC (Program Change), Channel
    msg.data[0] = (0x4 << 28) | ((group - 1) << 24) | (0xC0 << 16) | ((channel - 1) << 16);
    // Program, Bank MSB, Bank LSB
    msg.data[1] = (program << 24) | (bankMSB << 8) | bankLSB;
    msg.data[2] = 0;
    msg.data[3] = 0;
    
    m_ump_out->send_ump(msg);
  }
  W_SLOT(sendProgramChange)

  void sendPitchBend(int group, int channel, int value)
  {
    if(!m_is_open || !m_ump_out)
      return;
    
    if(group < 1 || group > 16 || channel < 1 || channel > 16)
      return;
    
    libremidi::ump msg;
    msg.timestamp = 0;
    // Message Type 4 (MIDI 2.0), Group, Status 0xE (Pitch Bend), Channel
    msg.data[0] = (0x4 << 28) | ((group - 1) << 24) | (0xE0 << 16) | ((channel - 1) << 16);
    // 32-bit pitch bend value
    msg.data[1] = static_cast<uint32_t>(value);
    msg.data[2] = 0;
    msg.data[3] = 0;
    
    m_ump_out->send_ump(msg);
  }
  W_SLOT(sendPitchBend)

  // Callbacks
  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;

private:
  std::unique_ptr<libremidi::observer> m_observer;
  std::unique_ptr<libremidi::midi_out> m_ump_out;
  std::atomic_bool m_is_open{false};
};

}
