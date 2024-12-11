#include <ossia/detail/json.hpp>

#include <ossia-max/src/ocue.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <boost/algorithm/string.hpp>

#include <rapidjson/prettywriter.h>

namespace
{
    bool is_absolute_path(std::string_view v)
    {
        if (v.starts_with('/') || v.starts_with('~'))
            return true;
        if (v.find(':') != std::string_view::npos)
            return true;
        return false;
    }
    static std::string fix_url(std::string_view u)
    {
        if(u.starts_with("~"))
        {
            const char* home = getenv("HOME");
            if (!home) home = "";
            std::string res = home;
            res += u.substr(1);
            return res;
        }
        return std::string(u);
    }
    static std::string to_absolute_path(t_patcher* patcher, std::string_view url)
    {
        std::string full_path;
        if (is_absolute_path(url))
        {
            full_path = fix_url(url);
        }
        else
        {
            std::string_view patcher_path = jpatcher_get_filepath(patcher)->s_name;
            std::string_view patcher_name = jpatcher_get_filename(patcher)->s_name;
            auto end_it = patcher_path.rfind(patcher_name);
            if (end_it <= 0 || end_it > std::string_view::npos)
                return {};

            int start = 0;
            if (patcher_path.starts_with("Macintosh HD:"))
                start = strlen("Macintosh HD:");
            full_path = patcher_path.substr(start, end_it - start);
            full_path += url;
        }
        return full_path;
    }
    static std::string
        prompt_open_filename(std::string_view dialogtitle, std::string_view default_filename)
    {
        char buffer[MAX_PATH_CHARS] = {};
        strncpy(buffer, default_filename.data(), default_filename.size());

        open_promptset(dialogtitle.data());

        short path{};
        t_fourcc outtype{};
        t_fourcc filetype = 'TEXT';
        if(open_dialog(buffer, &path, &outtype, &filetype, 1) == 0)
        {
            t_filehandle hdl{};
            path_opensysfile(buffer, path, &hdl, e_max_openfile_permissions::PATH_READ_PERM);

            char full_path[MAX_PATH_CHARS] = {};
            path_topathname(path, buffer, full_path);

            char native_path[MAX_PATH_CHARS];
            path_nameconform(full_path, native_path, PATH_STYLE_NATIVE, PATH_TYPE_BOOT);

            sysfile_close(hdl);
            return native_path;
        }
        return {};
    }
static std::string
prompt_save_filename(std::string_view dialogtitle, std::string_view default_filename)
{
  char buffer[MAX_PATH_CHARS] = {};
  strncpy(buffer, default_filename.data(), default_filename.size());

  saveas_promptset(dialogtitle.data());

  short path{};
  t_fourcc outtype{};
  t_fourcc filetype = 'TEXT';
  if(saveasdialog_extended(buffer, &path, &outtype, &filetype, 1) == 0)
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
        if(symb == "@valuetype")
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
            filt.bounding.push_back(ossia::bounding_mode::CLAMP_LOW);
          if(symb == "high")
            filt.bounding.push_back(ossia::bounding_mode::CLAMP_HIGH);
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

static void pack_to_atom(t_atom& t, long long& i)
{
  atom_setlong(&t, i);
}
static void pack_to_atom(t_atom& t, long& i)
{
  atom_setlong(&t, i);
}
static void pack_to_atom(t_atom& t, int& i)
{
  atom_setlong(&t, i);
}
static void pack_to_atom(t_atom& t, float& i)
{
  atom_setfloat(&t, i);
}
static void pack_to_atom(t_atom& t, char*& i)
{
  atom_setsym(&t, gensym(i));
}
static void pack_to_atom(t_atom& t, std::string_view i)
{
  if(!i.empty())
    atom_setsym(&t, gensym(i.data()));
  else
    atom_setsym(&t, gensym("<invalid>"));
}

static std::string_view
name_from_args(const ossia::cues& cue, std::string_view args) noexcept
{
  return args;
}

static std::string_view name_from_args(const ossia::cues& cue, int idx) noexcept
{
  if(idx >= 0 && idx < cue.m_cues.size())
    return cue.m_cues[idx].name;
  else
    return {};
}

static std::string_view name_from_args(const ossia::cues& cue) noexcept
{
  return name_from_args(cue, cue.current_index());
}

}
