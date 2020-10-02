#include <ossia/audio/audio_protocol.hpp>
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node_functions.hpp>

namespace ossia
{
audio_protocol::audio_protocol()
{
}


audio_protocol::~audio_protocol()
{
}

void audio_protocol::setup_tree(int inputs, int outputs)
{
  if(inputs == int(audio_ins.size()) && outputs == int(audio_outs.size()))
    return;

  auto& dev = get_device();
  auto& root = dev.get_root_node();

  audio_ins.clear();
  audio_outs.clear();

  main_audio_in = ossia::net::find_or_create_parameter<ossia::audio_parameter>(
      root, "/in/main");
  main_audio_out
      = ossia::net::find_or_create_parameter<ossia::audio_parameter>(
          root, "/out/main");
  for (int i = 0; i < inputs; i++)
  {
    audio_ins.push_back(
        ossia::net::find_or_create_parameter<ossia::audio_parameter>(
            root, "/in/" + std::to_string(i)));
  }
  for (int i = 0; i < outputs; i++)
  {
    audio_outs.push_back(
        ossia::net::find_or_create_parameter<ossia::audio_parameter>(
            root, "/out/" + std::to_string(i)));
  }

  main_audio_in->audio.resize(inputs);
  for (int i = 0; i < inputs; i++)
  {
    audio_ins[i]->audio.resize(1);
  }

  main_audio_out->audio.resize(outputs);
  for (int i = 0; i < outputs; i++)
  {
    audio_outs[i]->audio.resize(1);
  }
}

void audio_protocol::advance_tick(std::size_t count)
{
  for (auto& chan : main_audio_in->audio)
  {
    if (!chan.empty())
    {
      chan = chan.subspan(count);
    }
  }
  for (auto& chan : main_audio_out->audio)
  {
    if (!chan.empty())
    {
      chan = chan.subspan(count);
    }
  }

  for (auto in : audio_ins)
  {
    for (auto& chan : in->audio)
    {
      if (!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for (auto in : in_mappings)
  {
    for (auto& chan : in->audio)
    {
      if (!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for (auto out : audio_outs)
  {
    for (auto& chan : out->audio)
    {
      if (!chan.empty())
      {
        chan = chan.subspan(count);
      }
    }
  }

  for (auto out : out_mappings)
  {
    for (auto& chan : out->audio)
    {
      if (!chan.empty())
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

bool audio_protocol::push_bundle(
    const std::vector<const ossia::net::parameter_base*>&)
{
  return false;
}

bool audio_protocol::push_raw(const ossia::net::full_parameter_data&)
{
  return false;
}

bool audio_protocol::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>&)
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

void audio_protocol::register_parameter(mapped_audio_parameter& p)
{
  if (p.is_output)
    out_mappings.push_back(&p);
  else
    in_mappings.push_back(&p);
}

void audio_protocol::unregister_parameter(mapped_audio_parameter& p)
{
  if (p.is_output)
  {
    auto it = ossia::find(out_mappings, &p);
    if (it != out_mappings.end())
      out_mappings.erase(it);
  }
  else
  {
    auto it = ossia::find(in_mappings, &p);
    if (it != in_mappings.end())
      in_mappings.erase(it);
  }
}

void audio_protocol::register_parameter(virtual_audio_parameter& p)
{
   virtaudio.push_back(&p);
}

void audio_protocol::unregister_parameter(virtual_audio_parameter& p)
{
  auto it = ossia::find(virtaudio, &p);
  if (it != virtaudio.end())
    virtaudio.erase(it);
}

void audio_protocol::setup_buffers(ossia::audio_tick_state state)
{
  setup_tree(state.n_in, state.n_out);

  //using idx_t = gsl::span<float>::index_type;
  const gsl::span<float>::size_type fc = state.frames;

  // Prepare virtual audio inputs
  for (auto virt : virtaudio)
  {
    virt->set_buffer_size(state.frames);
  }

  // Prepare audio inputs
  for (int i = 0; i < state.n_in; i++)
  {
    main_audio_in->audio[i] = {state.inputs[i], fc};
    audio_ins[i]->audio[0] = {state.inputs[i], fc};
  }

  for (auto mapped : in_mappings)
  {
    mapped->audio.resize(mapped->mapping.size());
    for (std::size_t i = 0; i < mapped->mapping.size(); i++)
    {
      auto map_channel = mapped->mapping[i];
      if (map_channel < state.n_in)
        mapped->audio[i] = {state.inputs[map_channel], fc};
      else
        mapped->audio[i] = {};
    }
  }

  // Prepare audio outputs
  for (int i = 0; i < state.n_out; i++)
  {
    main_audio_out->audio[i] = {state.outputs[i], fc};
    audio_outs[i]->audio[0] = {state.outputs[i], fc};

    for (int j = 0; j < (int)state.frames; j++)
    {
      state.outputs[i][j] = 0;
    }
  }

  for (auto mapped : out_mappings)
  {
    mapped->audio.resize(mapped->mapping.size());
    for (std::size_t i = 0; i < mapped->mapping.size(); i++)
    {
      auto map_channel = mapped->mapping[i];
      if (map_channel < state.n_out)
        mapped->audio[i] = {state.outputs[map_channel], fc};
      else
        mapped->audio[i] = {};
    }
  }
}
}
