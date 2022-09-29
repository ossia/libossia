// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/json.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/network/value/detail/value_parse_impl.hpp>
#include <ossia/preset/exception.hpp>
#include <ossia/preset/preset.hpp>

#include <boost/algorithm/string.hpp>

#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <ossia-config.hpp>
#if defined(OSSIA_C)
#include <ossia-c/log/ossia_log.h>
#include <ossia-c/ossia/ossia_utils.hpp>
#include <ossia-c/preset/preset.h>
#include <ossia-c/preset/result.h>
#endif

#if defined(GetObject)
#undef GetObject
#endif

struct ossia_preset
{
  ossia::presets::preset impl;
  ossia_preset()
      : impl(ossia::presets::preset())
  {
  }
  ossia_preset(const ossia::presets::preset& prst)
      : impl(prst)
  {
  }

  auto find(const std::string key)
  {
    for(auto it = impl.begin(); it < impl.end(); it++)
    {
      if(it->first == key)
        return it;
    }
    return impl.end();
  }
};

/// C functions ///
#if defined(OSSIA_C)
extern "C" {
static ossia_preset_result lippincott();

ossia_preset_result ossia_presets_read_json(const char* str, ossia_preset_t* presetptr)
{
  if(presetptr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }

  *presetptr = nullptr;
  if(str != nullptr)
  {
    try
    {
      *presetptr = new ossia_preset(ossia::presets::read_json(std::string(str), false));
      return OSSIA_PRESETS_OK;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_EMPTY_JSON;
}

ossia_preset_result ossia_presets_free(ossia_preset_t preset)
{
  if(preset != nullptr)
  {
    try
    {
      delete preset;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_OK;
}

ossia_preset_result ossia_presets_write_json(
    const ossia_preset_t preset, const char* device, const char** buffer)
{
  if(preset)
  {
    try
    {
      *buffer = copy_string(ossia::presets::write_json(device, preset->impl));
      return OSSIA_PRESETS_OK;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_size(const ossia_preset_t preset, int* size)
{
  if(preset)
  {
    try
    {
      *size = preset->impl.size();
      return OSSIA_PRESETS_OK;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result
ossia_presets_to_string(const ossia_preset_t preset, const char** buffer)
{
  if(preset)
  {
    try
    {
      *buffer = copy_string(ossia::presets::to_string(preset->impl));
      return OSSIA_PRESETS_OK;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result
ossia_devices_write_json(const ossia_device_t odev, const char** buffer)
{
  if(odev != nullptr)
  {
    try
    {
      assert(odev->device);
      *buffer = copy_string(ossia::presets::write_json(*(odev->device)));
      return OSSIA_PRESETS_OK;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result
ossia_devices_apply_preset(ossia_device_t odevptr, ossia_preset_t preset, bool keep_arch)
{
  if(!odevptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if(!preset)
  {
    return OSSIA_PRESETS_NULL_PRESET;
  }
  try
  {
    ossia::presets::keep_arch_type keep_arch_token;
    if(keep_arch)
      keep_arch_token = ossia::presets::keep_arch_on;
    else
      keep_arch_token = ossia::presets::keep_arch_off;

    if(!odevptr->device)
    {
      return OSSIA_PRESETS_NULL_DEVICE;
    }

    ossia::presets::apply_preset(
        odevptr->device->get_root_node(), preset->impl, keep_arch_token /*, {}, false*/);
  }
  catch(...)
  {
    return lippincott();
  }
  return OSSIA_PRESETS_OK;
}

ossia_preset_result
ossia_devices_make_preset(ossia_device_t odev, ossia_preset_t* presetptr)
{
  if(presetptr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }

  *presetptr = nullptr;
  if(odev != nullptr)
  {
    try
    {
      assert(odev->device);
      *presetptr
          = new ossia_preset(ossia::presets::make_preset(odev->device->get_root_node()));
      return OSSIA_PRESETS_OK;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_to_string(ossia_device_t odev, const char** buffer)
{
  if(odev != nullptr)
  {
    try
    {
      assert(odev->device);
      *buffer = copy_string(ossia::presets::to_string(*(odev->device)));
      return OSSIA_PRESETS_OK;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result
ossia_devices_get_node(ossia_device_t odev, const char* addr, ossia_node_t* nodeptr)
{
  if(nodeptr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if(addr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_ADDRESS;
  }
  if(odev != nullptr)
  {
    try
    {
      assert(odev->device);
      auto gotnode = ossia::presets::get_node(odev->device->get_root_node(), addr);
      if(gotnode == nullptr)
      {
        return OSSIA_PRESETS_INVALID_ADDRESS;
      }
      *nodeptr = convert(gotnode);
      return OSSIA_PRESETS_OK;
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result
ossia_devices_get_child(ossia_node_t root, const char* childname, ossia_node_t* nodeptr)
{
  if(nodeptr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if(childname == nullptr)
  {
    return OSSIA_PRESETS_INVALID_ADDRESS;
  }
  if(root == nullptr)
  {
    return OSSIA_PRESETS_NULL_NODE;
  }
  try
  {
    auto gotnode = ossia::presets::get_node(*convert_node(root), childname);
    if(gotnode == nullptr)
    {
      return OSSIA_PRESETS_INVALID_ADDRESS;
    }
    *nodeptr = convert(gotnode);
  }
  catch(...)
  {
    return lippincott();
  }
  return OSSIA_PRESETS_OK;
}

bool ossia_presets_has_key(const ossia_preset_t preset, const char* key)
{
  if(preset && key)
  {
    try
    {
      return preset->find(key) != preset->impl.end();
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return false;
}

ossia_preset_result ossia_presets_key_to_string(
    const ossia_preset_t preset, const char* key, const char** buffer)
{
  if(preset && key)
  {
    try
    {
      auto it = preset->find(key);
      if(it != preset->impl.end())
      {
        *buffer = copy_string(ossia::convert<std::string>(it->second));
        return OSSIA_PRESETS_OK;
      }
      else
      {
        return OSSIA_PRESETS_KEY_NOT_FOUND;
      }
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_key_to_value(
    const ossia_preset_t preset, const char* key, ossia_value_t* buffer)
{
  if(preset && key)
  {
    try
    {
      auto it = preset->find(key);
      if(it != preset->impl.end())
      {
        *buffer = convert(it->second);
        return OSSIA_PRESETS_OK;
      }
      else
      {
        return OSSIA_PRESETS_KEY_NOT_FOUND;
      }
    }
    catch(...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_free_string(const char* strptr)
{
  delete[] strptr;
  return OSSIA_PRESETS_OK;
}

} // extern "C"
#endif
/// C++ Implementations: Presets ///

static void sort_by_priority(std::vector<ossia::net::node_base*>& children)
{
  ossia::sort(children, [](auto n1, auto n2) {
    return ossia::net::get_priority(*n1) > ossia::net::get_priority(*n2);
  });
}

static bool must_save_parameter(
    const ossia::net::parameter_base& param, ossia::presets::preset_save_options opt)
{
  if(param.get_value_type() == ossia::val_type::IMPULSE)
    return false;

  const ossia::access_mode acc = param.get_access();
  switch(acc)
  {
    case ossia::access_mode::BI:
      return opt.save_bi;
    case ossia::access_mode::GET:
      return opt.save_get;
    case ossia::access_mode::SET:
      return opt.save_set;
  }
  return false;
}

static ossia::value json_to_ossia_value(const rapidjson::Value& value)
{

  ossia::value val;
  enum rapidjson::Type tval = value.GetType();

  switch(tval)
  {
    case rapidjson::kFalseType:
      val = bool(false);
      break;
    case rapidjson::kTrueType:
      val = bool(true);
      break;
    case rapidjson::kNumberType:
      if(value.IsInt())
      {
        val = int32_t(value.GetInt());
      }
      else
      {
        val = float(value.GetDouble());
      }
      break;
    case rapidjson::kStringType:
      val = std::string(value.GetString());
      break;
    case rapidjson::kArrayType: {
      const auto N = value.Size();
      auto handle_vec = [&](const auto& value) {
        std::vector<ossia::value> list;
        list.reserve(N);
        for(unsigned int i = 0; i < N; i++)
        {
          list.push_back(json_to_ossia_value(value[i]));
        }
        return list;
      };
      switch(N)
      {
        case 2:
          if(value[0].IsDouble() && value[1].GetDouble())
            return ossia::make_vec(value[0].GetDouble(), value[1].GetDouble());
          else
            return handle_vec(value);
        case 3:
          if(value[0].IsDouble() && value[1].GetDouble() && value[2].GetDouble())
            return ossia::make_vec(
                value[0].GetDouble(), value[1].GetDouble(), value[2].GetDouble());
          else
            return handle_vec(value);
        case 4:
          if(value[0].IsDouble() && value[1].GetDouble() && value[2].GetDouble()
             && value[3].GetDouble())
            return ossia::make_vec(
                value[0].GetDouble(), value[1].GetDouble(), value[2].GetDouble(),
                value[3].GetDouble());
          else
            return handle_vec(value);
        default:
          return handle_vec(value);
      }
    }
    default:
      break;
  }

  return val;
}

static bool is_nested(const rapidjson::Value& v)
{
  return (v.IsArray() || v.IsObject());
}

static void explore(
    std::string root, const rapidjson::Value& jsonval, ossia::presets::preset* preset)
{
  if(is_nested(jsonval))
  {
    if(jsonval.IsObject())
    {
      for(auto member = jsonval.MemberBegin(); member != jsonval.MemberEnd(); ++member)
      {
        explore(root + "/" + member->name.GetString(), member->value, preset);
      }
    }

    if(jsonval.IsArray())
    {
      int arrsize = jsonval.Size();
      bool is_not_obj = true;
      for(int i = 0; i < arrsize; ++i)
      {
        is_not_obj &= !jsonval[i].IsObject();
        if(!is_not_obj)
          break;
      }

      if(!is_not_obj)
      {
        for(int i = 0; i < arrsize; ++i)
        {
          explore(fmt::format("{}.{}", root, i), jsonval[i], preset);
        }
      }
      else
      {
        ossia::presets::preset_pair pp(root, json_to_ossia_value(jsonval));
        preset->push_back(pp);
      }
    }
    /*
    if (jsonval.IsArray())
    {
      ossia::presets::preset_pair pp(root, json_to_ossia_value(jsonval));
      preset->push_back(pp);
    }*/
  }
  else
  {
    ossia::presets::preset_pair pp(root, json_to_ossia_value(jsonval));
    preset->push_back(pp);
  }
}

ossia::presets::preset
ossia::presets::read_json(const std::string& str, bool skip_first_level)
{
  preset prst;
  std::string preset;
  preset.reserve(str.size() + 64);
  preset.assign(str.begin(), str.end());

  rapidjson::Document doc;
  doc.Parse(preset.c_str());

  if(!doc.HasParseError())
  {
    if(!skip_first_level)
    {
      explore("", doc, &prst);
    }
    else
    {
      if(doc.IsObject())
      {
        const auto& obj = doc.GetObject();
        if(obj.MemberCount() > 0)
        {
          explore("", obj.MemberBegin()->value, &prst);
        }
      }
    }
    return prst;
  }
  else
  {
    ossiaException_InvalidJSON exc(__LINE__, __FILE__);
    throw exc;
  }
}

struct value_to_json_preset_value
{
  rapidjson::Document::AllocatorType& allocator;
  rapidjson::Value operator()(ossia::impulse) const { return rapidjson::Value{}; }
  rapidjson::Value operator()(int v) const { return rapidjson::Value{v}; }
  rapidjson::Value operator()(float v) const { return rapidjson::Value{v}; }
  rapidjson::Value operator()(bool v) const { return rapidjson::Value{v}; }
  rapidjson::Value operator()(char v) const
  {
    return rapidjson::Value{&v, 1, allocator};
  } // 1-char string
  rapidjson::Value operator()(const std::string& v) const
  {
    return rapidjson::Value{v, allocator};
  }

  template <std::size_t N>
  rapidjson::Value operator()(const std::array<float, N>& vec) const
  {
    rapidjson::Value v(rapidjson::kArrayType);
    for(std::size_t i = 0; i < N; i++)
    {
      v.PushBack(vec[i], allocator);
    }
    return v;
  }

  rapidjson::Value operator()(const std::vector<ossia::value>& vec) const
  {
    rapidjson::Value v(rapidjson::kArrayType);
    for(std::size_t i = 0; i < vec.size(); i++)
    {
      v.PushBack(vec[i].apply(*this), allocator);
    }
    return v;
  }
  rapidjson::Value operator()() const
  {
    throw std::runtime_error("value_to_json_value: no type");
  }
};

static rapidjson::Value ossia_to_json_value(
    const ossia::value& val, rapidjson::Document::AllocatorType& docallocator)
{
  return val.apply(value_to_json_preset_value{docallocator});
}

template <typename Alloc>
static void insert(
    rapidjson::Value& recipient, const std::vector<std::string>& keys,
    const ossia::value& val, Alloc& doc);

template <typename Alloc>
static void insert_array(
    rapidjson::Value& array, const std::vector<std::string>& keys,
    const ossia::value& val, Alloc& alloc, const std::vector<std::string>& arraykeys)
{

  if(!array.IsArray())
  {
    array.SetArray();
  }

  if(!arraykeys.empty())
  {
    std::size_t index = std::atoi(arraykeys[0].c_str());
    if(index >= array.Size())
    {
      auto missing = index - array.Size() + 1;
      for(std::size_t i = 0; i < missing; ++i)
      {
        rapidjson::Value dummy;
        array.PushBack(dummy, alloc);
      }
    }
    if(arraykeys.size() == 1)
    {
      if(keys.empty())
      {
        rapidjson::Value v;
        v = ossia_to_json_value(val, alloc);
        array[index].Swap(v);
      }
      else
      {
        if(array[index].IsNull())
        {
          array[index].SetObject();
        }
        insert(array[index], keys, val, alloc);
      }
    }
    else
    {
      std::vector<std::string> newarraykeys(arraykeys);
      newarraykeys.erase(newarraykeys.begin());
      insert_array(array[index], keys, val, alloc, newarraykeys);
    }
  }
}

template <typename Alloc>
static void insert(
    rapidjson::Value& recipient, const std::vector<std::string>& keys,
    const ossia::value& val, Alloc& alloc)
{
  if(!keys.empty())
  {

    std::string currentkey = keys[0];
    std::vector<std::string> newkeys(keys);
    newkeys.erase(newkeys.begin());

    if(recipient.IsNull())
    {
      recipient.SetObject();
    }

    std::vector<std::string> arraykeys;
    boost::split(
        arraykeys, currentkey, [](char c) { return c == '.'; },
        boost::token_compress_on);
    if(arraykeys.size() > 1)
    {

      rapidjson::Value arrname(rapidjson::kStringType);
      std::string arrnamestr(arraykeys[0]);
      arrname.SetString(arrnamestr, alloc);
      arraykeys.erase(arraykeys.begin());

      if(!recipient.HasMember(arrname))
      {
        rapidjson::Value array(rapidjson::kArrayType);
        recipient.AddMember(arrname, array, alloc);
      }

      arrname.SetString(arrnamestr, alloc);
      insert_array(recipient[arrname], newkeys, val, alloc, arraykeys);
    }

    else
    {
      if(keys.size() == 1)
      {
        rapidjson::Value v = ossia_to_json_value(val, alloc);
        if(!recipient.HasMember(currentkey))
        {
          rapidjson::Value name(rapidjson::kStringType);
          name.SetString(currentkey, alloc);
          recipient.AddMember(name, v, alloc);
        }
        else
        {
          recipient[currentkey].Swap(v);
        }
      }

      else
      {
        if(!recipient.HasMember(currentkey))
        {
          rapidjson::Value node(rapidjson::kObjectType);
          rapidjson::Value name(rapidjson::kStringType);
          name.SetString(currentkey, alloc);
          recipient.AddMember(name, node, alloc);
        }
        insert(recipient[currentkey], newkeys, val, alloc);
      }
    }
  }
}

std::string ossia::presets::write_json(const std::string& devicename, const preset& prst)
{
  rapidjson::Document d;

  d.SetObject();
  rapidjson::Value obj;
  obj.SetObject();

  for(auto it = prst.begin(); it != prst.end(); ++it)
  {
    std::vector<std::string> keys;
    boost::split(
        keys, it->first, [](char c) { return c == '/'; }, boost::token_compress_on);

    if(!keys.empty() && keys.front().empty())
      keys.erase(keys.begin()); // first element is empty

    auto& val = it->second;
    insert(obj, keys, val, d.GetAllocator());
  }
  rapidjson::Value k;
  k.SetString(devicename, d.GetAllocator());
  d.AddMember(k, std::move(obj), d.GetAllocator());

  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  return buffer.GetString();
}

static std::string ossia_value_to_std_string(const ossia::value& val)
{
  std::stringstream ss;
  switch(val.get_type())
  {
    case ossia::val_type::BOOL:
      ss << "Bool " << val.get<bool>();
      break;
    case ossia::val_type::INT:
      ss << "Int " << val.get<int32_t>();
      break;
    case ossia::val_type::FLOAT:
      ss << "Float " << val.get<float>();
      break;
    case ossia::val_type::CHAR:
      ss << "Char " << val.get<char>();
      break;
    case ossia::val_type::STRING:
      ss << "String " << val.get<std::string>();
      break;
    default:
      ss << "Other";
  }
  return ss.str();
}

std::string ossia::presets::to_string(const preset& pr)
{
  std::string str;
  str.reserve(pr.size() * 32); // totally empirical
  for(const preset_pair& pp : pr)
  {
    str.append(pp.first);
    str += '\t';
    str.append(value_to_pretty_string(pp.second));
    str += '\n';
  }
  return str;
}

ossia::presets::preset ossia::presets::from_string(const ossia::string_view& str)
{
  preset ps;

  using boost::spirit::x3::phrase_parse;
  using ossia::detail::parse::preset_;
  auto first = str.cbegin(), last = str.cend();
  bool r = phrase_parse(first, last, preset_, boost::spirit::x3::ascii::space, ps);
  if(!r)
    ossia::logger().error("ossia::presetss::from_string error: {}", str);
  return ps;
}

std::string domain_to_string(const ossia::domain& domain)
{
  auto min = domain.get_min();
  auto max = domain.get_max();
  if(min.valid() && max.valid())
  {
    return ossia::convert<std::string>(min) + " " + ossia::convert<std::string>(max);
  }
  return {};
}

/// C++ Implementation: Devices ///
static rapidjson::Value
export_nodes_to_json(const ossia::net::node_base& node, rapidjson::Document& d)
{
  rapidjson::Value v;
  v.SetObject();
  auto& alloc = d.GetAllocator();

  auto address = node.get_parameter();
  if(address)
  {
    auto default_value = ossia::net::get_default_value(node);
    switch(address->get_value_type())
    {
      case ossia::val_type::IMPULSE: {
        v.AddMember("type", "impulse", alloc);
        break;
      }
      case ossia::val_type::BOOL: {
        v.AddMember("type", "boolean", alloc);

        if(default_value)
          if(auto valueDefault = default_value->target<bool>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }
        break;
      }
      case ossia::val_type::INT: {
        // append type attribute
        v.AddMember("type", "integer", alloc);

        // append default value attribute
        if(default_value)
          if(auto valueDefault = default_value->target<int>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        // append domain attribute
        auto domain = domain_to_string(address->get_domain());
        if(!domain.empty())
        {
          rapidjson::Value s;
          s.SetString(domain, alloc);
          v.AddMember("rangeBounds", s, alloc);
        }

        // append step size attribute
        auto valueStepSize = ossia::get_optional_attribute<ossia::net::value_step_size>(
            node, "valueStepSize");
        if(valueStepSize)
        {
          v.AddMember("valueStepSize", *valueStepSize, alloc);
        }

        break;
      }
      case ossia::val_type::FLOAT: {
        // append type attribute
        v.AddMember("type", "decimal", alloc);
        // append default value attribute
        if(default_value)
          if(auto valueDefault = default_value->target<float>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        // append domain attribute
        auto domain = domain_to_string(address->get_domain());
        if(!domain.empty())
        {
          rapidjson::Value s;
          s.SetString(domain, alloc);
          v.AddMember("rangeBounds", s, alloc);
        }

        // append step size attribute
        auto valueStepSize = ossia::net::get_value_step_size(node);
        if(valueStepSize)
        {
          v.AddMember("valueStepSize", *valueStepSize, alloc);
        }
        break;
      }
      case ossia::val_type::CHAR: {
        v.AddMember("type", "char", alloc);
        if(default_value)
          if(auto valueDefault = default_value->target<char>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        break;
      }
      case ossia::val_type::STRING: {
        auto ex = ossia::net::get_extended_type(node);
        if(ex && *ex == ossia::filesystem_path_type())
        {
          v.AddMember("type", "filepath", alloc);
        }
        else
        {
          v.AddMember("type", "string", alloc);
        }

        if(default_value)
          if(auto valueDefault = default_value->target<std::string>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }
        break;
      }
      default:
        break;
    }

    // append description attribute
    if(auto descr = ossia::net::get_description(node))
    {
      v.AddMember("description", *descr, alloc);
    }

    if(auto dynInstance = ossia::net::get_instance_bounds(node))
    {
      v.AddMember("dynamicInstances", true, alloc);
      v.AddMember(
          "instanceBounds",
          fmt::format("{} {}", dynInstance->min_instances, dynInstance->max_instances),
          alloc);
    }
  }

  for(const auto& child : node.children())
  {
    rapidjson::Value s;
    s.SetString(child->get_name(), alloc);
    v.AddMember(s, export_nodes_to_json(*child, d), alloc);
  }

  return v;
}

std::string ossia::presets::write_json(const ossia::net::device_base& deviceBase)
{
  rapidjson::Document d;
  d.SetObject();
  auto& alloc = d.GetAllocator();

  auto& node = deviceBase.get_root_node();

  // Device metadata
  // append app name attribute
  auto appName = ossia::get_optional_attribute<std::string>(node, "appName");
  if(appName)
  {
    rapidjson::Value tmp;
    tmp.SetString(*appName, alloc);
    d.AddMember("appName", tmp, alloc);
  }
  // append app version attribute
  auto appVersion = ossia::get_optional_attribute<std::string>(node, "appVersion");
  if(appVersion)
  {
    rapidjson::Value tmp;
    tmp.SetString(*appVersion, alloc);
    d.AddMember("appVersion", tmp, alloc);
  }
  // append app creator attribute
  auto appCreator = ossia::get_optional_attribute<std::string>(node, "appCreator");
  if(appCreator)
  {
    rapidjson::Value tmp;
    tmp.SetString(*appCreator, alloc);
    d.AddMember("appCreator", tmp, alloc);
  }

  // parse device node tree and export to json
  std::string deviceName = deviceBase.get_name();
  d.AddMember(
      rapidjson::StringRef(deviceName),
      export_nodes_to_json(deviceBase.get_root_node(), d), d.GetAllocator());

  // return json string
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  auto output = buffer.GetString();

  return output;
}

void ossia::presets::write_file(ossia::string_view content, ossia::string_view filename)
{
  std::ofstream out;
  out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  std::string sf(filename.data(), filename.size());
  out.open(sf);
  out << content;
  out.close();
}

const std::string ossia::presets::read_file(const std::string& filename)
{
  std::stringstream buffer;
  // output json file if needed
  if(!filename.empty())
  {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(filename);
    buffer << file.rdbuf();
    file.close();
  }
  return buffer.str();
}

static std::string preset_to_device_key(const std::string& presetkey)
{
  return presetkey;
  std::vector<std::string> indexes;
  boost::split(
      indexes, presetkey, [](char c) { return c == '.'; }, boost::token_compress_on);

  for(std::size_t i = 1; i < indexes.size(); ++i)
  {
    int current_index = std::atoi((indexes[i]).c_str());
    std::stringstream ss;
    ss << (current_index + 1);
    indexes[i] = std::string(ss.str());
  }

  return boost::join(indexes, ".");
}

static void apply_preset_node(
    ossia::net::node_base& root, std::vector<std::string> keys, const ossia::value& val,
    ossia::presets::keep_arch_type keeparch,
    std::vector<ossia::net::node_base*>& created_nodes, bool allow_nonterminal)
{
  if(keys.size() == 0)
  {
    if(!allow_nonterminal && root.children().size() > 0)
    {
      std::string details = "Node " + root.get_name() + " is not terminal";
      throw(ossia::ossiaException_InvalidAddress(__LINE__, __FILE__, details.c_str()));
    }
    else
    {
      root.get_parameter()->push_value(val);
    }
  }
  else
  {
    // TODO this is slow. Either use the last (fast) or use a list (less slow).
    std::string currentkey = preset_to_device_key(keys[0]);
    keys.erase(keys.begin());
    bool child_exists = false;

    for(auto& child : root.children())
    {
      if(!child)
      {
        auto err = std::string("Null child of : ") + root.get_name();
        ossia::logger().error("{}", err.c_str());
      }
      else
      {
        const std::string childName = child->get_name();
        if(currentkey == childName)
        {
          child_exists = true;
          apply_preset_node(
              *child, keys, val, keeparch, created_nodes, allow_nonterminal);
        }
        else
        {
          // Case of the 'bar.0' / 'bar'.
          const auto N = currentkey.size();
          if((childName.size() == (N - 2)) && currentkey[N - 2] == '.'
             && currentkey[N - 1] == '0')
          {
            currentkey.resize(currentkey.size() - 2);
            child_exists = true;
            apply_preset_node(
                *child, keys, val, keeparch, created_nodes, allow_nonterminal);
          }
        }
      }
    }

    if(!child_exists)
    {
      if(keeparch == ossia::presets::keep_arch_on)
      {
        ossia::logger().warn("Can't change device architecture");
        throw std::runtime_error("preset loading: can't change device architecture");
      }
      else
      {
        if(auto newchild = root.create_child(currentkey))
        {
          // We push them in the vector in their order of creation
          // This way when iterating we will have the parents before the
          // children.
          created_nodes.push_back(newchild);

          // addresses only on leaf nodes... maybe we should not have this
          // restriction.
          if(keys.empty())
            newchild->create_parameter(val.get_type());

          apply_preset_node(
              *newchild, keys, val, keeparch, created_nodes, allow_nonterminal);
        }
      }
    }
  }
}

static void on_instance_creation(
    ossia::net::node_base& created_node, const ossia::presets::instance_functions& funcs)
{
  auto full_address = ossia::net::address_string_from_node(created_node);

  for(auto& function : funcs)
  {
    // First check if there is a matching regex.
    if(std::regex_match(full_address, function.first))
    {
      // If it is the case we can call the given function on it.
      function.second(created_node);
    }
  }
}

void ossia::presets::apply_preset(
    ossia::net::node_base& node, const ossia::presets::preset& preset,
    ossia::presets::keep_arch_type keeparch, presets::instance_functions functions,
    bool allow_nonterminal, bool remove_first)
{
  std::vector<ossia::net::node_base*> created_nodes;

  for(auto itpp = preset.begin(); itpp != preset.end(); ++itpp)
  {
    std::vector<std::string> keys;
    boost::split(
        keys, itpp->first, [](char c) { return c == '/'; }, boost::token_compress_on);
    if(remove_first)
    {
      if(!keys.empty())
        keys.erase(keys.begin()); // first substring is empty
    }
    if(!allow_nonterminal)
    {
      if(!keys.empty())
        keys.erase(keys.begin()); // then we have to remove the "initial" key
                                  // which is the device name
    }
    if(node.get_parent())
      keys.erase(keys.begin()); // remove another one in case node is not a root

    apply_preset_node(
        node, keys, itpp->second, keeparch, created_nodes, allow_nonterminal);
  }

  for(auto node : created_nodes)
  {
    on_instance_creation(*node, functions);
  }
}

ossia::presets::preset
ossia::presets::make_preset(ossia::net::node_base& node, preset_save_options opt)
{
  ossia::presets::preset cue;
  auto nodes = ossia::net::list_all_children(&node);
  for(auto n : nodes)
  {
    if(auto param = n->get_parameter())
    {
      if(must_save_parameter(*param, opt))
      {
        std::string key = "/" + n->get_name();
        key.reserve(100);
        auto n1 = n->get_parent();
        while(n1 != &node)
        {
          key.insert(0, "/" + n1->get_name());
          n1 = n1->get_parent();
        }
        cue.push_back({key, param->value()});
      }
    }
  }

  return cue;
}

ossia::net::node_base*
get_node_node(ossia::net::node_base& root, std::vector<std::string>& keys)
{
  ossia::net::node_base* res = nullptr;

  if(keys.size() > 0)
  {
    std::string currentkey = keys[0];

    if(root.get_name().compare(currentkey) == 0)
    {
      if(keys.size() == 1)
      {
        res = &root;
      }
      else
      {
        keys.erase(keys.begin());
        for(auto& child : root.children())
        {
          auto childres = get_node_node(*child, keys);
          if(childres != nullptr)
          {
            res = childres;
          }
        }
      }
    }
  }
  return res;
}

ossia::net::node_base*
ossia::presets::get_node(ossia::net::node_base& root, const std::string& nodeaddr)
{
  std::vector<std::string> keys;
  boost::split(keys, nodeaddr, [](char c) { return c == '/'; });
  keys.erase(keys.begin());

  return get_node_node(root, keys);
}

static void to_string_node(
    const ossia::net::node_base& root, std::vector<std::string>& strnodes,
    std::vector<std::string>& keys)
{
  for(auto& child : root.children())
  {
    std::vector<std::string> newkeys = keys;
    newkeys.push_back(root.get_name());
    to_string_node(*child, strnodes, newkeys);
  }

  if(root.get_parameter() != nullptr && root.children().size() == 0)
  {
    auto v = root.get_parameter()->value();
    if(v.valid())
    {
      keys.push_back(root.get_name());
      std::string nodename = boost::join(keys, "/");
      strnodes.push_back(nodename + ": " + ossia_value_to_std_string(v));
    }
  }
}

std::string ossia::presets::to_string(const ossia::net::device_base& ossiadev)
{
  auto& root = ossiadev.get_root_node();
  std::stringstream ss;
  std::vector<std::string> strnodes;
  std::vector<std::string> keys;
  to_string_node(root, strnodes, keys);
  ss << "[" << boost::join(strnodes, ", ") << "]";
  return std::string(ss.str());
}

static void make_json_from_node(
    rapidjson::Document& d, ossia::net::node_base* node,
    ossia::presets::preset_save_options opt)
{
  std::vector<ossia::net::node_base*> children = node->children_copy();
  rapidjson::Document subdoc(&d.GetAllocator());
  subdoc.SetObject();

  // Save the parameter
  if(auto param = node->get_parameter())
  {
    if(must_save_parameter(*param, opt))
    {
      rapidjson::Value v
          = param->value().apply(value_to_json_preset_value{d.GetAllocator()});
      rapidjson::Value name(node->get_name(), d.GetAllocator());
      d.AddMember(name, v, d.GetAllocator());
    }
  }

  // Recurse
  if(children.empty())
    return;

  sort_by_priority(children);

  for(auto it = children.begin(); it != children.end(); it++)
  {
    make_json_from_node(subdoc, *it, opt);
  }

  rapidjson::Value name(node->get_name(), d.GetAllocator());
  d.AddMember(name, subdoc, d.GetAllocator());
}

const std::string ossia::presets::make_json_preset(
    const ossia::net::node_base& node, preset_save_options opt)
{
  std::vector<ossia::net::node_base*> children = node.children_copy();

  sort_by_priority(children);

  rapidjson::Document doc;
  doc.SetObject();
  for(auto it = children.begin(); it != children.end(); it++)
  {
    make_json_from_node(doc, *it, opt);
  }

  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return buffer.GetString();
}

static void apply_json_member_to_node(
    rapidjson::Value::ConstMemberIterator object, ossia::net::node_base& node,
    ossia::presets::func_t fn)
{
  if(object->value.IsObject())
  {
    for(auto it = object->value.MemberBegin(); it != object->value.MemberEnd(); ++it)
    {
      std::string name = it->name.GetString();
      auto n = ossia::net::find_node(node, it->name.GetString());
      if(n && !ossia::net::get_recall_safe(*n))
      {
        apply_json_member_to_node(it, *n, fn);
      }
    }
  }
  else
  {
    if(!ossia::net::get_recall_safe(node))
    {
      if(auto param = node.get_parameter())
      {
        param->push_value(json_to_ossia_value(object->value));
        if(fn)
          fn(&node);
      }
    }
  }
}

bool ossia::presets::apply_json(
    const std::string& json, ossia::net::node_base& node, func_t fn)
{
  rapidjson::Document doc;
  doc.Parse(json.c_str());
  if(doc.HasParseError())
    return false;
  else
  {
    if(doc.IsObject())
    {
      for(auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
      {
        std::string name = it->name.GetString();
        auto n = ossia::net::find_node(node, name);
        if(n && !ossia::net::get_recall_safe(*n))
          apply_json_member_to_node(it, *n, fn);
      }
    }
  }
  return true;
}

void ossia::presets::apply_preset(
    const std::string& kiss, ossia::net::node_base& node, ossia::presets::func_t fn)
{
  auto preset = ossia::presets::from_string(kiss);
  for(auto& p : preset)
  {
    if(auto n = ossia::net::find_node(node, p.first))
    {
      if(ossia::net::get_recall_safe(*n))
        continue;

      if(auto param = n->get_parameter())
      {
        param->push_value(p.second);
        if(fn)
          fn(n);
      }
    }
  }
}

#if defined(OSSIA_C)
/// Exception handling ///

static ossia_preset_result lippincott()
{
  try
  {
    try
    {
      throw;
    }
    catch(const ossia::ossiaException_InvalidAddress& b)
    {
      ossia::logger().error("{}", b.what());
      return OSSIA_PRESETS_INVALID_ADDRESS;
    }
    catch(const ossia::ossiaException_InvalidJSON& b)
    {
      ossia::logger().error("{}", b.what());
      return OSSIA_PRESETS_INVALID_JSON;
    }
    catch(const ossia::ossiaException_InvalidXML& b)
    {
      ossia::logger().error("{}", b.what());
      return OSSIA_PRESETS_INVALID_XML;
    }
    catch(const ossia::ossiaException& b)
    {
      ossia::logger().error("{}", b.what());
      return OSSIA_PRESETS_GENERIC_EXCEPTION;
    }
    catch(const std::exception& e)
    {
      ossia::logger().error("{}", e.what());
      return OSSIA_PRESETS_STANDARD_EXCEPTION;
    }
    catch(...)
    {
      return OSSIA_PRESETS_UNKNOWN_ERROR;
    }
  }
  catch(...)
  {
    return OSSIA_PRESETS_UNKNOWN_ERROR;
  }
}
#endif
