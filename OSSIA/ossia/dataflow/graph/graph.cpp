// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>
namespace ossia
{

void graph_util::log_inputs(const graph_node& n, spdlog::logger& logger)
{
  struct
  {
      spdlog::logger& logger;
      int i = 0;
      void operator()(const ossia::value_port& p)
      {
        for(const ossia::tvalue& val : p.get_data())
          logger.log(spdlog::level::debug, "input {} (value): {}", i, val.value);
        i++;
      }
      void operator()(const ossia::audio_port& p)
      {
        //logger.log(spdlog::level::debug, "input {} (audio)");
        i++;
      }
      void operator()(const ossia::midi_port& p)
      {
        for(const mm::MidiMessage& val : p.messages)
        {
          switch(val.data.size())
          {
            case 1:
              logger.log(spdlog::level::debug, "input {} (midi): {}", i, val.data[0]);
              break;
            case 2:
              logger.log(spdlog::level::debug, "input {} (midi): {} {}", i, val.data[0], val.data[1]);
              break;
            case 3:
              logger.log(spdlog::level::debug, "input {} (midi): {} {} {}", i, val.data[0], val.data[1], val.data[2]);
              break;
            default:
              break;
          }
        }
        i++;
      }

      void operator()() { }
  } vis{logger};

  for(const ossia::inlet_ptr& in : n.inputs())
  {
    ossia::apply(vis, in->data);
  }
}

void graph_util::log_outputs(const graph_node& n, spdlog::logger& logger)
{
  struct
  {
      spdlog::logger& logger;
      int i = 0;
      void operator()(const ossia::value_port& p)
      {
        for(const ossia::tvalue& val : p.get_data())
          logger.log(spdlog::level::debug, "output {} (value): {}", i, val.value);
        i++;
      }
      void operator()(const ossia::audio_port& p)
      {
        //logger.log(spdlog::level::debug, "output {} (audio)");
        i++;
      }
      void operator()(const ossia::midi_port& p)
      {
        for(const mm::MidiMessage& val : p.messages)
        {
          switch(val.data.size())
          {
            case 1:
              logger.log(spdlog::level::debug, "output {} (midi): {}", i, val.data[0]);
              break;
            case 2:
              logger.log(spdlog::level::debug, "output {} (midi): {} {}", i, val.data[0], val.data[1]);
              break;
            case 3:
              logger.log(spdlog::level::debug, "output {} (midi): {} {} {}", i, val.data[0], val.data[1], val.data[2]);
              break;
            default:
              break;
          }
        }
        i++;
      }
      void operator()() { }
  } vis{logger};

  for(const ossia::outlet_ptr& out : n.outputs())
  {
    ossia::apply(vis, out->data);
  }

}
graph_interface::~graph_interface()
{

}
audio_parameter::~audio_parameter()
{
}

texture_generic_parameter::~texture_generic_parameter()
{
}
}
