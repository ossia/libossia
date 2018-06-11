#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/execution_state.hpp>

namespace ossia::nodes
{

// TODO have a process that does reset it on start & stop.
// necessary for the looping to work correctly.
class input final :
    public ossia::graph_node
{
public:
  input()
  {
    m_inlets.push_back(ossia::make_inlet<ossia::audio_port>());
    m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
  }

  ~input() override
  {

  }

  void set_start(std::size_t v) { m_startChan = v; m_data.clear(); }
  void set_num_channel(std::size_t v) { m_numChan = v; m_data.clear(); }
private:
  ossia::net::node_base* get_params(ossia::exec_state_facade e)
  {
    auto dev = ossia::find_if(e.impl.audioDevices, [] (const ossia::net::device_base* dev) {
      return dev->get_name() == "audio";
    });
    if(dev != e.impl.audioDevices.end())
    {
      auto in = ossia::net::find_node((*dev)->get_root_node(), "/in/main");
      return in;
    }
    return {};
  }
  void run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
  {
    // First read the requested channels at the end of "data".
    if(m_numChan == 0)
    {
      return;
    }
    auto in_node = get_params(e);
    if(!in_node)
      return;

    auto p = static_cast<ossia::audio_parameter*>(in_node->get_parameter());

    auto maxchan = std::min(m_startChan + m_numChan, p->audio.size());

    m_data.resize(maxchan - m_startChan);
    for(std::size_t chan = m_startChan; chan < maxchan; chan++)
    {
      const auto& src = p->audio[chan];
      auto& res = m_data[chan - m_startChan];
      std::size_t start = res.size();
      res.resize(start + (std::size_t)src.size());
      for (int i = 0; i < src.size(); i++)
        res[start + i] = src[i];
    }

    // Then copy the requested data to the output
    if(m_data.empty())
      return;

    const std::size_t chan = m_data.size();
    const auto len = (int64_t)m_data[0].size();
    ossia::audio_port& ap = *m_outlets[0]->data.target<ossia::audio_port>();
    ap.samples.resize(chan);
    int64_t max_N = std::min(t.date.impl, len);
    if(max_N <= 0)
      return;
    auto samples = max_N - t.prev_date + t.offset.impl;
    if(samples <= 0)
      return;

    if(t.date > t.prev_date)
    {
      for(std::size_t i = 0; i < chan; i++)
      {
        ap.samples[i].resize(samples);
        for(int64_t j = t.prev_date; j < max_N; j++)
        {
          ap.samples[i][j - t.prev_date + t.offset.impl] = m_data[i][j];
        }
      }
    }
    else
    {
      // TODO rewind correctly and add rubberband
      for(std::size_t i = 0; i < chan; i++)
      {
        ap.samples[i].resize(samples);
        for(int64_t j = t.prev_date; j < max_N; j++)
        {
          ap.samples[i][max_N - (j - t.prev_date) + t.offset.impl] = m_data[i][j];
        }
      }
    }
  }

  // todo linearizeme
  std::vector<ossia::float_vector> m_data;

  std::size_t m_startChan{};
  std::size_t m_numChan{};
};


}
