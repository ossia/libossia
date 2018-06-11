#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia::nodes
{
class OSSIA_EXPORT merger final :
    public ossia::graph_node
{
public:
  merger(int count)
  {
    for(int i = 0; i < count; i++)
    {
      auto inl = ossia::make_inlet<ossia::audio_port>();
      inl->data.target<ossia::audio_port>()->samples.resize(2);
      for(auto& channel : inl->data.target<ossia::audio_port>()->samples)
      {
        channel.reserve(512);
      }
      m_inlets.push_back(std::move(inl));
    }

    m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
    m_outlets.back()->data.target<ossia::audio_port>()->samples.resize(2 * count);
    for(auto& channel : m_outlets.back()->data.target<ossia::audio_port>()->samples)
    {
      channel.reserve(512);
    }
  }

  ~merger() override
  {

  }

  void run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
  {
    auto& out = m_outlets.back()->data.target<ossia::audio_port>()->samples;
    std::size_t cur = 0;
    for(auto inl : m_inlets)
    {
      auto& in = inl->data.target<ossia::audio_port>()->samples;

      for(std::size_t i = 0; i < std::min((std::size_t)2, in.size()); i++)
      {
        assert(cur < out.size());
        out[cur] = in[i];
        cur++;
      }
    }
  }

  std::string label() const noexcept override
  {
    return "Stereo Merger";
  }
};
}
