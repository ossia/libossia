// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/json.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdio>
#include <iostream>
#include <memory>
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sstream>
#include <string>
#include <vector>

#include <ossia/detail/logger.hpp>
#include <ossia-c/log/ossia_log.h>
#include <ossia/preset/exception.hpp>
#include <ossia-c/preset/preset.h>
#include <ossia/preset/preset.hpp>
#include <ossia-c/preset/result.h>

#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/ossia.hpp>
#include <ossia-c/ossia/ossia_utils.hpp>
#include <ossia/editor/value/detail/value_parse_impl.hpp>

struct ossia_preset
{
  ossia::presets::preset impl;
  ossia_preset() : impl(ossia::presets::preset())
  {
  }
  ossia_preset(const ossia::presets::preset& prst) : impl(prst)
  {
  }
};

/// C functions ///

extern "C" {
static ossia_preset_result lippincott();

ossia_preset_result
ossia_presets_read_json(const char* str, ossia_preset_t* presetptr)
{
  if (presetptr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }

  *presetptr = nullptr;
  if (str != nullptr)
  {
    try
    {
      *presetptr
          = new ossia_preset(ossia::presets::read_json(std::string(str)));
      return OSSIA_PRESETS_OK;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_EMPTY_JSON;
}

ossia_preset_result ossia_presets_free(ossia_preset_t preset)
{
  if (preset != nullptr)
  {
    try
    {
      delete preset;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_OK;
}

ossia_preset_result ossia_presets_write_json(
    const ossia_preset_t preset, const char* device, const char** buffer)
{
  if (preset)
  {
    try
    {
      *buffer = copy_string(ossia::presets::write_json(device, preset->impl));
      return OSSIA_PRESETS_OK;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_size(const ossia_preset_t preset, int* size)
{
  if (preset)
  {
    try
    {
      *size = preset->impl.size();
      return OSSIA_PRESETS_OK;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result
ossia_presets_to_string(const ossia_preset_t preset, const char** buffer)
{
  if (preset)
  {
    try
    {
      *buffer = copy_string(ossia::presets::to_string(preset->impl));
      return OSSIA_PRESETS_OK;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result
ossia_devices_write_json(const ossia_device_t odev, const char** buffer)
{
  if (odev != nullptr)
  {
    try
    {
      assert(odev->device);
      *buffer = copy_string(ossia::presets::write_json(*(odev->device)));
      return OSSIA_PRESETS_OK;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_apply_preset(
    ossia_device_t odevptr, ossia_preset_t preset, bool keep_arch)
{
  if (!odevptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if (!preset)
  {
    return OSSIA_PRESETS_NULL_PRESET;
  }
  try
  {
    ossia::presets::keep_arch_type keep_arch_token;
    if (keep_arch)
      keep_arch_token = ossia::presets::keep_arch_on;
    else
      keep_arch_token = ossia::presets::keep_arch_off;

    if (!odevptr->device)
    {
      return OSSIA_PRESETS_NULL_DEVICE;
    }

    ossia::presets::apply_preset(
        odevptr->device->get_root_node(), preset->impl, keep_arch_token);
  }
  catch (...)
  {
    return lippincott();
  }
  return OSSIA_PRESETS_OK;
}

ossia_preset_result
ossia_devices_make_preset(ossia_device_t odev, ossia_preset_t* presetptr)
{
  if (presetptr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }

  *presetptr = nullptr;
  if (odev != nullptr)
  {
    try
    {
      assert(odev->device);
      *presetptr
          = new ossia_preset(ossia::presets::make_preset(odev->device->get_root_node()));
      return OSSIA_PRESETS_OK;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result
ossia_devices_to_string(ossia_device_t odev, const char** buffer)
{
  if (odev != nullptr)
  {
    try
    {
      assert(odev->device);
      *buffer = copy_string(ossia::presets::to_string(*(odev->device)));
      return OSSIA_PRESETS_OK;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_get_node(
    ossia_device_t odev, const char* addr, ossia_node_t* nodeptr)
{
  if (nodeptr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if (addr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_ADDRESS;
  }
  if (odev != nullptr)
  {
    try
    {
      assert(odev->device);
      auto gotnode
          = ossia::presets::get_node(odev->device->get_root_node(), addr);
      if (gotnode == nullptr)
      {
        return OSSIA_PRESETS_INVALID_ADDRESS;
      }
      *nodeptr = convert(gotnode);
      return OSSIA_PRESETS_OK;
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_get_child(
    ossia_node_t root, const char* childname, ossia_node_t* nodeptr)
{
  if (nodeptr == nullptr)
  {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if (childname == nullptr)
  {
    return OSSIA_PRESETS_INVALID_ADDRESS;
  }
  if (root == nullptr)
  {
    return OSSIA_PRESETS_NULL_NODE;
  }
  try
  {
    auto gotnode = ossia::presets::get_node(*convert_node(root), childname);
    if (gotnode == nullptr)
    {
      return OSSIA_PRESETS_INVALID_ADDRESS;
    }
    *nodeptr = convert(gotnode);
  }
  catch (...)
  {
    return lippincott();
  }
  return OSSIA_PRESETS_OK;
}

bool ossia_presets_has_key(const ossia_preset_t preset, const char* key)
{
  if (preset && key)
  {
    try
    {
      return preset->impl.find(key) != preset->impl.end();
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return false;
}

ossia_preset_result ossia_presets_key_to_string(
    const ossia_preset_t preset, const char* key, const char** buffer)
{
  if (preset && key)
  {
    try
    {
      auto it = preset->impl.find(key);
      if (it != preset->impl.end())
      {
        *buffer = copy_string(ossia::convert<std::string>(it->second));
        return OSSIA_PRESETS_OK;
      }
      else
      {
        return OSSIA_PRESETS_KEY_NOT_FOUND;
      }
    }
    catch (...)
    {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_key_to_value(
    const ossia_preset_t preset, const char* key, ossia_value_t* buffer)
{
  if (preset && key)
  {
    try
    {
      auto it = preset->impl.find(key);
      if (it != preset->impl.end())
      {
        *buffer = convert(it->second);
        return OSSIA_PRESETS_OK;
      }
      else
      {
        return OSSIA_PRESETS_KEY_NOT_FOUND;
      }
    }
    catch (...)
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

/// C++ Implementations: Presets ///

ossia::value json_to_ossia_value(const rapidjson::Value& value)
{

  ossia::value val;
  enum rapidjson::Type tval = value.GetType();

  switch (tval)
  {
    case rapidjson::kFalseType:
      val = bool(false);
      break;
    case rapidjson::kTrueType:
      val = bool(true);
      break;
    case rapidjson::kNumberType:
      if (value.IsInt())
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
    default:
      break;
  }

  return val;
}

bool is_nested(const rapidjson::Value& v)
{
  return (v.IsArray() || v.IsObject());
}

void explore(
    std::string root, const rapidjson::Value& jsonval,
    ossia::presets::preset* preset)
{

  if (is_nested(jsonval))
  {
    if (jsonval.IsObject())
    {
      for (auto member = jsonval.MemberBegin(); member != jsonval.MemberEnd();
           ++member)
      {
        explore(root + "/" + member->name.GetString(), member->value, preset);
      }
    }
    if (jsonval.IsArray())
    {
      int arrsize = jsonval.Size();
      for (int i = 0; i < arrsize; ++i)
      {
        std::stringstream newroot;
        newroot << root << "." << i;
        explore(newroot.str(), jsonval[i], preset);
      }
    }
  }
  else
  {
    ossia::presets::preset_pair pp(root, json_to_ossia_value(jsonval));
    preset->insert(pp);
  }
}

ossia::presets::preset ossia::presets::read_json(const std::string& str)
{
  preset prst;

  rapidjson::Document doc;
  doc.Parse(str.c_str());

  if (!doc.HasParseError())
  {
    explore("", doc, &prst);
    return prst;
  }
  else
  {
    ossiaException_InvalidJSON exc(__LINE__, __FILE__);
    throw exc;
  }
}

rapidjson::Value ossia_to_json_value(
    const ossia::value& val, rapidjson::Document::AllocatorType& docallocator)
{
  return val.apply(ossia::oscquery::detail::value_to_json_value{docallocator});
}

template <typename Alloc>
void insert(
    rapidjson::Value& recipient, const std::vector<std::string>& keys,
    const ossia::value& val, Alloc& doc);

template <typename Alloc>
void insert_array(
    rapidjson::Value& array, const std::vector<std::string>& keys,
    const ossia::value& val, Alloc& alloc,
    const std::vector<std::string>& arraykeys)
{

  if (!array.IsArray())
  {
    array.SetArray();
  }

  if (!arraykeys.empty())
  {
    std::size_t index = std::atoi(arraykeys[0].c_str());
    if (index >= array.Size())
    {
      auto missing = index - array.Size() + 1;
      for (std::size_t i = 0; i < missing; ++i)
      {
        rapidjson::Value dummy;
        array.PushBack(dummy, alloc);
      }
    }
    if (arraykeys.size() == 1)
    {
      if (keys.empty())
      {
        rapidjson::Value v;
        v = ossia_to_json_value(val, alloc);
        array[index].Swap(v);
      }
      else
      {
        if (array[index].IsNull())
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
void insert(
    rapidjson::Value& recipient, const std::vector<std::string>& keys,
    const ossia::value& val, Alloc& alloc)
{
  if (!keys.empty())
  {

    std::string currentkey = keys[0];
    std::vector<std::string> newkeys(keys);
    newkeys.erase(newkeys.begin());

    if (recipient.IsNull())
    {
      recipient.SetObject();
    }

    std::vector<std::string> arraykeys;
    boost::split(
        arraykeys, currentkey, [](char c) { return c == '.'; },
        boost::token_compress_on);
    if (arraykeys.size() > 1)
    {

      rapidjson::Value arrname(rapidjson::kStringType);
      std::string arrnamestr(arraykeys[0]);
      arrname.SetString(arrnamestr, alloc);
      arraykeys.erase(arraykeys.begin());

      if (!recipient.HasMember(arrname))
      {
        rapidjson::Value array(rapidjson::kArrayType);
        recipient.AddMember(arrname, array, alloc);
      }

      arrname.SetString(arrnamestr, alloc);
      insert_array(recipient[arrname], newkeys, val, alloc, arraykeys);
    }

    else
    {
      if (keys.size() == 1)
      {
        rapidjson::Value v = ossia_to_json_value(val, alloc);
        if (!recipient.HasMember(currentkey))
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
        if (!recipient.HasMember(currentkey))
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

std::string
ossia::presets::write_json(const std::string& devicename, const preset& prst)
{
  rapidjson::Document d;

  d.SetObject();
  rapidjson::Value obj;
  obj.SetObject();

  for (auto it = prst.begin(); it != prst.end(); ++it)
  {
    std::vector<std::string> keys;
    boost::split(
        keys, it->first, [](char c) { return c == '/'; },
        boost::token_compress_on);
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

std::string ossia_value_to_std_string(const ossia::value& val)
{
  std::stringstream ss;
  switch (val.getType())
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
  for (const preset_pair& pp : pr)
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

  using boost::spirit::x3::parse;
  using ossia::detail::parse::preset_;
  auto first = str.cbegin(), last = str.cend();
  bool r = parse(first, last, preset_, ps);
  if(!r)
    ossia::logger().error("ossia::presetss::from_string error: {}", str);
  return ps;
}

std::string domain_to_string(const ossia::domain& domain)
{
  auto min = domain.get_min();
  auto max = domain.get_max();
  if (min.valid() && max.valid())
  {
    return ossia::convert<std::string>(min) + " "
           + ossia::convert<std::string>(max);
  }
  return {};
}

/// C++ Implementation: Devices ///
rapidjson::Value
export_nodes_to_json(const ossia::net::node_base& node, rapidjson::Document& d)
{
  rapidjson::Value v;
  v.SetObject();
  auto& alloc = d.GetAllocator();

  auto address = node.get_parameter();
  if (address)
  {
    auto default_value = ossia::net::get_default_value(node);
    switch (address->get_value_type())
    {
      case ossia::val_type::IMPULSE:
      {
        v.AddMember("type", "impulse", alloc);
        break;
      }
      case ossia::val_type::BOOL:
      {
        v.AddMember("type", "boolean", alloc);

        if (default_value)
          if (auto valueDefault = default_value->target<bool>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }
        break;
      }
      case ossia::val_type::INT:
      {
        // append type attribute
        v.AddMember("type", "integer", alloc);

        // append default value attribute
        if (default_value)
          if (auto valueDefault = default_value->target<int>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        // append domain attribute
        auto domain = domain_to_string(address->get_domain());
        if (!domain.empty())
        {
          rapidjson::Value s;
          s.SetString(domain, alloc);
          v.AddMember("rangeBounds", s, alloc);
        }

        // append step size attribute
        auto valueStepSize
            = ossia::get_optional_attribute<ossia::net::value_step_size>(
                node, "valueStepSize");
        if (valueStepSize)
        {
          v.AddMember("valueStepSize", *valueStepSize, alloc);
        }

        break;
      }
      case ossia::val_type::FLOAT:
      {
        // append type attribute
        v.AddMember("type", "decimal", alloc);
        // append default value attribute
        if (default_value)
          if (auto valueDefault = default_value->target<float>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        // append domain attribute
        auto domain = domain_to_string(address->get_domain());
        if (!domain.empty())
        {
          rapidjson::Value s;
          s.SetString(domain, alloc);
          v.AddMember("rangeBounds", s, alloc);
        }

        // append step size attribute
        auto valueStepSize = ossia::net::get_value_step_size(node);
        if (valueStepSize)
        {
          v.AddMember("valueStepSize", *valueStepSize, alloc);
        }
        break;
      }
      case ossia::val_type::CHAR:
      {
        v.AddMember("type", "char", alloc);
        if (default_value)
          if (auto valueDefault = default_value->target<char>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        break;
      }
      case ossia::val_type::STRING:
      {
        auto ex = ossia::net::get_extended_type(node);
        if (ex && *ex == ossia::filesystem_path_type())
        {
          v.AddMember("type", "filepath", alloc);
        }
        else
        {
          v.AddMember("type", "string", alloc);
        }

        if (default_value)
          if (auto valueDefault = default_value->target<std::string>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }
        break;
      }
      default:
        break;
    }

    // append description attribute
    if (auto descr = ossia::net::get_description(node))
    {
      v.AddMember("description", *descr, alloc);
    }

    if (auto dynInstance = ossia::net::get_instance_bounds(node))
    {
      v.AddMember("dynamicInstances", true, alloc);
      v.AddMember(
          "instanceBounds",
          fmt::format(
              "{} {}", dynInstance->min_instances, dynInstance->max_instances),
          alloc);
    }
  }

  for (const auto& child : node.children())
  {
    rapidjson::Value s;
    s.SetString(child->get_name(), alloc);
    v.AddMember(s, export_nodes_to_json(*child, d), alloc);
  }

  return v;
}

std::string
ossia::presets::write_json(const ossia::net::device_base& deviceBase)
{
  rapidjson::Document d;
  d.SetObject();
  auto& alloc = d.GetAllocator();

  auto& node = deviceBase.get_root_node();

  // Device metadata
  // append app name attribute
  auto appName = ossia::get_optional_attribute<std::string>(node, "appName");
  if (appName)
  {
    rapidjson::Value tmp;
    tmp.SetString(*appName, alloc);
    d.AddMember("appName", tmp, alloc);
  }
  // append app version attribute
  auto appVersion
      = ossia::get_optional_attribute<std::string>(node, "appVersion");
  if (appVersion)
  {
    rapidjson::Value tmp;
    tmp.SetString(*appVersion, alloc);
    d.AddMember("appVersion", tmp, alloc);
  }
  // append app creator attribute
  auto appCreator
      = ossia::get_optional_attribute<std::string>(node, "appCreator");
  if (appCreator)
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

void ossia::presets::write_file(
    ossia::string_view content, ossia::string_view filename)
{
  std::ofstream out;
  out.exceptions ( std::ofstream::failbit | std::ofstream::badbit );
  std::string sf(filename.data(), filename.size());
  out.open(sf);
  out << content;
  out.close();
}

const std::string ossia::presets::read_file(
    const std::string& filename)
{
  std::stringstream buffer;
  // output json file if needed
  if (!filename.empty())
  {
    std::ifstream file;
    file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    file.open(filename);
    buffer << file.rdbuf();
  }
  return buffer.str();
}

std::string preset_to_device_key(const std::string& presetkey)
{
  return presetkey;
  std::vector<std::string> indexes;
  boost::split(
      indexes, presetkey, [](char c) { return c == '.'; },
      boost::token_compress_on);

  for (std::size_t i = 1; i < indexes.size(); ++i)
  {
    int current_index = std::atoi((indexes[i]).c_str());
    std::stringstream ss;
    ss << (current_index + 1);
    indexes[i] = std::string(ss.str());
  }

  return boost::join(indexes, ".");
}

std::string device_to_preset_key(
    const ossia::net::node_base& node, const ossia::net::node_base& parent)
{
  if (!parent.is_root_instance(node))
    return node.get_name();
  return node.get_name() + ".0";
}

bool instance_string_compare(
    const std::string& str, const ossia::net::node_base& node)
{
  auto node_name = node.get_name();
  auto opt = ossia::net::sanitize_name(str, {});
  return opt == node_name;
}

void apply_preset_node(
    ossia::net::node_base& root, std::vector<std::string> keys,
    const ossia::value& val, ossia::presets::keep_arch_type keeparch,
    std::vector<ossia::net::node_base*>& created_nodes)
{
  if (keys.size() == 0)
  {
    if (root.children().size() > 0)
    {
      std::string details = "Node " + root.get_name() + " is not terminal";
      throw(ossia::ossiaException_InvalidAddress(
          __LINE__, __FILE__, details.c_str()));
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

    for (auto& child : root.children())
    {
      if (!child)
      {
        auto err = std::string("Null child of : ") + root.get_name();
        ossia::logger().error("{}", err.c_str());
      }
      else
      {
        const std::string childName = child->get_name();
        if (currentkey == childName)
        {
          child_exists = true;
          apply_preset_node(*child, keys, val, keeparch, created_nodes);
        }
        else
        {
          // Case of the 'bar.0' / 'bar'.
          const auto N = currentkey.size();
          if ((childName.size() == (N - 2)) && currentkey[N - 2] == '.'
              && currentkey[N - 1] == '0')
          {
            currentkey.resize(currentkey.size() - 2);
            child_exists = true;
            apply_preset_node(*child, keys, val, keeparch, created_nodes);
          }
        }
      }
    }

    if (!child_exists)
    {
      if (keeparch == ossia::presets::keep_arch_on)
      {
        ossia::logger().warn("{}:{}  {}", __LINE__, __FILE__, "Can't change device architecture");
      }
      else
      {
        if (auto newchild = root.create_child(currentkey))
        {
          // We push them in the vector in their order of creation
          // This way when iterating we will have the parents before the
          // children.
          created_nodes.push_back(newchild);

          // addresses only on leaf nodes... maybe we should not have this
          // restriction.
          if (keys.empty())
            newchild->create_parameter(val.getType());

          apply_preset_node(*newchild, keys, val, keeparch, created_nodes);
        }
      }
    }
  }
}

void on_instance_creation(
    ossia::net::node_base& created_node,
    const ossia::presets::instance_functions& funcs)
{
  auto full_address = ossia::net::address_string_from_node(created_node);

  for (auto& function : funcs)
  {
    // First check if there is a matching regex.
    if (std::regex_match(full_address, function.first))
    {
      // If it is the case we can call the given function on it.
      function.second(created_node);
    }
  }
}

void ossia::presets::apply_preset(
    ossia::net::node_base& node, const ossia::presets::preset& preset,
    ossia::presets::keep_arch_type keeparch,
    presets::instance_functions functions)
{
  std::vector<ossia::net::node_base*> created_nodes;

  for (auto itpp = preset.begin(); itpp != preset.end(); ++itpp)
  {
    std::vector<std::string> keys;
    boost::split(
        keys, itpp->first, [](char c) { return c == '/'; },
        boost::token_compress_on);
    if(!keys.empty())
      keys.erase(keys.begin()); // first subtring is empty
    if(!keys.empty())
      keys.erase(keys.begin()); // then we have to remove the "initial" key which is the device name
    if(node.get_parent())
      keys.erase(keys.begin()); // remove another one in case node is not a root

    apply_preset_node(
        node, keys, itpp->second, keeparch, created_nodes);
  }

  for (auto node : created_nodes)
  {
    on_instance_creation(*node, functions);
  }
}

void make_preset_node(
    ossia::net::node_base& node, ossia::presets::preset& preset,
    const std::string& key)
{
  std::string currentkey = key;
  if (auto parent = node.get_parent())
  {
    currentkey += "/" + device_to_preset_key(node, *parent);
  }
#if !defined(OSSIA_HAS_SHARED_MUTEX)
  auto children = node.children_copy();
#else
  const auto& children = node.children();
#endif

  if (children.size() == 0)
  {
    if (auto addr = node.get_parameter())
      preset.insert(std::make_pair(currentkey, addr->value()));
  }
  else
  {
    for (auto& child : children)
    {
      make_preset_node(*child, preset, currentkey);
    }
  }
}

ossia::presets::preset
ossia::presets::make_preset(ossia::net::node_base& node)
{
  presets::preset preset;
  make_preset_node(node, preset, "");
  return preset;
}

ossia::net::node_base*
get_node_node(ossia::net::node_base& root, std::vector<std::string>& keys)
{
  ossia::net::node_base* res = nullptr;

  if (keys.size() > 0)
  {
    std::string currentkey = keys[0];

    if (root.get_name().compare(currentkey) == 0)
    {
      if (keys.size() == 1)
      {
        res = &root;
      }
      else
      {
        keys.erase(keys.begin());
        for (auto& child : root.children())
        {
          auto childres = get_node_node(*child, keys);
          if (childres != nullptr)
          {
            res = childres;
          }
        }
      }
    }
  }
  return res;
}

ossia::net::node_base* ossia::presets::get_node(
    ossia::net::node_base& root, const std::string& nodeaddr)
{
  std::vector<std::string> keys;
  boost::split(keys, nodeaddr, [](char c) { return c == '/'; });
  keys.erase(keys.begin());

  return get_node_node(root, keys);
}

void to_string_node(
    const ossia::net::node_base& root, std::vector<std::string>& strnodes,
    std::vector<std::string>& keys)
{
  for (auto& child : root.children())
  {
    std::vector<std::string> newkeys = keys;
    newkeys.push_back(root.get_name());
    to_string_node(*child, strnodes, newkeys);
  }

  if (root.get_parameter() != nullptr && root.children().size() == 0)
  {
    auto v = root.get_parameter()->value();
    if (v.valid())
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

/// Exception handling ///

static ossia_preset_result lippincott()
{
  try
  {
    try
    {
      throw;
    }
    catch (const ossia::ossiaException_InvalidAddress& b)
    {
      ossia::logger().error("{}", b.what());
      return OSSIA_PRESETS_INVALID_ADDRESS;
    }
    catch (const ossia::ossiaException_InvalidJSON& b)
    {
      ossia::logger().error("{}", b.what());
      return OSSIA_PRESETS_INVALID_JSON;
    }
    catch (const ossia::ossiaException_InvalidXML& b)
    {
      ossia::logger().error("{}", b.what());
      return OSSIA_PRESETS_INVALID_XML;
    }
    catch (const ossia::ossiaException& b)
    {
      ossia::logger().error("{}", b.what());
      return OSSIA_PRESETS_GENERIC_EXCEPTION;
    }
    catch (const std::exception& e)
    {
      ossia::logger().error("{}", e.what());
      return OSSIA_PRESETS_STANDARD_EXCEPTION;
    }
    catch (...)
    {
      return OSSIA_PRESETS_UNKNOWN_ERROR;
    }
  }
  catch (...)
  {
    return OSSIA_PRESETS_UNKNOWN_ERROR;
  }
}
