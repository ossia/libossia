#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia::nodes
{
class mono_merger final : public ossia::graph_node
{
  int m_count{};

public:
  explicit mono_merger(int count)
      : m_count{count}
  {
    for(int i = 0; i < count; i++)
    {
      auto inl = new ossia::audio_inlet;
      inl->target<ossia::audio_port>()->set_channels(1);
      m_inlets.push_back(std::move(inl));
    }

    m_outlets.push_back(new ossia::audio_outlet);
    m_outlets.back()->target<ossia::audio_port>()->set_channels(count);
  }

  ~mono_merger() override = default;

  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    auto& op = *m_outlets.back()->target<ossia::audio_port>();
    op.set_channels(this->m_count);
    auto& out = op.get();
    std::size_t cur = 0;
    for(int i = 0; i < m_count; i++)
    {
      auto& in = m_inlets[i]->target<ossia::audio_port>()->get();

      if(in.size() > 0)
        out[cur++] = in[0];
      else
        out[cur++].resize(e.bufferSize());
    }

    for(auto& c : out)
    {
      if(c.size() < e.bufferSize())
      {
        c.resize(e.bufferSize());
      }
    }
  }

  [[nodiscard]] std::string label() const noexcept override { return "Mono Merger"; }
};
class merger final : public ossia::graph_node
{
  int m_count{};

public:
  explicit merger(int count)
      : m_count{count}
  {
    for(int i = 0; i < count; i++)
    {
      auto inl = new ossia::audio_inlet;
      inl->target<ossia::audio_port>()->set_channels(2);
      for(auto& channel : *inl->target<ossia::audio_port>())
      {
        channel.reserve(512);
      }
      m_inlets.push_back(std::move(inl));
    }

    m_outlets.push_back(new ossia::audio_outlet);
    m_outlets.back()->target<ossia::audio_port>()->set_channels(2 * count);
    for(auto& channel : *m_outlets.back()->target<ossia::audio_port>())
    {
      channel.reserve(512);
    }
  }

  ~merger() override = default;

  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    auto& op = *m_outlets.back()->target<ossia::audio_port>();
    op.set_channels(2 * this->m_count);
    auto& out = op.get();
    std::size_t cur = 0;
    for(int i = 0; i < m_count; i++)
    {
      auto& in = m_inlets[i]->target<ossia::audio_port>()->get();

      switch(in.size())
      {
        case 1:
          out[cur++] = in[0];
          out[cur++] = in[0];
          break;
        case 2:
          out[cur++] = in[0];
          out[cur++] = in[1];
          break;
        default:
          out[cur++].resize(e.bufferSize());
          out[cur++].resize(e.bufferSize());
          break;
      }
    }

    for(auto& c : out)
    {
      if(c.size() < e.bufferSize())
      {
        c.resize(e.bufferSize());
      }
    }
  }

  [[nodiscard]] std::string label() const noexcept override { return "Stereo Merger"; }
};
}
