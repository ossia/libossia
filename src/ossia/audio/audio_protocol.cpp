#include <ossia/audio/audio_parameter.hpp>
#include <ossia/audio/audio_protocol.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node_functions.hpp>

namespace ossia
{
audio_protocol::audio_protocol()
    : protocol_base{flags{}}
{
}

audio_protocol::~audio_protocol() = default;

void audio_protocol::clear_buffers() noexcept
{
  auto clear = [](ossia::audio_parameter* p) {
    if(p)
      for(auto& chan : p->audio)
        chan = {};
  };

  clear(main_audio_in);
  clear(main_audio_out);
  for(auto p : audio_ins)
    clear(p);
  for(auto p : audio_outs)
    clear(p);
  for(auto p : in_mappings)
    clear(p);
  for(auto p : out_mappings)
    clear(p);
}

void audio_protocol::stop()
{
  clear_buffers();
}

void audio_protocol::setup_tree(int inputs, int outputs)
{
  // The parameters that go away when the channel count shrinks would otherwise
  // keep spans into the previous driver's buffers.
  clear_buffers();

  auto& dev = get_device();
  auto& root = dev.get_root_node();

  audio_ins.clear();
  audio_outs.clear();

  main_audio_in
      = ossia::net::find_parameter_or_create_node<ossia::audio_parameter>(root, "/in/main");
  main_audio_out
      = ossia::net::find_parameter_or_create_node<ossia::audio_parameter>(root, "/out/main");
  main_audio_in->stage = audio_parameter::gain_stage::pull;
  main_audio_out->stage = audio_parameter::gain_stage::external;
  for(int i = 0; i < inputs; i++)
  {
    audio_ins.push_back(ossia::net::find_parameter_or_create_node<ossia::audio_parameter>(
        root, "/in/" + std::to_string(i + 1)));
    audio_ins.back()->stage = audio_parameter::gain_stage::pull;
    audio_ins.back()->upstream = main_audio_in;
  }
  for(int i = 0; i < outputs; i++)
  {
    audio_outs.push_back(ossia::net::find_parameter_or_create_node<ossia::audio_parameter>(
        root, "/out/" + std::to_string(i + 1)));
  }

  main_audio_in->audio.resize(inputs);
  for(int i = 0; i < inputs; i++)
  {
    audio_ins[i]->audio.resize(1);
  }

  for(auto p : in_mappings)
    p->upstream = main_audio_in;

  main_audio_out->audio.resize(outputs);
  for(int i = 0; i < outputs; i++)
  {
    audio_outs[i]->audio.resize(1);
  }
}

void audio_protocol::apply_main_gain(const audio_tick_state& state) noexcept
{
  const float start = m_main_gain;
  const float target = main_audio_out ? main_audio_out->gain() : 1.f;
  m_main_gain = target;

  const auto frames = state.frames;
  if(frames == 0)
    return;

  if(start == target)
  {
    if(target == 1.f)
      return;

    for(int c = 0; c < state.n_out; c++)
    {
      float* out = state.outputs[c];
      for(std::size_t i = 0; i < frames; i++)
        out[i] *= target;
    }
  }
  else
  {
    const float step = (target - start) / float(frames);
    for(int c = 0; c < state.n_out; c++)
    {
      float* out = state.outputs[c];
      for(std::size_t i = 0; i < frames; i++)
        out[i] *= start + step * float(i + 1);
    }
  }
}

void audio_protocol::advance_tick(std::size_t count)
{
  if(main_audio_in)
  {
    for(auto& chan : main_audio_in->audio)
    {
      if(!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }
  if(main_audio_out)
  {
    for(auto& chan : main_audio_out->audio)
    {
      if(!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for(auto in : audio_ins)
  {
    for(auto& chan : in->audio)
    {
      if(!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for(auto in : m_live.in_mappings)
  {
    for(auto& chan : in->audio)
    {
      if(!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for(auto out : audio_outs)
  {
    for(auto& chan : out->audio)
    {
      if(!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for(auto out : m_live.out_mappings)
  {
    for(auto& chan : out->audio)
    {
      if(!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }
}

bool audio_protocol::pull(ossia::net::parameter_base&)
{
  return false;
}

bool audio_protocol::push(const ossia::net::parameter_base&, const ossia::value& v)
{
  return false;
}

bool audio_protocol::push_bundle(const std::vector<const ossia::net::parameter_base*>&)
{
  return false;
}

bool audio_protocol::push_raw(const ossia::net::full_parameter_data&)
{
  return false;
}

bool audio_protocol::push_raw_bundle(const std::vector<ossia::net::full_parameter_data>&)
{
  return false;
}

bool audio_protocol::observe(ossia::net::parameter_base&, bool)
{
  return false;
}

bool audio_protocol::update(ossia::net::node_base& node_base)
{
  return false;
}

void audio_protocol::set_device(ossia::net::device_base& dev)
{
  m_dev = &dev;
}

namespace
{
template <typename T>
void erase_one(std::vector<T*>& v, T* p)
{
  if(auto it = ossia::find(v, p); it != v.end())
    v.erase(it);
}
}

void audio_protocol::register_parameter(mapped_audio_parameter& p)
{
  if(p.is_output)
  {
    out_mappings.push_back(&p);
  }
  else
  {
    p.upstream = main_audio_in;
    in_mappings.push_back(&p);
  }
  if(!m_deferred)
    m_live = current_ports();
}

void audio_protocol::unregister_parameter(mapped_audio_parameter& p)
{
  // Wherever it is: its direction may have changed since it was registered.
  erase_one(out_mappings, &p);
  erase_one(in_mappings, &p);
  if(!m_deferred)
    m_live = current_ports();
}

void audio_protocol::register_parameter(virtual_audio_parameter& p)
{
  virtaudio.push_back(&p);
  if(!m_deferred)
    m_live = current_ports();
}

void audio_protocol::unregister_parameter(virtual_audio_parameter& p)
{
  erase_one(virtaudio, &p);
  if(!m_deferred)
    m_live = current_ports();
}

void audio_protocol::defer_port_changes(bool b)
{
  m_deferred = b;
  m_live = current_ports();
}

audio_protocol::ports audio_protocol::current_ports() const
{
  return {in_mappings, out_mappings, virtaudio};
}

void audio_protocol::swap_ports(ports& p) noexcept
{
  std::swap(m_live, p);
}

void audio_protocol::setup_buffers(ossia::audio_tick_state state)
{
  // The tree itself is only built in setup_tree, off the audio thread: here we
  // merely rebind the spans, and ignore the channels the tree does not have.
  const int n_in = std::min(state.n_in, int(audio_ins.size()));
  const int n_out = std::min(state.n_out, int(audio_outs.size()));

  // using idx_t = std::span<float>::index_type;
  const std::span<float>::size_type fc = state.frames;

  // Prepare virtual audio inputs
  for(auto virt : m_live.virtaudio)
  {
    virt->set_buffer_size(state.frames);
  }

  // Prepare audio inputs
  for(int i = 0; i < n_in; i++)
  {
    main_audio_in->audio[i] = {state.inputs[i], fc};
    audio_ins[i]->audio[0] = {state.inputs[i], fc};
  }

  for(auto mapped : m_live.in_mappings)
  {
    mapped->audio.resize(mapped->mapping.size());
    for(std::size_t i = 0; i < mapped->mapping.size(); i++)
    {
      auto map_channel = mapped->mapping[i];
      if(map_channel < state.n_in)
        mapped->audio[i] = {state.inputs[map_channel], fc};
      else
        mapped->audio[i] = {};
    }
  }

  // Prepare audio outputs
  for(int i = 0; i < state.n_out; i++)
  {
    for(int j = 0; j < (int)state.frames; j++)
    {
      state.outputs[i][j] = 0;
    }
  }

  for(int i = 0; i < n_out; i++)
  {
    main_audio_out->audio[i] = {state.outputs[i], fc};
    audio_outs[i]->audio[0] = {state.outputs[i], fc};
  }

  for(auto mapped : m_live.out_mappings)
  {
    mapped->audio.resize(mapped->mapping.size());
    for(std::size_t i = 0; i < mapped->mapping.size(); i++)
    {
      auto map_channel = mapped->mapping[i];
      if(map_channel < state.n_out)
        mapped->audio[i] = {state.outputs[map_channel], fc};
      else
        mapped->audio[i] = {};
    }
  }
}
}
