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
  enum scope_t : uint8_t
  {
    none = 1 << 0,
    local = 1 << 1,
    global = 1 << 2,
    both = local | global
  };

  scope_t scope{scope_t::both};

protected:
  port() = default;
  port(const port&) = delete;
  port(port&&) = delete;
  port& operator=(const port&) = delete;
  port& operator=(port&&) = delete;
};

struct value_inlet;
struct OSSIA_EXPORT inlet : public port
{
protected:
  inlet() noexcept = default;

  inlet(destination_t dest) noexcept
      : address{std::move(dest)}
  {
  }

  inlet(ossia::net::parameter_base& addr) noexcept
      : address{&addr}
  {
  }

  inlet(graph_edge& edge) noexcept
  {
    sources.push_back(&edge);
  }

public:
  virtual ~inlet();

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

  virtual std::size_t which() const noexcept = 0;

  template<typename T>
  T* target() noexcept;
  template<typename T>
  const T* target() const noexcept;
  template<typename T>
  T& cast() noexcept;
  template<typename T>
  const T& cast() const noexcept;
  template<typename T>
  auto visit(const T& t);
  template<typename T>
  auto visit(const T& t) const;

  auto& cables() noexcept { return sources; }
  auto& cables() const noexcept { return sources; }

  virtual void pre_process();
  virtual void post_process();

  destination_t address;
  ossia::small_vector<graph_edge*, 2> sources;
  ossia::small_vector<value_inlet*, 2> child_inlets;

  friend struct audio_inlet;
  friend struct value_inlet;
  friend struct midi_inlet;
};

struct OSSIA_EXPORT outlet : public port
{
protected:
  outlet() noexcept = default;
  outlet(destination_t dest) noexcept
      : address{std::move(dest)}
  {
  }

  outlet(ossia::net::parameter_base& addr) noexcept
      : address{&addr}
  {
  }

  outlet(graph_edge& edge) noexcept
  {
    targets.push_back(&edge);
  }

public:
  virtual ~outlet();

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

  virtual std::size_t which() const noexcept = 0;

  template<typename T>
  T* target() noexcept;
  template<typename T>
  const T* target() const noexcept;
  template<typename T>
  T& cast() noexcept;
  template<typename T>
  const T& cast() const noexcept;
  template<typename T>
  auto visit(const T& t);
  template<typename T>
  auto visit(const T& t) const;

  virtual void pre_process();
  virtual void post_process();

  auto& cables() noexcept { return targets; }
  auto& cables() const noexcept { return targets; }

  destination_t address;
  ossia::small_vector<graph_edge*, 2> targets;
  ossia::small_vector<value_inlet*, 2> child_inlets;

  friend struct audio_outlet;
  friend struct value_outlet;
  friend struct midi_outlet;
};

struct OSSIA_EXPORT audio_inlet : public ossia::inlet
{
  audio_inlet() noexcept = default;

  audio_inlet(destination_t dest) noexcept
      : inlet{std::move(dest)}
  {
  }

  audio_inlet(ossia::net::parameter_base& addr) noexcept
      : inlet{&addr}
  {
  }

  audio_inlet(graph_edge& edge) noexcept
  {
    sources.push_back(&edge);
  }

  ~audio_inlet();

  const ossia::audio_port& operator*() const noexcept { return data; }
  const ossia::audio_port* operator->() const noexcept { return &data; }
  ossia::audio_port& operator*() noexcept { return data; }
  ossia::audio_port* operator->() noexcept { return &data; }

  std::size_t which() const noexcept final override { return audio_port::which; }

  ossia::audio_port data;
};

struct OSSIA_EXPORT midi_inlet : public ossia::inlet
{
  midi_inlet() noexcept = default;

  midi_inlet(destination_t dest) noexcept
      : inlet{std::move(dest)}
  {
  }

  midi_inlet(ossia::net::parameter_base& addr) noexcept
      : inlet{&addr}
  {
  }

  midi_inlet(graph_edge& edge) noexcept
  {
    sources.push_back(&edge);
  }

  ~midi_inlet();

  const ossia::midi_port& operator*() const noexcept { return data; }
  const ossia::midi_port* operator->() const noexcept { return &data; }
  ossia::midi_port& operator*() noexcept { return data; }
  ossia::midi_port* operator->() noexcept { return &data; }

  std::size_t which() const noexcept final override { return midi_port::which; }

  ossia::midi_port data;
};

struct OSSIA_EXPORT value_inlet : public ossia::inlet
{
  value_inlet() noexcept = default;

  value_inlet(destination_t dest) noexcept
      : inlet{std::move(dest)}
  {
  }

  value_inlet(ossia::net::parameter_base& addr) noexcept
      : inlet{&addr}
  {
  }

  value_inlet(graph_edge& edge) noexcept
  {
    sources.push_back(&edge);
  }


  ~value_inlet();

  const ossia::value_port& operator*() const noexcept { return data; }
  const ossia::value_port* operator->() const noexcept { return &data; }
  ossia::value_port& operator*() noexcept { return data; }
  ossia::value_port* operator->() noexcept { return &data; }

  std::size_t which() const noexcept final override { return value_port::which; }

  ossia::value_port data;
};

struct audio_outlet;

OSSIA_EXPORT
void process_audio_out_mono(ossia::audio_outlet& audio_out);

OSSIA_EXPORT
void process_audio_out_general(ossia::audio_outlet& audio_out);

OSSIA_EXPORT
void process_audio_out_mono(ossia::audio_port& i, ossia::audio_outlet& audio_out);

OSSIA_EXPORT
void process_audio_out_general(ossia::audio_port& i, ossia::audio_outlet& audio_out);

struct OSSIA_EXPORT audio_outlet : public ossia::outlet
{
  audio_outlet() noexcept
  {
    init();
  }

  audio_outlet(destination_t dest) noexcept
      : outlet{std::move(dest)}
  {
    init();
  }

  audio_outlet(ossia::net::parameter_base& addr) noexcept
      : outlet{&addr}
  {
    init();
  }

  audio_outlet(graph_edge& edge) noexcept
  {
    targets.push_back(&edge);
    init();
  }

  ~audio_outlet();

  const ossia::audio_port& operator*() const noexcept { return data; }
  const ossia::audio_port* operator->() const noexcept { return &data; }
  ossia::audio_port& operator*() noexcept { return data; }
  ossia::audio_port* operator->() noexcept { return &data; }

  std::size_t which() const noexcept final override { return audio_port::which; }

  void post_process() override;

  double gain{1.};
  pan_weight pan{1., 1.};

  ossia::value_inlet gain_inlet;
  ossia::value_inlet pan_inlet;

  ossia::audio_port data;
  bool has_gain{};

private:
  void init() noexcept
  {
    this->child_inlets.resize(2);
    this->child_inlets[0] = &gain_inlet;
    this->child_inlets[1] = &pan_inlet;
  }
};

#if BOOST_VERSION >= 107200
static_assert(noexcept(pan_weight{}));
static_assert(noexcept(value_inlet{}));
static_assert(noexcept(std::allocator<ossia::outlet>{}));
static_assert(noexcept(audio_outlet{}));
#endif

struct OSSIA_EXPORT midi_outlet : public ossia::outlet
{
  midi_outlet() noexcept = default;

  midi_outlet(destination_t dest) noexcept
      : outlet{std::move(dest)}
  {
  }

  midi_outlet(ossia::net::parameter_base& addr) noexcept
      : outlet{&addr}
  {
  }

  midi_outlet(graph_edge& edge) noexcept
  {
    targets.push_back(&edge);
  }
  ~midi_outlet();

  const ossia::midi_port& operator*() const noexcept { return data; }
  const ossia::midi_port* operator->() const noexcept { return &data; }
  ossia::midi_port& operator*() noexcept { return data; }
  ossia::midi_port* operator->() noexcept { return &data; }

  std::size_t which() const noexcept final override { return midi_port::which; }

  ossia::midi_port data;
};

struct OSSIA_EXPORT value_outlet : public ossia::outlet
{
  value_outlet() noexcept = default;

  value_outlet(destination_t dest) noexcept
      : outlet{std::move(dest)}
  {
  }

  value_outlet(ossia::net::parameter_base& addr) noexcept
      : outlet{&addr}
  {
  }

  value_outlet(graph_edge& edge) noexcept
  {
    targets.push_back(&edge);
  }
  ~value_outlet();

  const ossia::value_port& operator*() const noexcept { return data; }
  const ossia::value_port* operator->() const noexcept { return &data; }
  ossia::value_port& operator*() noexcept { return data; }
  ossia::value_port* operator->() noexcept { return &data; }

  std::size_t which() const noexcept final override { return value_port::which; }

  ossia::value_port data;
};


struct texture_port {
  static const constexpr int which = 3;
};

struct OSSIA_EXPORT texture_inlet : public ossia::inlet
{
  texture_inlet() noexcept = default;

  texture_inlet(destination_t dest) noexcept
      : inlet{std::move(dest)}
  {
  }

  texture_inlet(ossia::net::parameter_base& addr) noexcept
      : inlet{&addr}
  {
  }

  texture_inlet(graph_edge& edge) noexcept
  {
    sources.push_back(&edge);
  }

  ~texture_inlet();

  std::size_t which() const noexcept final override { return texture_port::which; }
};

struct OSSIA_EXPORT texture_outlet : public ossia::outlet
{
  texture_outlet() noexcept = default;

  texture_outlet(destination_t dest) noexcept
      : outlet{std::move(dest)}
  {
  }

  texture_outlet(ossia::net::parameter_base& addr) noexcept
      : outlet{&addr}
  {
  }

  texture_outlet(graph_edge& edge) noexcept
  {
    targets.push_back(&edge);
  }
  ~texture_outlet();

  std::size_t which() const noexcept final override { return texture_port::which; }
};

template<typename T>
inline T* inlet::target() noexcept
{
  if constexpr(std::is_same_v<T, audio_port>)
  {
    if(which() == 0)
    {
      return &static_cast<audio_inlet*>(this)->data;
    }
    else
    {
      return (audio_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, midi_port>)
  {
    if(which() == 1)
    {
      return &static_cast<midi_inlet*>(this)->data;
    }
    else
    {
      return (midi_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, value_port>)
  {
    if(which() == 2)
    {
      return &static_cast<value_inlet*>(this)->data;
    }
    else
    {
      return (value_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, texture_inlet>)
  {
    if(which() == 3)
    {
      return static_cast<texture_inlet*>(this);
    }
    else
    {
      return (texture_inlet*)nullptr;
    }
  }
  else
  {
    static_assert(std::is_same_v<T, struct _>, "Invalid type requested");
  }
}

template<typename T>
inline T* outlet::target() noexcept
{
  if constexpr(std::is_same_v<T, audio_port>)
  {
    if(which() == 0)
    {
      return &static_cast<audio_outlet*>(this)->data;
    }
    else
    {
      return (audio_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, midi_port>)
  {
    if(which() == 1)
    {
      return &static_cast<midi_outlet*>(this)->data;
    }
    else
    {
      return (midi_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, value_port>)
  {
    if(which() == 2)
    {
      return &static_cast<value_outlet*>(this)->data;
    }
    else
    {
      return (value_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, texture_outlet>)
  {
    if(which() == 3)
    {
      return static_cast<texture_outlet*>(this);
    }
    else
    {
      return (texture_outlet*)nullptr;
    }
  }
  else
  {
    static_assert(std::is_same_v<T, struct _>, "Invalid type requested");
  }
}
template<typename T>
inline const T* inlet::target() const noexcept
{
  if constexpr(std::is_same_v<T, audio_port>)
  {
    if(which() == 0)
    {
      return &static_cast<const audio_inlet*>(this)->data;
    }
    else
    {
      return (const audio_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, midi_port>)
  {
    if(which() == 1)
    {
      return &static_cast<const midi_inlet*>(this)->data;
    }
    else
    {
      return (const midi_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, value_port>)
  {
    if(which() == 2)
    {
      return &static_cast<const value_inlet*>(this)->data;
    }
    else
    {
      return (const value_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, texture_inlet>)
  {
    if(which() == 3)
    {
      return static_cast<const texture_inlet*>(this);
    }
    else
    {
      return (const texture_inlet*)nullptr;
    }
  }
  else
  {
    static_assert(std::is_same_v<T, struct _>, "Invalid type requested");
  }
}

template<typename T>
inline const T* outlet::target() const noexcept
{
  if constexpr(std::is_same_v<T, audio_port>)
  {
    if(which() == 0)
    {
      return &static_cast<const audio_outlet*>(this)->data;
    }
    else
    {
      return (const audio_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, midi_port>)
  {
    if(which() == 1)
    {
      return &static_cast<const midi_outlet*>(this)->data;
    }
    else
    {
      return (const midi_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, value_port>)
  {
    if(which() == 2)
    {
      return &static_cast<const value_outlet*>(this)->data;
    }
    else
    {
      return (const value_port*)nullptr;
    }
  }
  else if constexpr(std::is_same_v<T, texture_outlet>)
  {
    if(which() == 3)
    {
      return static_cast<const texture_outlet*>(this);
    }
    else
    {
      return (const texture_outlet*)nullptr;
    }
  }
  else
  {
    static_assert(std::is_same_v<T, struct _>, "Invalid type requested");
  }
}



template<typename T>
inline T& inlet::cast() noexcept
{
  if constexpr(std::is_same_v<T, audio_port>)
  {
    return static_cast<audio_inlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, midi_port>)
  {
    return static_cast<midi_inlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, value_port>)
  {
    return static_cast<value_inlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, texture_inlet>)
  {
    return static_cast<texture_inlet&>(*this);
  }
  else
  {
    static_assert(std::is_same_v<T, struct _>, "Invalid type requested");
  }
}

template<typename T>
inline T& outlet::cast() noexcept
{
  if constexpr(std::is_same_v<T, audio_port>)
  {
    return static_cast<audio_outlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, midi_port>)
  {
    return static_cast<midi_outlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, value_port>)
  {
    return static_cast<value_outlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, texture_outlet>)
  {
    return static_cast<texture_outlet&>(*this);
  }
  else
  {
    static_assert(std::is_same_v<T, struct _>, "Invalid type requested");
  }
}
template<typename T>
inline const T& inlet::cast() const noexcept
{
  if constexpr(std::is_same_v<T, audio_port>)
  {
    return static_cast<const audio_inlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, midi_port>)
  {
    return static_cast<const midi_inlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, value_port>)
  {
    return static_cast<const value_inlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, texture_inlet>)
  {
    return static_cast<const texture_inlet&>(*this);
  }
  else
  {
    static_assert(std::is_same_v<T, struct _>, "Invalid type requested");
  }
}

template<typename T>
inline const T& outlet::cast() const noexcept
{
  if constexpr(std::is_same_v<T, audio_port>)
  {
    return static_cast<const audio_outlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, midi_port>)
  {
    return static_cast<const midi_outlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, value_port>)
  {
    return static_cast<const value_outlet*>(this)->data;
  }
  else if constexpr(std::is_same_v<T, texture_outlet>)
  {
    return static_cast<const texture_outlet&>(*this);
  }
  else
  {
    static_assert(std::is_same_v<T, struct _>, "Invalid type requested");
  }
}



template<typename T>
inline auto inlet::visit(const T& t)
{
  switch(which())
  {
    case 0: return t(static_cast<audio_inlet*>(this)->data);
    case 1: return t(static_cast<midi_inlet*>(this)->data);
    case 2: return t(static_cast<value_inlet*>(this)->data);
    //case 3: return t(static_cast<texture_inlet&>(*this));
  }

  if constexpr(std::is_invocable_v<T>)
    return t();
}

template<typename T>
inline auto outlet::visit(const T& t)
{
  switch(which())
  {
    case 0: return t(static_cast<audio_outlet*>(this)->data);
    case 1: return t(static_cast<midi_outlet*>(this)->data);
    case 2: return t(static_cast<value_outlet*>(this)->data);
    //case 3: return t(static_cast<texture_outlet&>(*this));
  }

  if constexpr(std::is_invocable_v<T>)
    return t();
}
template<typename T>
inline auto inlet::visit(const T& t) const
{
  switch(which())
  {
    case 0: return t(static_cast<const audio_inlet*>(this)->data);
    case 1: return t(static_cast<const midi_inlet*>(this)->data);
    case 2: return t(static_cast<const value_inlet*>(this)->data);
    //case 3: return t(static_cast<const texture_inlet&>(*this));
  }

  if constexpr(std::is_invocable_v<T>)
    return t();
}
template<typename T>
inline auto outlet::visit(const T& t) const
{
  switch(which())
  {
    case 0: return t(static_cast<const audio_outlet*>(this)->data);
    case 1: return t(static_cast<const midi_outlet*>(this)->data);
    case 2: return t(static_cast<const value_outlet*>(this)->data);
    //case 3: return t(static_cast<const texture_outlet&>(*this));
  }

  if constexpr(std::is_invocable_v<T>)
    return t();
}


}
