#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/midi/midi_protocol.hpp>
#include <ModernMIDI/midi_input.h>
#include <ModernMIDI/midi_message.h>
#include <ModernMIDI/midi_output.h>
#include <gsl/span>

namespace ossia
{
class OSSIA_EXPORT audio_parameter : public ossia::net::parameter_base
{

public:
  std::vector<gsl::span<float>> audio;

  audio_parameter(ossia::net::node_base& n);

  virtual ~audio_parameter();


  void clone_value(audio_vector& res) const;
  void push_value(const audio_port& port);

  net::node_base& get_node() const override;

  void pull_value() override;
  net::parameter_base& push_value(const ossia::value&) override;
  net::parameter_base& push_value(ossia::value&&) override;
  net::parameter_base& push_value() override;
  ossia::value value() const override;
  net::parameter_base& set_value(const ossia::value&) override;
  net::parameter_base& set_value(ossia::value&&) override;
  val_type get_value_type() const override;
  net::parameter_base& set_value_type(val_type) override;
  access_mode get_access() const override;
  net::parameter_base& set_access(access_mode) override;
  const domain& get_domain() const override;
  net::parameter_base& set_domain(const domain&) override;
  bounding_mode get_bounding() const override;
  net::parameter_base& set_bounding(bounding_mode) override;
  repetition_filter get_repetition_filter() const override;
  net::parameter_base& set_repetition_filter(repetition_filter) override;

  private:
    ossia::net::node_base& m_node;
};

class OSSIA_EXPORT midi_generic_parameter : public ossia::net::parameter_base
{
  ossia::net::node_base& m_node;
  std::unique_ptr<mm::MidiInput> m_input;
  std::unique_ptr<mm::MidiOutput> m_output;

public:
  midi_generic_parameter(ossia::net::node_base& n) : m_node{n}
  {
  }

  virtual ~midi_generic_parameter();

  value_vector<mm::MidiMessage> messages;

  void clone_value(value_vector<mm::MidiMessage>& port) const
  {
    port.insert(port.end(), messages.begin(), messages.end());
  }

  void push_value(const mm::MidiMessage& mess)
  {
    if (m_output)
    {
      m_output->send(mess);
    }
  }

  net::node_base& get_node() const override
  {
    return m_node;
  }

  void pull_value() override
  {
  }
  net::parameter_base& push_value(const ossia::value&) override
  {
    return *this;
  }
  net::parameter_base& push_value(ossia::value&&) override
  {
    return *this;
  }
  net::parameter_base& push_value() override
  {
    return *this;
  }
  ossia::value value() const override
  {
    return {};
  }
  net::parameter_base& set_value(const ossia::value&) override
  {
    return *this;
  }
  net::parameter_base& set_value(ossia::value&&) override
  {
    return *this;
  }
  val_type get_value_type() const override
  {
    return {};
  }
  net::parameter_base& set_value_type(val_type) override
  {
    return *this;
  }
  access_mode get_access() const override
  {
    return {};
  }
  net::parameter_base& set_access(access_mode) override
  {
    return *this;
  }
  const domain& get_domain() const override
  {
    throw;
  }
  net::parameter_base& set_domain(const domain&) override
  {
    return *this;
  }
  bounding_mode get_bounding() const override
  {
    return {};
  }
  net::parameter_base& set_bounding(bounding_mode) override
  {
    return *this;
  }
  repetition_filter get_repetition_filter() const override
  {
    return {};
  }
  net::parameter_base& set_repetition_filter(repetition_filter) override
  {
    return *this;
  }
};

class OSSIA_EXPORT texture_generic_parameter : public ossia::net::parameter_base
{
  ossia::net::node_base& m_node;
  int32_t m_tex{};

public:
  texture_generic_parameter(ossia::net::node_base& n) : m_node{n}
  {
  }

  virtual ~texture_generic_parameter();

  void clone_value() const
  {
  }

  void push_value(int)
  {
  }

  net::node_base& get_node() const override
  {
    return m_node;
  }

  void pull_value() override
  {
  }
  net::parameter_base& push_value(const ossia::value&) override
  {
    return *this;
  }
  net::parameter_base& push_value(ossia::value&&) override
  {
    return *this;
  }
  net::parameter_base& push_value() override
  {
    return *this;
  }
  ossia::value value() const override
  {
    return {};
  }
  net::parameter_base& set_value(const ossia::value&) override
  {
    return *this;
  }
  net::parameter_base& set_value(ossia::value&&) override
  {
    return *this;
  }
  val_type get_value_type() const override
  {
    return {};
  }
  net::parameter_base& set_value_type(val_type) override
  {
    return *this;
  }
  access_mode get_access() const override
  {
    return {};
  }
  net::parameter_base& set_access(access_mode) override
  {
    return *this;
  }
  const domain& get_domain() const override
  {
    throw;
  }
  net::parameter_base& set_domain(const domain&) override
  {
    return *this;
  }
  bounding_mode get_bounding() const override
  {
    return {};
  }
  net::parameter_base& set_bounding(bounding_mode) override
  {
    return *this;
  }
  repetition_filter get_repetition_filter() const override
  {
    return {};
  }
  net::parameter_base& set_repetition_filter(repetition_filter) override
  {
    return *this;
  }
};
}
