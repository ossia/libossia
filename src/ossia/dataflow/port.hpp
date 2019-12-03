#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{
struct OSSIA_EXPORT port
{
  data_type data;

  enum scope_t : uint8_t
  {
    none = 1 << 0,
    local = 1 << 1,
    global = 1 << 2,
    both = local | global
  };

  scope_t scope{scope_t::both};

protected:
  port(data_type&& d) noexcept : data{std::move(d)}
  {
  }

  port() = delete;
  port(const port&) = delete;
  port(port&&) = delete;
  port& operator=(const port&) = delete;
  port& operator=(port&&) = delete;
};

struct OSSIA_EXPORT inlet : public port
{
  inlet(data_type&& d) noexcept : port{std::move(d)}
  {
  }
  inlet(data_type&& d, destination_t dest) noexcept
      : port{std::move(d)}, address{std::move(dest)}
  {
  }

  inlet(data_type&& d, ossia::net::parameter_base& addr) noexcept
      : port{std::move(d)}, address{&addr}
  {
  }

  inlet(data_type&& d, graph_edge& edge) noexcept : port{std::move(d)}
  {
    sources.push_back(&edge);
  }

  ~inlet()
  {

  }

  void connect(graph_edge* e) noexcept
  {
    auto it = ossia::find(sources, e);
    if (it == sources.end())
      sources.push_back(e);
  }

  void disconnect(graph_edge* e) noexcept
  {
    ossia::remove_erase(sources, e);
  }

  destination_t address;
  ossia::small_vector<graph_edge*, 2> sources;
};

struct OSSIA_EXPORT outlet : public port
{
  outlet(data_type&& d) noexcept : port{std::move(d)}
  {
  }
  outlet(data_type&& d, destination_t dest) noexcept
      : port{std::move(d)}, address{std::move(dest)}
  {
  }

  outlet(data_type&& d, ossia::net::parameter_base& addr) noexcept
      : port{std::move(d)}, address{&addr}
  {
  }

  outlet(data_type&& d, graph_edge& edge) noexcept : port{std::move(d)}
  {
    targets.push_back(&edge);
  }

  ~outlet()
  {

  }

  void connect(graph_edge* e) noexcept
  {
    auto it = ossia::find(targets, e);
    if (it == targets.end())
      targets.push_back(e);
  }

  void disconnect(graph_edge* e) noexcept
  {
    ossia::remove_erase(targets, e);
  }

  void write(execution_state& e);

  destination_t address;
  ossia::small_vector<graph_edge*, 2> targets;
};

template <typename T, typename... Args>
inlet_ptr make_inlet(Args&&... args)
{
  return new inlet(T{}, std::forward<Args>(args)...);
}
template <typename T, typename... Args>
outlet_ptr make_outlet(Args&&... args)
{
  return new outlet(T{}, std::forward<Args>(args)...);
}



struct audio_inlet : public ossia::inlet
{
  audio_inlet(): ossia::inlet{ossia::audio_port{}} { }
  const ossia::audio_port& operator*() const noexcept { return *this->data.target<ossia::audio_port>(); }
  const ossia::audio_port* operator->() const noexcept { return this->data.target<ossia::audio_port>(); }
  ossia::audio_port& operator*() noexcept { return *this->data.target<ossia::audio_port>(); }
  ossia::audio_port* operator->() noexcept { return this->data.target<ossia::audio_port>(); }
};
struct midi_inlet : public ossia::inlet
{
  midi_inlet(): ossia::inlet{ossia::midi_port{}} { }
  const ossia::midi_port& operator*() const noexcept { return *this->data.target<ossia::midi_port>(); }
  const ossia::midi_port* operator->() const noexcept { return this->data.target<ossia::midi_port>(); }
  ossia::midi_port& operator*()  noexcept { return *this->data.target<ossia::midi_port>(); }
  ossia::midi_port* operator->()  noexcept { return this->data.target<ossia::midi_port>(); }
};
struct value_inlet : public ossia::inlet
{
  value_inlet(): ossia::inlet{ossia::value_port{}} { }
  const ossia::value_port& operator*() const noexcept { return *this->data.target<ossia::value_port>(); }
  const ossia::value_port* operator->() const noexcept { return this->data.target<ossia::value_port>(); }
  ossia::value_port& operator*()  noexcept { return *this->data.target<ossia::value_port>(); }
  ossia::value_port* operator->()  noexcept { return this->data.target<ossia::value_port>(); }
};
struct audio_outlet : public ossia::outlet
{
  audio_outlet(): ossia::outlet{ossia::audio_port{}} { }
  const ossia::audio_port& operator*() const noexcept { return *this->data.target<ossia::audio_port>(); }
  const ossia::audio_port* operator->() const noexcept { return this->data.target<ossia::audio_port>(); }
  ossia::audio_port& operator*()  noexcept { return *this->data.target<ossia::audio_port>(); }
  ossia::audio_port* operator->()  noexcept { return this->data.target<ossia::audio_port>(); }
};
struct midi_outlet : public ossia::outlet
{
  midi_outlet(): ossia::outlet{ossia::midi_port{}} { }
  const ossia::midi_port& operator*() const noexcept { return *this->data.target<ossia::midi_port>(); }
  const ossia::midi_port* operator->() const noexcept { return this->data.target<ossia::midi_port>(); }
  ossia::midi_port& operator*()  noexcept { return *this->data.target<ossia::midi_port>(); }
  ossia::midi_port* operator->()  noexcept { return this->data.target<ossia::midi_port>(); }
};
struct value_outlet : public ossia::outlet
{
  value_outlet(): ossia::outlet{ossia::value_port{}} { }
  const ossia::value_port& operator*() const noexcept { return *this->data.target<ossia::value_port>(); }
  const ossia::value_port* operator->() const noexcept { return this->data.target<ossia::value_port>(); }
  ossia::value_port& operator*()  noexcept { return *this->data.target<ossia::value_port>(); }
  ossia::value_port* operator->()  noexcept { return this->data.target<ossia::value_port>(); }
};
}
