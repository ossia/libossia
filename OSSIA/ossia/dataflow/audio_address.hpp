#pragma once
#include <ossia/network/base/address.hpp>
#include <ossia/dataflow/data.hpp>
#include <gsl/span>
#include <ModernMIDI/midi_message.h>
#include <ModernMIDI/midi_input.h>
#include <ModernMIDI/midi_output.h>
#include <ossia/network/midi/midi_protocol.hpp>
namespace ossia
{
class OSSIA_EXPORT audio_address : public ossia::net::address_base
{
  ossia::net::node_base& m_node;

public:
  audio_address(ossia::net::node_base& n): m_node{n} { }

  virtual ~audio_address();

  gsl::span<float> audio;

  void clone_value(audio_vector& res) const
  {
    const auto N = std::min((int) audio.size(), (int) res.size());
    for(int i = 0; i < N; i++)
      res[i] = audio[i];

    // Fill remaining with zeros.
    for(int i = N; i < res.size(); i++)
      res[i] = 0.;
  }

  void push_value(const audio_port& port)
  {
    const int port_N = port.samples.size();

    // The first buffer, if it exists, will fill with zeros
    if(port_N > 0)
    {
      const int N = std::min((int)audio.size(), (int)port.samples[0].size());

      // First buffer
      for(int j = 0; j < N; j++)
        audio[j] = port.samples[0][j];

      // Following buffers are mixed
      for(int i = 1; i < port_N; i++)
      {
        auto& v = port.samples[i];
        const int N = std::min((int)audio.size(), (int)v.size());

        for(int j = 0; j < N; j++)
          audio[j] += v[j];
      }
    }
    else
    {
      for(auto& v : audio) v = 0.;
    }
  }

  net::node_base&getNode() const override
  { return m_node; }

  void pullValue() override
  { }
  net::address_base&pushValue(const value&) override
  { return *this; }
  net::address_base&pushValue() override
  { return *this; }
  value cloneValue() const override
  { return {}; }
  net::address_base& setValue(const value&) override
  { return *this; }
  val_type getValueType() const override
  { return {}; }
  net::address_base&setValueType(val_type) override
  { return *this; }
  access_mode getAccessMode() const override
  { return {}; }
  net::address_base&setAccessMode(access_mode) override
  { return *this; }
  const domain&getDomain() const override
  { throw; }
  net::address_base&setDomain(const domain&) override
  { return *this; }
  bounding_mode getBoundingMode() const override
  { return {}; }
  net::address_base&setBoundingMode(bounding_mode) override
  { return *this; }
  repetition_filter getRepetitionFilter() const override
  { return {}; }
  net::address_base&setRepetitionFilter(repetition_filter) override
  { return *this; }
};


class OSSIA_EXPORT midi_generic_address : public ossia::net::address_base
{
  ossia::net::node_base& m_node;
  std::unique_ptr<mm::MidiInput> m_input;
  std::unique_ptr<mm::MidiOutput> m_output;

public:
  midi_generic_address(
      ossia::net::node_base& n):
    m_node{n}
  { }

  virtual ~midi_generic_address();

  value_vector<mm::MidiMessage> messages;


  void clone_value(value_vector<mm::MidiMessage>& port) const
  {
    port.insert(port.end(), messages.begin(), messages.end());
  }

  void push_value(const mm::MidiMessage& mess)
  {
    if(m_output)
    {
      m_output->send(mess);
    }
  }

  net::node_base&getNode() const override
  { return m_node; }

  void pullValue() override
  { }
  net::address_base&pushValue(const value&) override
  { return *this; }
  net::address_base&pushValue() override
  { return *this; }
  value cloneValue() const override
  { return {}; }
  net::address_base& setValue(const value&) override
  { return *this; }
  val_type getValueType() const override
  { return {}; }
  net::address_base&setValueType(val_type) override
  { return *this; }
  access_mode getAccessMode() const override
  { return {}; }
  net::address_base&setAccessMode(access_mode) override
  { return *this; }
  const domain&getDomain() const override
  { throw; }
  net::address_base&setDomain(const domain&) override
  { return *this; }
  bounding_mode getBoundingMode() const override
  { return {}; }
  net::address_base&setBoundingMode(bounding_mode) override
  { return *this; }
  repetition_filter getRepetitionFilter() const override
  { return {}; }
  net::address_base&setRepetitionFilter(repetition_filter) override
  { return *this; }
};


}
