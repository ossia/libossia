// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>
#include <ossia/dataflow/graph/graph.hpp>

namespace ossia
{

struct rescale_in
{
    const ossia::time_value& prev_date;
    const ossia::token_request& request;
    void operator()(ossia::audio_port& port)
    {
      if(request.speed > 1.0)
      {
        for(auto& c : port.samples)
        {
          auto old = c;
          c.resize(c.size() * request.speed);
          for(double i = 0; i < old.size(); i += request.speed)
          {

          }
        }
      }
      else if(request.speed < 1.0)
      {

      }

    }
    void operator()(ossia::midi_port& port)
    {
      value_vector<mm::MidiMessage> values;
      for(auto& val : port.messages)
      {
        if(val.timestamp >= request.offset && val.timestamp < request.offset + request.date - prev_date)
        {
          values.push_back(val);
          values.back().timestamp *= request.speed;
        }
      }
      port.messages = std::move(values);
    }
    void operator()(ossia::value_port& port)
    {
      value_vector<ossia::tvalue> values;
      for(auto& val : port.get_data())
      {
        if(val.timestamp >= request.offset && val.timestamp < request.offset + request.date - prev_date)
        {
          values.push_back(val);
          values.back().timestamp *= request.speed;
        }
      }
      port.set_data(std::move(values));
    }

    void operator()()
    {

    }
};
struct rescale_out
{
    const ossia::time_value& prev_date;
    const ossia::token_request& request;
    void operator()(ossia::audio_port&)
    {

    }
    void operator()(ossia::midi_port&)
    {

    }
    void operator()(ossia::value_port&)
    {

    }

    void operator()()
    {

    }
};

void graph_util::run_scaled(graph_node& first_node, execution_state& e)
{
  std::vector<ossia::data_type> orig_ins;
  orig_ins.reserve(first_node.inputs().size());
  for(auto& n : first_node.inputs())
  {
    orig_ins.push_back(n->data);
  }


  for(const auto& request : first_node.requested_tokens)
  {
    for(auto& n : first_node.inputs())
      ossia::apply(rescale_in{first_node.prev_date(), request}, n->data);

    first_node.run(request, e);
    for(auto& n : first_node.outputs())
      ossia::apply(rescale_out{first_node.prev_date(), request}, n->data);

    first_node.set_prev_date(request.date);

    // Restore original input for the next token
    for(std::size_t i = 0; i < first_node.inputs().size(); i++)
      first_node.inputs()[i]->data = orig_ins[i];
  }
}


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

graph::~graph()
{
  clear();
}

}
