#include <ossia/detail/json.hpp>

#include <ossia-max/src/ocue.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <boost/algorithm/string.hpp>

#include <rapidjson/prettywriter.h>

namespace
{
static
std::string prompt_filename(std::string_view dialogtitle, std::string_view default_filename)
{
    char buffer[MAX_PATH_CHARS] = {};
    strncpy(buffer, default_filename.data(), default_filename.size());

    saveas_promptset(dialogtitle.data());

    short path{};
    t_fourcc outtype{};
    t_fourcc filetype = 'TEXT';
    if (saveasdialog_extended(buffer, &path, &outtype, &filetype, 1) == 0)
    {
        t_filehandle hdl{};
        path_createsysfile(buffer, path, filetype, &hdl);

        char full_path[MAX_PATH_CHARS] = {};
        path_topathname(path, buffer, full_path);

        char native_path[MAX_PATH_CHARS];
        path_nameconform(full_path, native_path, PATH_STYLE_NATIVE, PATH_TYPE_BOOT);

        sysfile_close(hdl);
        return native_path;
    }
    return {};
}

ossia::selection_filters parse_selection_filter(int argc, t_atom* argv)
{
  ossia::selection_filters filt;

  enum
  {
    unknown,
    processing_names,
    processing_val_type,
    processing_access,
    processing_bounding,
    processing_tags,
    processing_visibility
  } state
      = processing_names;

  for(int i = 0; i < argc; i++)
  {
    if(argv[i].a_type == A_SYM)
    {
      std::string symb
          = boost::algorithm::to_lower_copy(std::string(argv[i].a_w.w_sym->s_name));
      if(symb.starts_with("@"))
      {
        if(symb == "@type")
        {
          state = processing_val_type;
          continue;
        }
        if(symb == "@access")
        {
          state = processing_access;
          continue;
        }
        if(symb == "@bounding")
        {
          state = processing_bounding;
          continue;
        }
        if(symb == "@tags")
        {
          state = processing_tags;
          continue;
        }
        if(symb == "@visibility")
        {
          state = processing_visibility;
          continue;
        }
      }

      switch(state)
      {
        case processing_names:
          filt.selection.push_back(std::string(symb));
          break;
        case processing_val_type: {
          if(auto param = ossia::default_parameter_for_type(symb))
          {
            filt.type.push_back(ossia::underlying_type(param->type));
          }
          break;
        }
        case processing_access: {
          if(symb == "get")
            filt.access.push_back(ossia::access_mode::GET);
          if(symb == "set")
            filt.access.push_back(ossia::access_mode::SET);
          if(symb == "bi")
            filt.access.push_back(ossia::access_mode::BI);
          break;
        }
        case processing_bounding: {
          if(symb == "free")
            filt.bounding.push_back(ossia::bounding_mode::FREE);
          if(symb == "clip")
            filt.bounding.push_back(ossia::bounding_mode::CLIP);
          if(symb == "low")
            filt.bounding.push_back(ossia::bounding_mode::LOW);
          if(symb == "high")
            filt.bounding.push_back(ossia::bounding_mode::HIGH);
          if(symb == "wrap")
            filt.bounding.push_back(ossia::bounding_mode::WRAP);
          if(symb == "fold")
            filt.bounding.push_back(ossia::bounding_mode::FOLD);
          break;
        }
        case processing_tags: {
          filt.tags.push_back(std::string(symb));
          break;
        }
        case processing_visibility:
          filt.visibility
              = symb == "visible" || symb == "true" || symb == "yes" || symb == "ok"
                    ? ossia::selection_filters::visible
                    : ossia::selection_filters::invisible;
          break;
        case unknown:
          break;
      }
    }
  }

  return filt;
}
using writer_t = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
struct write_json_preset_value
{
  writer_t& writer;
  void operator()(ossia::impulse) const
  {
    writer.Key("type");
    writer.String("impulse");
  }
  void operator()(int v) const
  {
    writer.Key("type");
    writer.String("int");
    writer.Key("value");
    writer.Int(v);
  }
  void operator()(float v) const
  {
    writer.Key("type");
    writer.Key("float");
    writer.Key("value");
    writer.Double(v);
  }
  void operator()(bool v) const
  {
    writer.Key("type");
    writer.String("bool");
    writer.Key("value");
    writer.Bool(v);
  }

  void operator()(const std::string& v) const
  {
    writer.Key("type");
    writer.String("string");
    writer.Key("value");
    writer.String(v);
  }

  template <std::size_t N>
  void operator()(const std::array<float, N>& vec) const
  {
    writer.Key("type");
    writer.String("vec" + std::to_string(N) + "f");
    writer.Key("value");
    writer.StartArray();
    for(std::size_t i = 0; i < N; i++)
    {
      writer.Double(vec[i]);
    }
    writer.EndArray();
  }

  void operator()(const std::vector<ossia::value>& vec) const
  {
    writer.Key("type");
    writer.String("list");
    writer.Key("value");
    writer.StartArray();
    for(auto& val : vec)
    {
      writer.StartObject();
      val.apply(*this);
      writer.EndObject();
    }
    writer.EndArray();
  }
  void operator()(const ossia::value_map_type& vec) const { }
  void operator()() const { throw std::runtime_error("value_to_json_value: no type"); }
};

static void cue_to_json(writer_t& doc, const ossia::cue& cu)
{
  doc.StartObject();
  doc.Key("name");
  doc.String(cu.name);

  doc.Key("preset");
  doc.StartArray();
  for(auto& [k, v] : cu.preset)
  {
    doc.StartObject();
    doc.Key("address");
    doc.String(k);

    v.apply(write_json_preset_value{doc});
    doc.EndObject();
  }
  doc.EndArray();
  doc.EndObject();
}

static rapidjson::StringBuffer cues_to_string(const ossia::cues& c)
{

  rapidjson::StringBuffer buffer;
  writer_t doc{buffer};

  doc.StartObject();
  doc.Key("cues");
  doc.StartArray();
  for(auto& cue : c.m_cues)
  {
    cue_to_json(doc, cue);
  }
  doc.EndArray();
  doc.EndObject();

  return buffer;
}

// Format of a cue:
// {
//   "name": "foo",
//   "preset": [
//     { "address": "/foo/bar", "value": 123, type: "int" },
//     { "address": "/baz", "value": 123, type: "int" },
//   ]
// }

// Format of a cuelist:
// { "cues": [ ... cue ... ] }

static ossia::value
read_typed_ossia_value_from_json(const rapidjson::Document::ConstObject& value);
static ossia::value read_typed_ossia_value_from_json(
    const rapidjson::Document::ConstObject& value, std::string_view type)
{
  if(type == "impulse")
  {
    return ossia::impulse{};
  }
  auto it = value.FindMember("value");
  if(it == value.MemberEnd())
    return {};

  if(type == "int")
  {
    if(it->value.IsInt())
    {
      return it->value.GetInt();
    }
  }
  else if(type == "float")
  {
    if(it->value.IsFloat())
    {
      return it->value.GetFloat();
    }
  }
  else if(type == "string")
  {
    if(it->value.IsString())
    {
      return std::string(it->value.GetString(), it->value.GetStringLength());
    }
  }
  else if(type == "bool")
  {
    if(it->value.IsBool())
    {
      return it->value.GetBool();
    }
  }
  else if(type == "vec4f")
  {
    if(it->value.IsArray())
    {
      auto arr = it->value.GetArray();
      if(arr.Size() == 4 && arr[0].IsDouble() && arr[1].IsDouble() && arr[2].IsDouble()
         && arr[3].IsDouble())
        return ossia::make_vec(
            arr[0].GetDouble(), arr[1].GetDouble(), arr[2].GetDouble(),
            arr[3].GetDouble());
    }
  }
  else if(type == "vec3f")
  {
    if(it->value.IsArray())
    {
      auto arr = it->value.GetArray();
      if(arr.Size() == 3 && arr[0].IsDouble() && arr[1].IsDouble() && arr[2].IsDouble())
        return ossia::make_vec(
            arr[0].GetDouble(), arr[1].GetDouble(), arr[2].GetDouble());
    }
  }
  else if(type == "vec2f")
  {
    if(it->value.IsArray())
    {
      auto arr = it->value.GetArray();
      if(arr.Size() == 2 && arr[0].IsDouble() && arr[1].IsDouble())
        return ossia::make_vec(arr[0].GetDouble(), arr[1].GetDouble());
    }
  }
  else if(type == "list")
  {
    if(it->value.IsArray())
    {
      std::vector<ossia::value> vec;
      auto arr = it->value.GetArray();
      vec.reserve(arr.Size());
      for(const rapidjson::Value& sub_val : arr)
      {
        if(sub_val.IsObject())
          vec.push_back(read_typed_ossia_value_from_json(sub_val.GetObject()));
      }
      return vec;
    }
  }
  return {};
}

static ossia::value
read_typed_ossia_value_from_json(const rapidjson::Document::ConstObject& value)
{
  if(auto it = value.FindMember("type"); it != value.MemberEnd() && it->value.IsString())
  {
    std::string_view type{it->value.GetString(), it->value.GetStringLength()};
    return read_typed_ossia_value_from_json(value, type);
  }
  return {};
}

static void read_cue_preset_from_json(
    const rapidjson::Document::ConstArray& json, ossia::presets::preset& p)
{
  for(const rapidjson::Value& value : json)
  {
    if(!value.IsObject())
      continue;
    std::string addr;

    if(auto it = value.FindMember("address");
       it != value.MemberEnd() && it->value.IsString())
    {
      addr = std::string{it->value.GetString(), it->value.GetStringLength()};

      if(addr.empty())
        continue;
    }

    ossia::value v = read_typed_ossia_value_from_json(value.GetObject());
    if(v.valid())
      p.emplace_back(std::move(addr), std::move(v));
  }
}

static void read_cues_from_json(
    const rapidjson::Document::Array& json, std::vector<ossia::cue>& cues)
{
  for(const rapidjson::Value& value : json)
  {
    if(!value.IsObject())
      continue;
    ossia::cue c;
    if(auto it = value.FindMember("name");
       it != value.MemberEnd() && it->value.IsString())
    {
      c.name = std::string{it->value.GetString(), it->value.GetStringLength()};
    }

    if(auto it = value.FindMember("preset");
       it != value.MemberEnd() && it->value.IsArray())
    {
      read_cue_preset_from_json(it->value.GetArray(), c.preset);
    }
    cues.push_back(std::move(c));
  }
}
bool is_absolute_path(std::string_view v)
{
  if(v.starts_with('/'))
    return true;
  if(v.find(':') != std::string_view::npos)
    return true;
  return false;
}

static void invoke_mem_fun(int argc, t_atom* argv, auto f)
{
#define if_possible(F)          \
  if constexpr(requires { F; }) \
  {                             \
    F;                          \
  }
  if(argc == 0)
  {
    if_possible(f());
  }
  else if(argc == 1 && argv[0].a_type == A_SYM)
  {
    if_possible(f(argv[0].a_w.w_sym->s_name));
  }
  else if(argc == 1 && argv[0].a_type == A_LONG)
  {
    if_possible(f(argv[0].a_w.w_long));
  }
  else if(argc == 1 && argv[0].a_type == A_FLOAT)
  {
    if_possible(f((int)argv[0].a_w.w_float));
  }
  else if(argc == 2 && argv[0].a_type == A_LONG && argv[1].a_type == A_LONG)
  {
    if_possible(f(argv[0].a_w.w_long, argv[1].a_w.w_long));
  }
  else if(argc == 2 && argv[0].a_type == A_LONG && argv[1].a_type == A_FLOAT)
  {
    if_possible(f(argv[0].a_w.w_long, (int)argv[1].a_w.w_float));
  }
  else if(argc == 2 && argv[0].a_type == A_FLOAT && argv[1].a_type == A_LONG)
  {
    if_possible(f((int)argv[0].a_w.w_float, argv[1].a_w.w_long));
  }
  else if(argc == 2 && argv[0].a_type == A_FLOAT && argv[1].a_type == A_FLOAT)
  {
    if_possible(f((int)argv[0].a_w.w_float, (int)argv[1].a_w.w_float));
  }
  else if(argc == 2 && argv[0].a_type == A_LONG && argv[1].a_type == A_SYM)
  {
    if_possible(f(argv[0].a_w.w_long, argv[1].a_w.w_sym->s_name));
  }
  else if(argc == 2 && argv[0].a_type == A_SYM && argv[1].a_type == A_LONG)
  {
    if_possible(f(argv[0].a_w.w_sym->s_name, argv[1].a_w.w_long));
  }
  else if(argc == 2 && argv[0].a_type == A_SYM && argv[1].a_type == A_FLOAT)
  {
    if_possible(f(argv[0].a_w.w_sym->s_name, (int)argv[1].a_w.w_float));
  }
  else if(argc == 2 && argv[0].a_type == A_SYM && argv[1].a_type == A_SYM)
  {
    if_possible(f(argv[0].a_w.w_sym->s_name, argv[1].a_w.w_sym->s_name));
  }
#undef if_possible
}

static void pack_to_atom(t_atom& t, long long& i) {
  atom_setlong(&t, i);
}
static void pack_to_atom(t_atom& t, long& i) {
  atom_setlong(&t, i);
}
static void pack_to_atom(t_atom& t, int& i) {
  atom_setlong(&t, i);
}
static void pack_to_atom(t_atom& t, float& i) {
  atom_setfloat(&t, i);
}
static void pack_to_atom(t_atom& t, char*& i) {
  atom_setsym(&t, gensym(i));
}
static void pack_to_atom(t_atom& t, std::string_view i) {
  if(!i.empty())
    atom_setsym(&t, gensym(i.data()));
  else
    atom_setsym(&t, gensym("<invalid>"));
}

static
std::string_view name_from_args(const ossia::cues& cue, std::string_view args) noexcept {
    return args;
}

static
std::string_view name_from_args(const ossia::cues& cue, int idx) noexcept {
    if(idx >= 0 && idx < cue.m_cues.size())
      return cue.m_cues[idx].name;
    else
      return {};
}

static
std::string_view name_from_args(const ossia::cues& cue) noexcept {
    return name_from_args(cue, cue.current_index());
}


}

using namespace ossia::max_binding;
void* ossia_cue_create(t_symbol* s, long argc, t_atom* argv);
extern "C" OSSIA_MAX_EXPORT void ossia_cue_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.cue class
  ossia_library.ossia_cue_class = class_new(
      "ossia.cue", (method)ossia_cue_create, (method)ocue::free, (long)sizeof(ocue), 0L,
      A_GIMME, 0);

#define ADDMETHOD_NOTHING(name)                                                    \
  class_addmethod(                                                                 \
      c,                                                                           \
      (method) +                                                                   \
          [](ocue* x, t_symbol*) { x->name(); },                                   \
      #name, A_NOTHING, 0);
#define ADDMETHOD_NOTHING_(var, name)                                              \
  class_addmethod(                                                                 \
      c,                                                                           \
      (method) +                                                                   \
          [](ocue* x, t_symbol*) { x->var(); },                                    \
      name, A_NOTHING, 0);

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

  auto& c = ossia_library.ossia_cue_class;

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

  ADDMETHOD_NOTHING_(namespace_dump, "namespace/dump");
  ADDMETHOD_GIMME_(namespace_select, "namespace/select");
  ADDMETHOD_GIMME_(namespace_deselect, "namespace/deselect");
  ADDMETHOD_GIMME_(namespace_filter_all, "namespace/filter_all");
  ADDMETHOD_GIMME_(namespace_filter_any, "namespace/filter_any");
  ADDMETHOD_GIMME_(namespace_grab, "namespace/grab");

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

  class_register(CLASS_BOX, ossia_library.ossia_cue_class);
}

void* ossia_cue_create(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<ocue>();
  x->m_device_name = gensym("");
  x->m_otype = object_class::cue;

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


template<typename... T>
void ocue::dump_message(std::string_view msg, T&&... t)
{
    constexpr int N = sizeof...(T);
    t_atom args[N];
    {
        [&] <std::size_t... I> (std::index_sequence<I...>) {
          (pack_to_atom(args[I], t), ...);
        }(std::make_index_sequence<N>{});
    }
    outlet_anything(m_dumpout, gensym(msg.data()), N, args);
}

void ocue::dump_message(std::string_view msg, const std::vector<std::string_view>& t)
{
    const int N = t.size();
    t_atom* args = (t_atom*)alloca(sizeof(t_atom) * t.size());
    for(int i = 0; i < std::ssize(t); i++) {
        atom_setsym(args + i, gensym(t[i].data()));
    }

    outlet_anything(m_dumpout, gensym(msg.data()), N, args);
}

void ocue::create(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](std::string_view name) {
    if constexpr(requires { this->m_cues->create(name); })
    {
      this->m_cues->create(name);
      this->m_cues->update();

      dump_message("new", name);
      {
          std::vector<std::string_view> names;
          for(auto& cue : this->m_cues->m_cues)
              names.push_back(cue.name);
          dump_message("names", std::as_const(names));
      }
      dump_message("current", name);
    }
  });
}

void ocue::update(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->update(args...); })
    {
      this->m_cues->update(args...);
      dump_message("update", name_from_args(*m_cues, args...));
    }
  });
}

void ocue::recall(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->recall(args...); })
    {
      this->m_cues->recall(args...);
      dump_message("recall", name_from_args(*m_cues, args...));
    }
  });
}

void ocue::recall_next(int argc, t_atom* argv)
{
  if(this->m_cues->m_cues.empty())
    return;
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(sizeof...(args) == 0) {
      int idx = this->m_cues->current_index();
      int next_index = std::clamp(idx + 1, 0, this->m_cues->size() - 1);
      this->m_cues->recall(next_index);
      dump_message("recall/next");
    }
  });
}

void ocue::recall_previous(int argc, t_atom* argv)
{
    if(this->m_cues->m_cues.empty())
      return;
    invoke_mem_fun(argc, argv, [this](auto&&... args) {
      if constexpr(sizeof...(args) == 0) {
        int idx = this->m_cues->current_index();
        int next_index = std::clamp(idx - 1, 0, this->m_cues->size() - 1);
        this->m_cues->recall(next_index);
        dump_message("recall/previous");
      }
    });
}

void ocue::remove(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->remove(args...); }) {
      dump_message("remove", name_from_args(*m_cues, args...));
      this->m_cues->remove(args...);
    }
  });
}

void ocue::rename(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->rename(args...); }) {
      if constexpr(sizeof...(args) == 2) {
        auto [source, rename] = std::make_tuple(args...);
        std::string_view current = name_from_args(*m_cues, source);
        std::string_view next(rename);

        dump_message("rename", current, next);
      }
      else if constexpr(sizeof...(args) == 1) {
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

void ocue::namespace_dump()
{
  for(auto n : m_cues->m_selection)
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
    const struct {
        ocue& self;
        void operator()() const noexcept
        {
            if(auto name = prompt_filename("Open cue...", "cues.cue.json"); !name.empty())
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
        {
            self.m_last_filename = std::string(url);

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
        }
    } handler{*this};

    invoke_mem_fun(argc, argv, handler);
}

void ocue::write(int argc, t_atom* argv)
{
    const struct {
        ocue& self;
        void operator()() const noexcept
        {
            if(auto name = prompt_filename("Save cue...", "cues.cue.json"); !name.empty())
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
            rapidjson::StringBuffer buffer = cues_to_string(*self.m_cues);

            std::string full_path;
            if(is_absolute_path(url))
            {
              full_path = url;
            }
            else
            {
              std::string_view patcher_path = jpatcher_get_filepath(self.m_patcher)->s_name;
              std::string_view patcher_name = jpatcher_get_filename(self.m_patcher)->s_name;
              auto end_it = patcher_path.rfind(patcher_name);
              if(end_it <= 0 || end_it > std::string_view::npos)
                return;

              int start = 0;
              if(patcher_path.starts_with("Macintosh HD:"))
                start = strlen("Macintosh HD:");
              full_path = patcher_path.substr(start, end_it - start);
              full_path += url;
            }

            self.m_last_filename = full_path;

            try
            {
              ossia::presets::write_file({buffer.GetString(), buffer.GetLength()}, full_path);
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

    void operator()() const {
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

void ocue::namespace_select(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->namespace_select(args...); })
    {
      this->m_cues->namespace_select(args...);
      dump_message("namespace/select", args...);
    }
  });
}

void ocue::namespace_filter_all(int argc, t_atom* argv)
{
  this->m_cues->namespace_filter_all(parse_selection_filter(argc, argv));
}

void ocue::namespace_filter_any(int argc, t_atom* argv)
{
  this->m_cues->namespace_filter_any(parse_selection_filter(argc, argv));
}

void ocue::namespace_deselect(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->namespace_deselect(args...); })
    {
      this->m_cues->namespace_deselect(args...);
      dump_message("namespace/deselect", args...);
    }
  });
}

void ocue::namespace_grab(int argc, t_atom* argv)
{
  invoke_mem_fun(argc, argv, [this](auto&&... args) {
    if constexpr(requires { this->m_cues->namespace_grab(args...); })
    {
      this->m_cues->namespace_grab(args...);
      dump_message("namespace/grab", args...);
    }
  });
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
    // Bind to default device?
    const auto& default_dev = ossia_max::instance().get_default_device();
    return default_dev.get();
  }
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
  m_cues->set_device(get_device());
}

void ocue::unregister()
{
  m_cues->set_device(nullptr);
}

void ocue::free(ocue* x)
{
  if(x)
  {
    critical_enter(0);
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
