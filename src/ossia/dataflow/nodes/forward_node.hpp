#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
namespace ossia::nodes
{
class forward_node : public ossia::nonowning_graph_node
{
public:
  forward_node()
  {
    m_inlets.push_back(&audio_in);
    // m_inlets.push_back(&midi_in);
    m_outlets.push_back(&audio_out);
    // m_outlets.push_back(&midi_out);
  }

  void run(const token_request& t, exec_state_facade) noexcept override
  {
    // OPTIMIZEME : we copy the ins & outs of the token more
    // than necessary
    {
      ossia::audio_port& i = *audio_in;
      ossia::audio_port& o = *audio_out;
      if(!o.has_gain)
      {
        o.samples = i.samples;
      }
      else
      {
        const double g = o.gain;
        auto C = i.samples.size();
        o.samples.resize(C);

        switch(C)
        {
        case 0:
            return;

        case 1:
        {
            auto N = i.samples[0].size();
            auto i_ptr = i.samples[0].data();
            auto o_ptr  = o.samples[0].data();

            for(std::size_t sample = 0; sample < N; sample++)
            {
                o_ptr[sample] = i_ptr[sample] * g;
            }
            break;
        }

        default:
        {
            while(o.pan.size() < C)
                o.pan.push_back(ossia::sqrt_2 / 2.);
            for(auto chan = 0U; chan < C; chan++)
            {
                auto N = i.samples[chan].size();
                o.samples[chan].resize(N);

                auto i_ptr = i.samples[chan].data();
                auto o_ptr  = o.samples[chan].data();

                const auto vol = o.pan[chan] * g;
                for(std::size_t sample = 0; sample < N; sample++)
                {
                    o_ptr[sample] = i_ptr[sample] * vol;
                }
            }
            break;
        }
        }
      }
    }
    // {
    //   auto i = midi_in.target<ossia::midi_port>();
    //   auto o = midi_out.target<ossia::midi_port>();
    //   o->messages = i->messages;
    // }
  }
  ossia::audio_inlet audio_in;
  // ossia::midi_inlet midi_in;
  ossia::audio_outlet audio_out;
  // ossia::midi_outlet midi_out;
};

class interval final : public forward_node
{
public:
  using forward_node::forward_node;
  std::string label() const noexcept override
  {
    return "Interval";
  }
};
class loop final : public forward_node
{
public:
  using forward_node::forward_node;
  std::string label() const noexcept override
  {
    return "Loop";
  }
};
class scenario final : public forward_node
{
public:
  using forward_node::forward_node;
  std::string label() const noexcept override
  {
    return "Scenario";
  }
};
}
