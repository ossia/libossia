#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_PIPEWIRE)
#if __has_include(<pipewire/pipewire.h>) && __has_include(<spa/param/latency-utils.h>)
#define OSSIA_AUDIO_PIPEWIRE 1
#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/dylib_loader.hpp>
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/thread.hpp>

#include <pipewire/core.h>
#include <pipewire/filter.h>
#include <pipewire/pipewire.h>
#include <spa/pod/builder.h>
#include <spa/utils/result.h>

#include <cmath>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <stdexcept>

#include <spa/param/latency-utils.h>

namespace ossia
{
class libpipewire
{
public:
  decltype(&::pw_init) init{};
  decltype(&::pw_deinit) deinit{};

  decltype(&::pw_context_new) context_new{};
  decltype(&::pw_context_connect) context_connect{};
  decltype(&::pw_context_destroy) context_destroy{};

  decltype(&::pw_core_disconnect) core_disconnect{};

  decltype(&::pw_proxy_add_listener) proxy_add_listener{};
  decltype(&::pw_proxy_destroy) proxy_destroy{};

  decltype(&::pw_main_loop_new) main_loop_new{};
  decltype(&::pw_main_loop_destroy) main_loop_destroy{};
  decltype(&::pw_main_loop_quit) main_loop_quit{};
  decltype(&::pw_main_loop_run) main_loop_run{};
  decltype(&::pw_main_loop_get_loop) main_loop_get_loop{};

  decltype(&::pw_properties_new) properties_new{};
  decltype(&::pw_properties_free) properties_free{};
  decltype(&::pw_properties_get) properties_get{};

  decltype(&::pw_filter_new_simple) filter_new_simple{};
  decltype(&::pw_filter_get_node_id) filter_get_node_id{};
  decltype(&::pw_filter_get_properties) filter_get_properties{};
  decltype(&::pw_filter_add_port) filter_add_port{};
  decltype(&::pw_filter_destroy) filter_destroy{};
  decltype(&::pw_filter_connect) filter_connect{};
  decltype(&::pw_filter_get_dsp_buffer) filter_get_dsp_buffer{};

  static const libpipewire& instance()
  {
    static const libpipewire self;
    return self;
  }

private:
  dylib_loader library;

  libpipewire()
      : library("libpipewire-0.3.so.0")
  {
    // in terms of regex:
    // decltype\(&::([a-z_]+)\) [a-z_]+{};
    // \1 = library.symbol<decltype(&::\1)>("\1");
    init = library.symbol<decltype(&::pw_init)>("pw_init");
    deinit = library.symbol<decltype(&::pw_deinit)>("pw_deinit");

    context_new = library.symbol<decltype(&::pw_context_new)>("pw_context_new");
    context_connect
        = library.symbol<decltype(&::pw_context_connect)>("pw_context_connect");
    context_destroy
        = library.symbol<decltype(&::pw_context_destroy)>("pw_context_destroy");

    core_disconnect
        = library.symbol<decltype(&::pw_core_disconnect)>("pw_core_disconnect");

    proxy_add_listener
        = library.symbol<decltype(&::pw_proxy_add_listener)>("pw_proxy_add_listener");
    proxy_destroy = library.symbol<decltype(&::pw_proxy_destroy)>("pw_proxy_destroy");

    main_loop_new = library.symbol<decltype(&::pw_main_loop_new)>("pw_main_loop_new");
    main_loop_destroy
        = library.symbol<decltype(&::pw_main_loop_destroy)>("pw_main_loop_destroy");
    main_loop_quit = library.symbol<decltype(&::pw_main_loop_quit)>("pw_main_loop_quit");
    main_loop_run = library.symbol<decltype(&::pw_main_loop_run)>("pw_main_loop_run");
    main_loop_get_loop
        = library.symbol<decltype(&::pw_main_loop_get_loop)>("pw_main_loop_get_loop");

    properties_new = library.symbol<decltype(&::pw_properties_new)>("pw_properties_new");
    properties_free
        = library.symbol<decltype(&::pw_properties_free)>("pw_properties_free");
    properties_get = library.symbol<decltype(&::pw_properties_get)>("pw_properties_get");

    filter_new_simple
        = library.symbol<decltype(&::pw_filter_new_simple)>("pw_filter_new_simple");
    filter_get_node_id
        = library.symbol<decltype(&::pw_filter_get_node_id)>("pw_filter_get_node_id");
    filter_get_properties = library.symbol<decltype(&::pw_filter_get_properties)>(
        "pw_filter_get_properties");
    filter_add_port
        = library.symbol<decltype(&::pw_filter_add_port)>("pw_filter_add_port");
    filter_destroy = library.symbol<decltype(&::pw_filter_destroy)>("pw_filter_destroy");
    filter_connect = library.symbol<decltype(&::pw_filter_connect)>("pw_filter_connect");
    filter_get_dsp_buffer = library.symbol<decltype(&::pw_filter_get_dsp_buffer)>(
        "pw_filter_get_dsp_buffer");

    assert(init);
    assert(deinit);

    assert(context_new);
    assert(context_connect);
    assert(context_destroy);

    assert(core_disconnect);

    assert(proxy_destroy);

    assert(main_loop_new);
    assert(main_loop_destroy);
    assert(main_loop_quit);
    assert(main_loop_run);
    assert(main_loop_get_loop);

    assert(properties_new);
    assert(properties_free);
    assert(properties_get);

    assert(filter_new_simple);
    assert(filter_get_node_id);
    assert(filter_get_properties);
    assert(filter_add_port);
    assert(filter_destroy);
    assert(filter_connect);
    assert(filter_get_dsp_buffer);
  }
};

struct pipewire_context
{
  pw_main_loop* main_loop{};
  pw_loop* lp{};

  pw_context* context{};
  pw_core* core{};

  pw_registry* registry{};
  spa_hook registry_listener{};

  struct listened_port
  {
    uint32_t id{};
    pw_port* port{};
    std::unique_ptr<spa_hook> listener;
  };
  std::vector<listened_port> port_listener{};

  struct port_info
  {
    uint32_t id{};

    std::string format;
    std::string port_name;
    std::string port_alias;
    std::string object_path;
    std::string node_id;
    std::string port_id;

    bool physical{};
    bool terminal{};
    bool monitor{};
    pw_direction direction{};
  };

  struct node
  {
    std::vector<port_info> inputs;
    std::vector<port_info> outputs;
  };

  struct graph
  {
    ossia::hash_map<uint32_t, node> physical_audio;
    ossia::hash_map<uint32_t, node> physical_midi;
    ossia::hash_map<uint32_t, node> software_audio;
    ossia::hash_map<uint32_t, node> software_midi;

    void for_each_port(auto func)
    {
      for(auto& map : {physical_audio, physical_midi, software_audio, software_midi})
      {
        for(auto& [id, node] : map)
        {
          for(auto& port : node.inputs)
            func(port);
          for(auto& port : node.outputs)
            func(port);
        }
      }
    }

    void remove_port(uint32_t id)
    {
      for(auto map : {&physical_audio, &physical_midi, &software_audio, &software_midi})
      {
        for(auto& [_, node] : *map)
        {
          ossia::remove_erase_if(
              node.inputs, [id](const port_info& p) { return p.id == id; });
          ossia::remove_erase_if(
              node.outputs, [id](const port_info& p) { return p.id == id; });
        }
      }
    }
  } current_graph;

  int sync{};

  const libpipewire& pw = libpipewire::instance();
  explicit pipewire_context()
  {
    /// Initialize the PipeWire main loop, context, etc.
    int argc = 0;
    char* argv[] = {NULL};
    char** aa = argv;
    pw.init(&argc, &aa);

    this->main_loop = pw.main_loop_new(nullptr);
    if(!this->main_loop)
    {
      ossia::logger().error("PipeWire: main_loop_new failed!");
      return;
    }

    this->lp = pw.main_loop_get_loop(this->main_loop);
    if(!lp)
    {
      ossia::logger().error("PipeWire: main_loop_get_loop failed!");
      return;
    }

    this->context = pw.context_new(lp, nullptr, 0);
    if(!this->context)
    {
      ossia::logger().error("PipeWire: context_new failed!");
      return;
    }

    this->core = pw.context_connect(this->context, nullptr, 0);
    if(!this->core)
    {
      ossia::logger().error("PipeWire: context_connect failed!");
      return;
    }

    this->registry = pw_core_get_registry(this->core, PW_VERSION_REGISTRY, 0);
    if(!this->registry)
    {
      ossia::logger().error("PipeWire: core_get_registry failed!");
      return;
    }

    // Register a listener which will listen on when ports are added / removed
    spa_zero(registry_listener);
    static constexpr const struct pw_port_events port_events
        = {.version = PW_VERSION_PORT_EVENTS,
           .info =
               [](void* object, const pw_port_info* info) {
      ((pipewire_context*)object)->register_port(info);
    },
           .param = {}};

    static constexpr const struct pw_registry_events registry_events = {
        .version = PW_VERSION_REGISTRY_EVENTS,
        .global =
            [](void* object, uint32_t id, uint32_t /*permissions*/, const char* type,
               uint32_t /*version*/, const struct spa_dict* /*props*/) {
      pipewire_context& self = *(pipewire_context*)object;

      // When a port is added:
      if(strcmp(type, PW_TYPE_INTERFACE_Port) == 0)
      {
        auto port
            = (pw_port*)pw_registry_bind(self.registry, id, type, PW_VERSION_PORT, 0);
        self.port_listener.push_back({id, port, std::make_unique<spa_hook>()});
        auto& l = self.port_listener.back();

        pw_port_add_listener(l.port, l.listener.get(), &port_events, &self);
      }
        },
        .global_remove =
            [](void* object, uint32_t id) {
      pipewire_context& self = *(pipewire_context*)object;

      // When a port is removed:
      // Remove from the graph
      self.current_graph.remove_port(id);

      // Remove from the listeners
      auto it = ossia::find_if(
          self.port_listener, [&](const listened_port& l) { return l.id == id; });
      if(it != self.port_listener.end())
      {
        libpipewire::instance().proxy_destroy((pw_proxy*)it->port);
        self.port_listener.erase(it);
      }
        },
        };

    // Start listening
    pw_registry_add_listener(
        this->registry, &this->registry_listener, &registry_events, this);

    synchronize();
  }

  int pending{};
  int done{};
  void synchronize()
  {
    pending = 0;
    done = 0;

    if(!core)
      return;

    spa_hook core_listener;

    static constexpr struct pw_core_events core_events = {
        .version = PW_VERSION_CORE_EVENTS,
        .info = {},
        .done =
            [](void* object, uint32_t id, int seq) {
      auto& self = *(pipewire_context*)object;
      if(id == PW_ID_CORE && seq == self.pending)
      {
        self.done = 1;
        libpipewire::instance().main_loop_quit(self.main_loop);
      }
    },
        .ping = {},
        .error = {},
        .remove_id = {},
        .bound_id = {},
        .add_mem = {},
        .remove_mem = {},
#if defined(PW_CORE_EVENT_BOUND_PROPS)
        .bound_props = {},
#endif
    };

    spa_zero(core_listener);
    pw_core_add_listener(core, &core_listener, &core_events, this);

    pending = pw_core_sync(core, PW_ID_CORE, 0);
    while(!done)
    {
      pw.main_loop_run(this->main_loop);
    }
    spa_hook_remove(&core_listener);
  }

  pw_proxy* link_ports(uint32_t out_port, uint32_t in_port)
  {
    auto props = pw.properties_new(
        PW_KEY_LINK_OUTPUT_PORT, std::to_string(out_port).c_str(),
        PW_KEY_LINK_INPUT_PORT, std::to_string(in_port).c_str(), nullptr);

    auto proxy = (pw_proxy*)pw_core_create_object(
        this->core, "link-factory", PW_TYPE_INTERFACE_Link, PW_VERSION_LINK,
        &props->dict, 0);

    if(!proxy)
    {
      ossia::logger().error("PipeWire: could not allocate link");
      pw.properties_free(props);
      return nullptr;
    }

    synchronize();
    pw.properties_free(props);
    return proxy;
  }

  void register_port(const pw_port_info* info)
  {
    const spa_dict_item* item{};

    port_info p;
    p.id = info->id;

    spa_dict_for_each(item, info->props)
    {
      std::string_view k{item->key}, v{item->value};
      if(k == "format.dsp")
        p.format = v;
      else if(k == "port.name")
        p.port_name = v;
      else if(k == "port.alias")
        p.port_alias = v;
      else if(k == "object.path")
        p.object_path = v;
      else if(k == "port.id")
        p.port_id = v;
      else if(k == "node.id")
        p.node_id = v;
      else if(k == "port.physical" && v == "true")
        p.physical = true;
      else if(k == "port.terminal" && v == "true")
        p.terminal = true;
      else if(k == "port.monitor" && v == "true")
        p.monitor = true;
      else if(k == "port.direction")
      {
        if(v == "out")
        {
          p.direction = pw_direction::SPA_DIRECTION_OUTPUT;
        }
        else
        {
          p.direction = pw_direction::SPA_DIRECTION_INPUT;
        }
      }
    }

    if(p.node_id.empty())
      return;

    const auto nid = std::stoul(p.node_id);
    if(p.physical)
    {
      if(p.format.find("audio") != p.format.npos)
      {
        if(p.direction == pw_direction::SPA_DIRECTION_OUTPUT)
          this->current_graph.physical_audio[nid].outputs.push_back(std::move(p));
        else
          this->current_graph.physical_audio[nid].inputs.push_back(std::move(p));
      }
      else if(p.format.find("midi") != p.format.npos)
      {
        if(p.direction == pw_direction::SPA_DIRECTION_OUTPUT)
          this->current_graph.physical_midi[nid].outputs.push_back(std::move(p));
        else
          this->current_graph.physical_midi[nid].inputs.push_back(std::move(p));
      }
      else
      {
        // TODO, video ?
      }
    }
    else
    {
      if(p.format.find("audio") != p.format.npos)
      {
        if(p.direction == pw_direction::SPA_DIRECTION_OUTPUT)
          this->current_graph.software_audio[nid].outputs.push_back(std::move(p));
        else
          this->current_graph.software_audio[nid].inputs.push_back(std::move(p));
      }
      else if(p.format.find("midi") != p.format.npos)
      {
        if(p.direction == pw_direction::SPA_DIRECTION_OUTPUT)
          this->current_graph.software_midi[nid].outputs.push_back(std::move(p));
        else
          this->current_graph.software_midi[nid].inputs.push_back(std::move(p));
      }
      else
      {
        // TODO, video ?
      }
    }
  }

  int get_fd() const noexcept
  {
    if(!this->lp)
      return -1;

    auto spa_callbacks = this->lp->control->iface.cb;
    auto spa_loop_methods = (const spa_loop_control_methods*)spa_callbacks.funcs;
    if(spa_loop_methods->get_fd)
      return spa_loop_methods->get_fd(spa_callbacks.data);
    else
      return -1;
  }

  ~pipewire_context()
  {
    if(this->registry)
      pw.proxy_destroy((pw_proxy*)this->registry);
    for(auto& [id, p, l] : this->port_listener)
      if(l)
        pw.proxy_destroy((pw_proxy*)p);
    if(this->core)
      pw.core_disconnect(this->core);
    if(this->context)
      pw.context_destroy(this->context);
    if(this->main_loop)
      pw.main_loop_destroy(this->main_loop);

    pw.deinit();
  }
};

struct audio_setup
{
  std::string name;
  std::string card_in;
  std::string card_out;

  std::vector<std::string> inputs;
  std::vector<std::string> outputs;

  int rate{};
  int buffer_size{};
};

class pipewire_audio_protocol : public audio_engine
{
public:
  struct port
  {
  };

  std::shared_ptr<pipewire_context> loop{};
  pw_filter* filter{};
  std::vector<pw_proxy*> links{};

  explicit pipewire_audio_protocol(
      std::shared_ptr<pipewire_context> loop, const audio_setup& setup)
  {
    auto& pw = libpipewire::instance();

    static constexpr const struct pw_filter_events filter_events = {
        .version = PW_VERSION_FILTER_EVENTS,
        .destroy = {},
        .state_changed = {},
        .io_changed = {},
        .param_changed = {},
        .add_buffer = {},
        .remove_buffer = {},
        .process = on_process,
        .drained = {},
#if PW_VERSION_CORE > 3
        .command = {},
#endif
    };

    this->loop = loop;

    auto lp = loop->lp;
    // clang-format off
    // Create the filter (the main pipewire object which will represent the
    // software)
    auto filter_props{
        pw.properties_new(
            PW_KEY_MEDIA_TYPE, "Audio",
            PW_KEY_MEDIA_CATEGORY, "Duplex",
            PW_KEY_MEDIA_ROLE, "DSP",
            PW_KEY_MEDIA_NAME, "ossia",
            PW_KEY_NODE_NAME, "ossia",
            PW_KEY_NODE_GROUP, "group.dsp.0",
            PW_KEY_NODE_DESCRIPTION, "ossia score",
            PW_KEY_NODE_LATENCY, fmt::format("{}/{}", setup.buffer_size, setup.rate).c_str(),
            PW_KEY_NODE_FORCE_QUANTUM, fmt::format("{}", setup.buffer_size).c_str(),
            PW_KEY_NODE_LOCK_QUANTUM, "true",
            PW_KEY_NODE_TRANSPORT_SYNC, "true",
            //PW_KEY_NODE_RATE, fmt::format("{}/{}", 1, setup.rate).c_str(),
            PW_KEY_NODE_FORCE_RATE, fmt::format("{}", setup.rate).c_str(),
            //PW_KEY_NODE_LOCK_RATE, "true",
            //PW_KEY_NODE_ALWAYS_PROCESS, "true",
            PW_KEY_NODE_PAUSE_ON_IDLE, "false",
            PW_KEY_NODE_SUSPEND_ON_IDLE, "false",
            nullptr)};

    // clang-format on
    this->filter = pw.filter_new_simple(
        lp, setup.name.c_str(), filter_props, &filter_events, this);
    if(!this->filter)
    {
      throw std::runtime_error("PipeWire: could not create filter instance");
    }

    // Create the request ports
    for(std::size_t i = 0; i < setup.inputs.size(); i++)
    {
      auto p = (port*)pw.filter_add_port(
          this->filter, PW_DIRECTION_INPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS,
          sizeof(struct port),
          pw.properties_new(
              PW_KEY_FORMAT_DSP, "32 bit float mono audio", PW_KEY_PORT_NAME,
              setup.inputs[i].c_str(), NULL),
          NULL, 0);
      input_ports.push_back(p);
    }

    for(std::size_t i = 0; i < setup.outputs.size(); i++)
    {
      auto p = (port*)pw.filter_add_port(
          this->filter, PW_DIRECTION_OUTPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS,
          sizeof(struct port),
          pw.properties_new(
              PW_KEY_FORMAT_DSP, "32 bit float mono audio", PW_KEY_PORT_NAME,
              setup.outputs[i].c_str(), NULL),
          NULL, 0);
      output_ports.push_back(p);
    }

    if(pw.filter_connect(this->filter, PW_FILTER_FLAG_RT_PROCESS, nullptr, 0) < 0)
    {
      throw std::runtime_error("PipeWire: cannot connect");
    }

    // Wait until everything is registered with PipeWire
    this->loop->synchronize();
    {
      int k = 0;
      auto node_id = filter_node_id();
      while(node_id == 4294967295)
      {
        this->loop->synchronize();
        node_id = filter_node_id();

        if(k++; k > 100)
          return;
      }

      // Leave some time to resolve the ports
      k = 0;
      const auto num_local_ins = this->input_ports.size();
      const auto num_local_outs = this->output_ports.size();
      auto& this_node = this->loop->current_graph.software_audio[node_id];
      while(this_node.inputs.size() < num_local_ins
            || this_node.outputs.size() < num_local_outs)
      {
        this->loop->synchronize();
        if(k++; k > 100)
          return;
      }
    }

    activated = true;
    this->effective_buffer_size = setup.buffer_size;
    this->effective_sample_rate = setup.rate;
    this->effective_inputs = setup.inputs.size();
    this->effective_outputs = setup.outputs.size();
  }

  uint32_t filter_node_id() { return this->loop->pw.filter_get_node_id(this->filter); }

  void autoconnect()
  {
    auto node_id = filter_node_id();

    std::vector<std::pair<std::optional<uint32_t>, std::optional<uint32_t>>>
        phys_in_to_ossia;
    std::vector<std::pair<std::optional<uint32_t>, std::optional<uint32_t>>>
        ossia_to_phys_out;

    // Link to the first physical soundcard we see
    for(auto& [node, ports] : loop->current_graph.physical_audio)
    {
      auto& [out, in] = ports;

      // The soundcard outputs are input ports
      for(auto& port : in)
      {
        phys_in_to_ossia.emplace_back(port.id, std::nullopt);
      }

      // The soundcard inputs are output ports
      for(auto& port : out)
      {
        ossia_to_phys_out.emplace_back(std::nullopt, port.id);
      }
    }

    // Enumerate our matching local ports
    for(auto& [node, ports] : loop->current_graph.software_audio)
    {
      if(node == node_id)
      {
        auto& [in, out] = ports;

        // Connect our inputs to the soundcard inputs
        for(std::size_t i = 0; i < in.size(); i++)
        {
          if(i >= phys_in_to_ossia.size())
            break;

          phys_in_to_ossia[i].second = in[i].id;
        }

        // Connect our outputs to the soundcard inputs
        for(std::size_t i = 0; i < out.size(); i++)
        {
          if(i >= ossia_to_phys_out.size())
            break;

          ossia_to_phys_out[i].first = out[i].id;
        }
        break;
      }
    }

    // Connect as much as we can
    for(auto [phys, self] : phys_in_to_ossia)
    {
      if(phys && self)
      {
        if(auto link = this->loop->link_ports(*phys, *self))
          this->links.push_back(link);
      }
      else
      {
        break;
      }
    }

    for(auto [self, phys] : ossia_to_phys_out)
    {
      if(self && phys)
      {
        if(auto link = this->loop->link_ports(*self, *phys))
          this->links.push_back(link);
      }
      else
      {
        break;
      }
    }
  }

  void wait(int ms) override
  {
    if(!loop)
      return;

    using namespace std::chrono;
    using clk = high_resolution_clock;

    auto t0 = clk::now();
    auto t1 = clk::now();
    while(duration_cast<milliseconds>(t1 - t0).count() < ms)
    {
      pw_loop_iterate(loop->lp, ms);
      t1 = clk::now();
    }
  }

  bool running() const override
  {
    if(!this->loop)
      return false;
    return activated;
  }

  void stop() override
  {
    audio_engine::stop();

    if(!this->loop)
      return;

    if(!activated)
      return;

    auto& pw = libpipewire::instance();

    for(auto link : this->links)
      pw.proxy_destroy(link);

    pw.filter_destroy(this->filter);

    loop->synchronize();
    activated = false;
  }

  ~pipewire_audio_protocol() { stop(); }

  static void
  clear_buffers(pipewire_audio_protocol& self, uint32_t nframes, std::size_t outputs)
  {
    auto& pw = libpipewire::instance();
    for(std::size_t i = 0; i < outputs; i++)
    {
      auto chan = (float*)pw.filter_get_dsp_buffer(self.output_ports[i], nframes);
      if(chan)
        for(std::size_t j = 0; j < nframes; j++)
          chan[j] = 0.f;
    }

    return;
  }

  void do_process(uint32_t nframes, double secs)
  {
    const auto& pw = libpipewire::instance();

    tick_start();

    const auto inputs = input_ports.size();
    const auto outputs = output_ports.size();
    if(stop_processing)
    {
      tick_clear();
      clear_buffers(*this, nframes, outputs);
      return;
    }

    auto dummy = (float*)alloca(sizeof(float) * nframes);
    memset(dummy, 0, sizeof(float) * nframes);

    auto float_input = (float**)alloca(sizeof(float*) * inputs);
    auto float_output = (float**)alloca(sizeof(float*) * outputs);
    for(std::size_t i = 0; i < inputs; i++)
    {
      float_input[i] = (float*)pw.filter_get_dsp_buffer(input_ports[i], nframes);
      if(float_input[i] == nullptr)
        float_input[i] = dummy;
    }
    for(std::size_t i = 0; i < outputs; i++)
    {
      float_output[i] = (float*)pw.filter_get_dsp_buffer(output_ports[i], nframes);
      if(float_output[i] == nullptr)
        float_output[i] = dummy;
    }

    // Actual execution
    ossia::audio_tick_state ts{float_input,  float_output, (int)inputs,
                               (int)outputs, nframes,      secs};
    audio_tick(ts);
    tick_end();
  }

  static void on_process(void* userdata, struct spa_io_position* position)
  {
    [[maybe_unused]]
    static const thread_local auto _
        = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
      return 0;
    }();

    if(!userdata)
      return;

    auto& self = *(pipewire_audio_protocol*)userdata;
    uint32_t nframes = position->clock.duration;
    double current_time_ns = position->clock.nsec * 1e-9;
    while(nframes >= self.effective_buffer_size)
    {
      self.do_process(self.effective_buffer_size, current_time_ns);
      nframes -= self.effective_buffer_size;
      current_time_ns += double(self.effective_buffer_size) / self.effective_sample_rate;
    }

    if(nframes > 0)
    {
      self.do_process(self.effective_buffer_size, current_time_ns);
    }
  }

  std::vector<port*> input_ports;
  std::vector<port*> output_ports;
  bool activated{};
};

}
#endif
#endif
