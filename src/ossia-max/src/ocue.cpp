#include "ocue.utils.cpp"
#include "symbols.hpp"

#include <ossia/detail/json.hpp>

#include <ossia-max/src/ocue.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <boost/algorithm/string.hpp>

#include <rapidjson/prettywriter.h>

// Notes:
// - When we switch preset, the nodes in the preset becomes this ocue::'s running selection
// - Need "update" to add/remove nodes from the cue: just changing the local selection does nothing
// - namespace/X family of functions vs
// - select/all select deselect

using namespace ossia::max_binding;

t_class* ocue::max_class = {};

void* ossia_cue_create(t_symbol* s, long argc, t_atom* argv);
extern "C" OSSIA_MAX_EXPORT void ossia_cue_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.cue class
  ocue::max_class = class_new(
      "ossia.cue", (method)ossia_cue_create, (method)ocue::free, (long)sizeof(ocue), 0L,
      A_GIMME, 0);

#define ADDMETHOD_NOTHING(name) \
  class_addmethod(              \
      c, (method) + [](ocue* x, t_symbol*) { x->name(); }, #name, A_NOTHING, 0);
#define ADDMETHOD_NOTHING_(var, name) \
  class_addmethod(                    \
      c, (method) + [](ocue* x, t_symbol*) { x->var(); }, name, A_NOTHING, 0);

#define ADDMETHOD_GIMME(name)                                                      \
  class_addmethod(                                                                 \
      c,                                                                           \
      (method) +                                                                   \
          [](ocue* x, t_symbol*, int argc, t_atom* argv) { x->name(argc, argv); }, \
      #name, A_GIMME, 0);

#define ADDMETHOD_GIMME_(var, name)                                               \
  class_addmethod(                                                                \
      c,                                                                          \
      (method) +                                                                  \
          [](ocue* x, t_symbol*, int argc, t_atom* argv) { x->var(argc, argv); }, \
      name, A_GIMME, 0);

  auto& c = ocue::max_class;

  ADDMETHOD_GIMME_(create, "new");
  ADDMETHOD_GIMME(update);
  ADDMETHOD_GIMME(recall);
  ADDMETHOD_GIMME_(recall_next, "recall/next");
  ADDMETHOD_GIMME_(recall_previous, "recall/previous");
  ADDMETHOD_GIMME(remove);
  ADDMETHOD_GIMME(move);
  ADDMETHOD_GIMME(output);
  ADDMETHOD_GIMME(read);
  ADDMETHOD_GIMME(write);
  ADDMETHOD_GIMME(rename);
  ADDMETHOD_NOTHING(clear);
  ADDMETHOD_NOTHING(json);
  ADDMETHOD_NOTHING(names);

  ADDMETHOD_GIMME(explore);
  ADDMETHOD_GIMME(select);
  ADDMETHOD_GIMME(select_model);
  ADDMETHOD_GIMME(display_model);

  ADDMETHOD_NOTHING_(selection_dump, "selection/dump");
  ADDMETHOD_GIMME_(selection_add, "selection/add");
  ADDMETHOD_GIMME_(selection_remove, "selection/remove");
  ADDMETHOD_GIMME_(selection_switch, "selection/switch");
  ADDMETHOD_GIMME_(selection_filter_all, "selection/filter_all");
  ADDMETHOD_GIMME_(selection_filter_any, "selection/filter_any");
  ADDMETHOD_GIMME_(selection_grab, "selection/grab");

  CLASS_ATTR_SYM(c, "device", 0, ocue, m_device_name);
  CLASS_ATTR_LABEL(c, "device", 0, "Device to bind to");
  {
    t_object* theattr = (t_object*)class_attr_get(c, gensym("device"));
    object_method(
        theattr, gensym("setmethod"), USESYM(get),
        (method) + [](ocue* x, void* attr, long* ac, t_atom** av) {
          return x->get_device_name(ac, av);
        });
    object_method(
        theattr, gensym("setmethod"), USESYM(set),
        (method) + [](ocue* x, void* attr, long ac, t_atom* av) {
          return x->set_device_name(ac, av);
        });
  }

  search_sort_filter::setup_attribute<ocue>(c);
  class_register(CLASS_BOX, ocue::max_class);
}

void* ossia_cue_create(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<ocue>();
  x->m_device_name = gensym("");
  x->m_otype = object_class::cue;

  attr_args_process(x, (short)argc, argv);

  if(x)
  {
    critical_enter(0);
    ossia_max::get_patcher_descriptor(x->m_patcher).cues.push_back(x);
    // The cue has to register to a device
    x->m_cues = std::make_shared<ossia::cues>();
    x->m_last_filename = "cues.cue.json";
    x->m_dumpout = outlet_new(x, nullptr);
    x->m_data_out = outlet_new(x, nullptr);

    // default attributes
    x->m_name = _sym_nothing;

    critical_exit(0);
  }

  return x;
}

template <typename... T>
void ocue::dump_message(std::string_view msg, T&&... t)
{
  constexpr int N = sizeof...(T);

  if constexpr(N > 0)
  {
    t_atom args[N];
    {
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        (pack_to_atom(args[I], t), ...);
      }
      (std::make_index_sequence<N>{});
    }
    outlet_anything(m_dumpout, gensym(msg.data()), N, args);
  }
  else
  {
      outlet_anything(m_dumpout, gensym(msg.data()), 0, nullptr);
  }
}

void ocue::dump_message(std::string_view msg, const std::vector<std::string_view>& t)
{
  const int N = t.size();
  t_atom* args = (t_atom*)alloca(sizeof(t_atom) * t.size());
  for(int i = 0; i < std::ssize(t); i++)
  {
    atom_setsym(args + i, gensym(t[i].data()));
  }

  outlet_anything(m_dumpout, gensym(msg.data()), N, args);
}

void ocue::create(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](std::string_view name) {
    {
      this->m_cues->create(name);
      if(m_ns.dev)
        this->m_cues->update(m_ns.dev->get_root_node(), m_ns);

      dump_message("new", name);
      names();
      dump_message("current", name);
    }
  });
}

void ocue::names()
{
  std::vector<std::string_view> names;
  for (auto& cue : this->m_cues->m_cues)
      names.push_back(cue.name);
  dump_message("names", std::as_const(names));
}

void ocue::update(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->update(m_ns.dev->get_root_node(), m_ns, args...); })
    {
      if(m_ns.dev)
        this->m_cues->update(m_ns.dev->get_root_node(), m_ns, args...);
      dump_message("update", name_from_args(*m_cues, args...));
    }
  });
}

void ocue::recall(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->recall(m_ns.dev->get_root_node(), m_ns, args...); })
    {
      if(m_ns.dev)
      {
        this->m_cues->recall(m_ns.dev->get_root_node(), m_ns, args...);
        do_explore(gensym("/"));
      }
      dump_message("recall", name_from_args(*m_cues, args...));
    }
  });
}

void ocue::recall_next(int argc, t_atom* argv)
{
  if(this->m_cues->m_cues.empty())
    return;
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(sizeof...(args) == 0)
    {
      int idx = this->m_cues->current_index();
      int next_index = std::clamp(idx + 1, 0, this->m_cues->size() - 1);

      if(m_ns.dev)
      {
        this->m_cues->recall(m_ns.dev->get_root_node(), m_ns, next_index);
        do_explore(gensym("/"));
      }
      dump_message("recall/next");
    }
  });
}

void ocue::recall_previous(int argc, t_atom* argv)
{
  if(this->m_cues->m_cues.empty())
    return;
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(sizeof...(args) == 0)
    {
      int idx = this->m_cues->current_index();
      int next_index = std::clamp(idx - 1, 0, this->m_cues->size() - 1);

      if(m_ns.dev)
      {
        this->m_cues->recall(m_ns.dev->get_root_node(), m_ns, next_index);
        do_explore(gensym("/"));
      }
      dump_message("recall/previous");
    }
  });
}

void ocue::remove(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->remove(args...); })
    {
      dump_message("remove", name_from_args(*m_cues, args...));
      this->m_cues->remove(args...);
      do_explore(gensym("/"));
    }
  });
}

void ocue::rename(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->rename(args...); })
    {
      if constexpr(sizeof...(args) == 2)
      {
        auto [source, rename] = std::make_tuple(args...);
        std::string_view current = name_from_args(*m_cues, source);
        std::string_view next(rename);

        dump_message("rename", current, next);
      }
      else if constexpr(sizeof...(args) == 1)
      {
        std::string_view current = name_from_args(*m_cues);
        std::string_view next(args...);

        dump_message("rename", current, next);
      }

      this->m_cues->rename(args...);
    }
  });
}

void ocue::json()
{
  rapidjson::StringBuffer buffer = cues_to_string(*m_cues);
  dump_message("json", buffer.GetString());
}

void ocue::selection_dump()
{
  for(auto n : m_ns.m_selection)
  {
    if(n->get_parameter())
    {
      dump_message("namespace/dump", n->osc_address().c_str());
    }
  }
}

void ocue::clear()
{
  m_cues->clear();
  dump_message("clear");
}

void ocue::move(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->move(args...); })
    {
      this->m_cues->move(args...);
      dump_message("move", args...);
    }
  });
}

void ocue::read(int argc, t_atom* argv)
{
  const struct
  {
    ocue& self;
    void operator()() const noexcept
    {
      if(auto name = prompt_open_filename("Open cue...", "cues.cue.json"); !name.empty())
      {
        do_read(name);
      }
      self.dump_message("read");
    }
    void operator()(std::string_view url) const noexcept
    {
      do_read(url);
      self.dump_message("read", url);
    }
    void do_read(std::string_view url) const noexcept
            try
    {
      self.m_last_filename = to_absolute_path(self.m_patcher, url);
      post("Trying to read: \n - relative: %s\n - absolute: %s", url.data(), self.m_last_filename.data());
      if (self.m_last_filename.empty())
          return;

      rapidjson::Document doc;

      auto str = ossia::presets::read_file(self.m_last_filename);

      doc.ParseInsitu(str.data());
      if(doc.HasParseError())
      {
        post("ossia.cue: invalid json in file '%s'", url.data());
        return;
      }
      if(!doc.IsObject())
      {
        post("ossia.cue: invalid json in file '%s'", url.data());
        return;
      }
      auto cues = doc.FindMember("cues");
      if(cues == doc.MemberEnd() || !cues->value.IsArray())
      {
        post("ossia.cue: invalid json in file '%s'", url.data());
        return;
      }
      self.m_cues->clear();
      read_cues_from_json(cues->value.GetArray(), self.m_cues->m_cues);

      std::vector<t_atom> va;
      value2atom vm{va};
      for(auto& cue : self.m_cues->m_cues)
        vm(cue.name);

      outlet_anything(self.m_dumpout, gensym("names"), va.size(), va.data());
    } catch(...) { }
  } handler{*this};

  invoke_mem_fun(argc, argv, handler);
}

void ocue::write(int argc, t_atom* argv)
{
  const struct
  {
    ocue& self;
    void operator()() const noexcept
    {
      if(auto name = prompt_save_filename("Save cue...", "cues.cue.json"); !name.empty())
      {
        do_write(name);
      }
      self.dump_message("write");
    }

    void operator()(std::string_view url) const noexcept
    {
      do_write(url);
      self.dump_message("write", url);
    }
    void do_write(std::string_view url) const noexcept
    {
        self.m_last_filename = to_absolute_path(self.m_patcher, url);
        if (self.m_last_filename.empty())
            return;

      rapidjson::StringBuffer buffer = cues_to_string(*self.m_cues);

      try
      {
        ossia::presets::write_file({buffer.GetString(), buffer.GetLength()}, self.m_last_filename);
      }
      catch(const std::exception& e)
      {
        post("ossia.cue: write error: %s", e.what());
      }
    }
  } handler{*this};

  invoke_mem_fun(argc, argv, handler);
}

void ocue::output(int argc, t_atom* argv)
{
  struct do_output
  {
    ocue& self;
    ossia::cues& cues = *self.m_cues;
    void operator()(const ossia::cue& c) const
    {
      std::vector<t_atom> vec;
      for(auto& [k, v] : c.preset)
      {
        vec.clear();
        vec.reserve(6);
        vec.resize(1);
        A_SETSYM(&vec[0], gensym(k.c_str()));
        v.apply(value2atom{vec});

        outlet_list(self.m_data_out, _sym_list, vec.size(), vec.data());
      }
    }

    void operator()(const ossia::cue& c, std::string_view pattern) const
    {
      std::vector<t_atom> vec;
      for(auto& [k, v] : c.preset)
      {
        if(!k.starts_with(pattern))
          continue;
        vec.clear();
        vec.reserve(6);
        vec.resize(1);
        A_SETSYM(&vec[0], gensym(k.c_str()));
        v.apply(value2atom{vec});

        outlet_list(self.m_data_out, _sym_list, vec.size(), vec.data());
      }
    }

    void operator()() const
    {
      if(auto cue = cues.current_cue())
      {
        (*this)(*cue);
        self.dump_message("output");
      }
    }
    void operator()(int index) const
    {
      if(auto c = cues.get_cue(index))
      {
        (*this)(*c);
        self.dump_message("output", index);
      }
    }
    void operator()(std::string_view name) const
    {
      if(auto c = cues.find_cue(name))
      {
        (*this)(*c);
        self.dump_message("output", name);
      }
    }

    void operator()(int index, std::string_view pattern) const
    {
      if(auto c = cues.get_cue(index))
      {
        (*this)(*c, pattern);
        self.dump_message("output", index, pattern);
      }
    }
    void operator()(std::string_view name, std::string_view pattern) const
    {
      if(auto c = cues.find_cue(name))
      {
        (*this)(*c, pattern);
        self.dump_message("output", name, pattern);
      }
    }
  };
  do_output do_out{*this};

  invoke_mem_fun(argc, argv, [=](auto&&... args) {
    if constexpr(requires { do_out(args...); })
      do_out(args...);
  });
}

std::vector<std::shared_ptr<matcher>> get_matchers_for_address(
    object_base& self, ossia::net::device_base* device, t_symbol* m_name)
{
  if(!device)
    return {};

  self.m_paths.clear();
  self.m_matchers.clear();

  if(!m_name || m_name == _sym_nothing)
  {
    self.m_addr_scope = ossia::net::address_scope::relative;
    if(auto obj = self.find_parent_object())
    {
      // Explore at the current hierarchy level of ossia.explorer
      return obj->m_matchers;
    }
    else
    {
      // ossia.explorer is not in a hierarchy: explore from / of the device instead
      return {std::make_shared<matcher>(&device->get_root_node(), nullptr)};
    }
  }
  else
  {
    const std::string_view name = m_name->s_name;

    if(name.find(":/") != std::string_view::npos)
    {
      self.m_addr_scope = ossia::net::address_scope::global;
    }
    else if (name.starts_with("/"))
    {
      self.m_addr_scope = ossia::net::address_scope::absolute;
      if(name == "/")
        return { std::make_shared<matcher>(&device->get_root_node(), nullptr) };
    }
    else
    {
      self.m_addr_scope = ossia::net::address_scope::relative;
    }

    // Exploring from a specified address, e.g. explore /foo.
    // FIXME this->m_name has to beset before this
    auto matchers = self.find_or_create_matchers((ossia::net::generic_device*)device);
    if(matchers.empty())
    {
      self.m_addr_scope = ossia::net::address_scope::absolute;
      return {std::make_shared<matcher>(&device->get_root_node(), nullptr)};
    }
    else
    {
      return matchers;
    }
  }

  return {};
}

void ocue::do_explore(t_symbol* name)
{
  if(!m_ns.dev)
      return;
  search_sort_filter filter_models = *this;
  //filter_models.m_sort = gensym("priority");
  //filter_models.m_depth = 0;
  filter_models.m_format = gensym("jit.cellblock");
  filter_models.m_filter_type[0] = gensym("model");
  filter_models.m_filter_type_size = 1;

  // get namespace of given nodes
  m_name = name;
  auto matchers = get_matchers_for_address(*this, m_ns.dev, name);
  auto model_nodes = filter_models.sort_and_filter(matchers);

  // Pretty print it

  {
    auto& nodes = model_nodes;
    auto outlet = this->m_data_out;
    auto prefix = gensym("models");

    {
      t_atom a;
      A_SETLONG(&a, model_nodes.size());
      outlet_anything(outlet, s_size, 1, &a);
    }
    std::vector<t_atom> va;
    va.reserve(6);

    int k = 0;
    // First clear
    write_message(va, outlet, prefix, s_clear, "all");
    write_message(va, outlet, prefix, s_rows, int(std::ssize(nodes)));

    for(const auto& nn : nodes)
    {
      auto& n = *nn;

      const auto& osc_param = ossia::net::osc_parameter_string(n);

      // Format:
      // set x y <name>
      write_message(va, outlet, prefix, s_set, 0, k, osc_param.c_str());

      // cell x y brgb r g b
      if(this->m_ns.m_selection.contains(nn))
        write_message(va, outlet, prefix, s_cell, 0, k, "brgb", 24, 88, 132);
      else
        write_message(va, outlet, prefix, s_cell, 0, k, "brgb", 0, 0, 0);
      k++;
    }
  }

  if(!model_nodes.empty())
  {
    do_display_model(model_nodes.front()->osc_address());
  }
}

void ocue::explore(int argc, t_atom* argv)
{
  // List all the models in the "models" command
  auto name = _sym_nothing;
  if(argc > 0 && argv->a_type == A_SYM)
  {
    name = argv->a_w.w_sym;
  }
  do_explore(name);
}

void ocue::select(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [](std::string_view name) {

  });
}

void ocue::select_model(int argc, t_atom* argv)
{
  display_model(argc, argv);
  // Adds it / removes it from / to the selection
  invoke_mem_fun(argc, argv, [](std::string_view name) {

  });
}

void ocue::do_display_model(std::string_view name)
{
  if (!m_ns.dev)
      return;

  search_sort_filter filter_child_params = *this;
  //filter_child_params.m_sort = gensym("priority");
  filter_child_params.m_depth = 0;
  filter_child_params.m_filter_terminal = 1;
  filter_child_params.m_format = gensym("jit.cellblock");
  filter_child_params.m_filter_type[0] = gensym("parameter");
  filter_child_params.m_filter_type_size = 1;

  // get namespace of given nodes for the selected model

  // FIXME why get_matchers_for_address does not work *ç*
  m_name = gensym(name.data());
  auto matchers = get_matchers_for_address(*this, get_device(), m_name);
  auto nodes = filter_child_params.sort_and_filter(matchers);
  std::string name_plus_slash;
  name_plus_slash.reserve(name.size() + 2); 
  name_plus_slash.append(name);
  name_plus_slash += "/";

  std::erase_if(nodes, [=](ossia::net::node_base* n) {
    return !n->osc_address().starts_with(name_plus_slash);
  });

  // Pretty print it
  {
    auto outlet = this->m_data_out;
    auto prefix = gensym("parameters");

    {
      t_atom a;
      A_SETLONG(&a, nodes.size());
      outlet_anything(outlet, s_size, 1, &a);
    }
    std::vector<t_atom> va;
    va.reserve(6);

    int k = 0;
    // First clear
    write_message(va, outlet, prefix, s_clear, "all");
    write_message(va, outlet, prefix, s_rows, int(std::ssize(nodes)));

    for(const auto& nn : nodes)
    {
      auto& n = *nn;

      const auto& osc_param = ossia::net::osc_parameter_string(n);
      if(!osc_param.starts_with(name))
        continue;

      // Format:
      // set x y <name>
      write_message(va, outlet, prefix, s_set, 0, k, osc_param.c_str() + name.length());

      // cell x y brgb r g b
      if(this->m_ns.m_selection.contains(nn))
        write_message(va, outlet, prefix, s_cell, 0, k, "brgb", 24, 88, 132);
      else
        write_message(va, outlet, prefix, s_cell, 0, k, "brgb", 0, 0, 0);
      k++;
    }
  }
}
void ocue::display_model(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](std::string_view name) { do_display_model(name); });
}

void ocue::selection_add(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_ns.namespace_select(args...); })
    {
      this->m_ns.namespace_select(args...);
      dump_message("namespace/select", args...);
    }
  });
}

void ocue::selection_switch(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_ns.namespace_switch(args...); })
    {
      this->m_ns.namespace_switch(args...);
      dump_message("namespace/switch", args...);
    }
  });
}

void ocue::selection_filter_all(int argc, t_atom* argv)
{
  this->m_ns.namespace_filter_all(parse_selection_filter(argc, argv));
}

void ocue::selection_filter_any(int argc, t_atom* argv)
{
  this->m_ns.namespace_filter_any(parse_selection_filter(argc, argv));
}

void ocue::selection_remove(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_ns.namespace_deselect(args...); })
    {
      this->m_ns.namespace_deselect(args...);
      dump_message("namespace/deselect", args...);
    }
  });
}

void ocue::selection_grab(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_ns.namespace_grab(args...); })
    {
      this->m_ns.namespace_grab(args...);
      dump_message("namespace/grab", args...);
    }
  });
}

ossia::net::device_base* find_parent_device(const patcher_descriptor& desc)
{
    if(desc.device)
    {
      return desc.device->m_device.get();
    }
    else if(desc.client)
    {
      return desc.client->m_device.get();
    }
    else
    {
        if(auto parent = desc.parent_patcher)
        {
            auto& d = ossia_max::get_patcher_descriptor(parent);
            return find_parent_device(d);
        }
        else
        {
            // Bind to default device?
            const auto& default_dev = ossia_max::instance().get_default_device();
            return default_dev.get();

        }
    }
}
ossia::net::device_base* ocue::get_device() const noexcept
{
  // If we set an explicit device name try to use it
  if(m_device_name != nullptr)
  {
    std::vector<ossia::net::generic_device*> devs = get_all_devices();
    for(auto dev : devs)
    {
      if(dev->get_name() == m_device_name->s_name)
      {
        return dev;
      }
    }
  }

  // Fall back on the default case
  auto& desc = ossia_max::get_patcher_descriptor(m_patcher);
  return find_parent_device(desc);
}

t_max_err ocue::get_device_name(long* ac, t_atom** av)
{
  if((*ac) && (*av))
  {
    //memory passed in, use it
  }
  else
  {
    //otherwise allocate memory
    *ac = 1;
    if(!(*av = (t_atom*)getbytes(sizeof(t_atom) * (*ac))))
    {
      *ac = 0;
      return MAX_ERR_OUT_OF_MEM;
    }
  }
  atom_setsym(*av, m_device_name);
  return MAX_ERR_NONE;
}

t_max_err ocue::set_device_name(long ac, t_atom* av)
{
  if(ac && av && av[0].a_type == A_SYM)
  {
    auto sym = atom_getsym(av);
    m_device_name = sym;
  }
  else
  {
    // no args, set to zero
    m_device_name = _sym_nothing;
  }

  do_registration();
  return MAX_ERR_NONE;
}
void ocue::do_registration()
{
  m_ns.set_device(get_device());
}

void ocue::unregister()
{
  m_ns.set_device(nullptr);
}

void ocue::update_selection() { }

void ocue::free(ocue* x)
{
  if(x)
  {
    critical_enter(0);

    ossia_max::get_patcher_descriptor(x->m_patcher).cues.remove_all(x);

    outlet_delete(x->m_data_out);
    outlet_delete(x->m_dumpout);
    x->~ocue();
    critical_exit(0);
  }
}

t_max_err ocue::notify(ocue* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  return 0;
}

void ocue::assist(ocue* x, void* b, long m, long a, char* s) { }
