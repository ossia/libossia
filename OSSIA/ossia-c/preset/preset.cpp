#include <memory>
#include <sstream>
#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <ossia/detail/json.hpp>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/allocators.h>
#include <rapidjson/filewritestream.h>

#include <ossia-c/log/ossia_log.h>
#include <ossia-c/preset/preset.h>
#include <ossia-c/preset/preset.hpp>
#include <ossia-c/preset/exception.hpp>
#include <ossia-c/preset/result.h>

#include <ossia/ossia.hpp>
#include <ossia-c/ossia/ossia_utils.hpp>
#include <boost/lexical_cast.hpp>

struct ossia_preset
{
  ossia::presets::preset impl;
  ossia_preset(): impl(ossia::presets::preset ()) {}
  ossia_preset(const ossia::presets::preset& prst) : impl(prst){}
};

/// C functions ///

extern "C" {
static ossia_preset_result lippincott();

ossia_preset_result ossia_presets_read_json(const char* str, ossia_preset_t * presetptr) {
  if (presetptr == nullptr) {
    return OSSIA_PRESETS_INVALID_PTR;
  }

  *presetptr = nullptr;
  if (str != nullptr) {
    try {
      *presetptr = new ossia_preset(ossia::presets::read_json(std::string(str)));
      return OSSIA_PRESETS_OK;
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_EMPTY_JSON;
}

ossia_preset_result ossia_presets_free(ossia_preset_t preset) {
  if (preset != nullptr) {
    try {
      delete preset;
    }
    catch (...){
      return lippincott();
    }
  }
  return OSSIA_PRESETS_OK;
}

ossia_preset_result ossia_presets_write_json(const ossia_preset_t preset, const char** buffer) {
  if (preset) {
    try {
      *buffer = copy_string(ossia::presets::write_json(preset->impl));
      return OSSIA_PRESETS_OK;
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_size(const ossia_preset_t preset, int * size) {
  if (preset) {
    try {
      *size = preset->impl.size();
      return OSSIA_PRESETS_OK;
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_to_string(const ossia_preset_t preset, const char ** buffer) {
  if (preset) {
    try {
      *buffer = copy_string(ossia::presets::to_string(preset->impl));
      return OSSIA_PRESETS_OK;
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_devices_write_json(const ossia_device_t odev, const char ** buffer) {
  if (odev != nullptr) {
    try {
      assert(odev->device);
      *buffer = copy_string(ossia::devices::write_json(*(odev->device)));
      return OSSIA_PRESETS_OK;
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_apply_preset(ossia_device_t odevptr, ossia_preset_t preset, bool keep_arch) {
  if (!odevptr)  {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if (!preset) {
    return OSSIA_PRESETS_NULL_PRESET;
  }
  try {
    ossia::devices::keep_arch_type keep_arch_token;
    if (keep_arch)
      keep_arch_token = ossia::devices::keep_arch_on;
    else
      keep_arch_token = ossia::devices::keep_arch_off;

    if (!odevptr->device) {
      return OSSIA_PRESETS_NULL_DEVICE;
    }

    ossia::devices::apply_preset(*odevptr->device, preset->impl, keep_arch_token);
  }
  catch (...) {
    return lippincott();
  }
  return OSSIA_PRESETS_OK;
}

ossia_preset_result ossia_devices_make_preset(ossia_device_t odev, ossia_preset_t* presetptr) {
  if (presetptr == nullptr) {
    return OSSIA_PRESETS_INVALID_PTR;
  }

  *presetptr = nullptr;
  if (odev != nullptr) {
    try {
      assert(odev->device);
      *presetptr = new ossia_preset(ossia::devices::make_preset(*(odev->device)));
      return OSSIA_PRESETS_OK;
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_to_string(ossia_device_t odev, const char ** buffer) {
  if (odev != nullptr) {
    try {
      assert(odev->device);
      *buffer = copy_string(ossia::devices::to_string(*(odev->device)));
      return OSSIA_PRESETS_OK;
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_get_node(ossia_device_t odev, const char* addr, ossia_node_t* nodeptr) {
  if (nodeptr == nullptr) {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if (addr == nullptr) {
    return OSSIA_PRESETS_INVALID_ADDRESS;
  }
  if (odev != nullptr) {
    try {
      assert(odev->device);
      auto gotnode = ossia::devices::get_node(odev->device->getRootNode(), addr);
      if (gotnode == nullptr) {
        return OSSIA_PRESETS_INVALID_ADDRESS;
      }
      *nodeptr = convert(gotnode);
      return OSSIA_PRESETS_OK;
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_get_child(ossia_node_t root, const char * childname, ossia_node_t * nodeptr) {
  if (nodeptr == nullptr) {
    return OSSIA_PRESETS_INVALID_PTR;
  }
  if (childname == nullptr) {
    return OSSIA_PRESETS_INVALID_ADDRESS;
  }
  if (root == nullptr) {
    return OSSIA_PRESETS_NULL_NODE;
  }
  try {
    auto gotnode = ossia::devices::get_node(*convert_node(root), childname);
    if (gotnode == nullptr) {
      return OSSIA_PRESETS_INVALID_ADDRESS;
    }
    *nodeptr = convert(gotnode);
  }
  catch (...) {
    return lippincott();
  }
  return OSSIA_PRESETS_OK;
}


bool ossia_presets_has_key(
    const ossia_preset_t preset,
    const char* key)
{
  if (preset && key) {
    try {
      return preset->impl.find(key) != preset->impl.end();
    }
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_key_to_string(
    const ossia_preset_t preset,
    const char* key,
    const char** buffer)
{
  if (preset && key) {
    try {
      auto it = preset->impl.find(key);
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
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}



ossia_preset_result ossia_presets_key_to_value(
    const ossia_preset_t preset,
    const char* key,
    ossia_value_t* buffer)
{
  if (preset && key) {
    try {
      auto it = preset->impl.find(key);
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
    catch (...) {
      return lippincott();
    }
  }
  return OSSIA_PRESETS_NULL_PRESET;
}


ossia_preset_result ossia_free_string(const char * strptr) {
  delete[] strptr;
  return OSSIA_PRESETS_OK;
}

} // extern "C"

/// C++ Implementations: Presets ///

ossia::value json_to_ossia_value(const rapidjson::Value& value){

  ossia::value val;
  enum rapidjson::Type tval = value.GetType();

  switch (tval) {
    case rapidjson::kFalseType:
      val = bool(false);
      break;
    case rapidjson::kTrueType:
      val = bool(true);
      break;
    case rapidjson::kNumberType:
      if (value.IsInt()) {
        val = int32_t(value.GetInt());
      }
      else {
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

bool is_nested(const rapidjson::Value& v) {
  return (v.IsArray() || v.IsObject());
}

void explore(std::string root, const rapidjson::Value& jsonval, ossia::presets::preset* preset) {

  if (is_nested(jsonval)) {
    if (jsonval.IsObject()) {
      for (auto member = jsonval.MemberBegin(); member != jsonval.MemberEnd(); ++member) {
        explore(root + "/" + member->name.GetString(), member->value, preset);
      }
    }
    if (jsonval.IsArray()) {
      int arrsize = jsonval.Size();
      for (int i = 0; i < arrsize; ++i) {
        std::stringstream newroot;
        newroot << root << "." << i;
        explore(newroot.str(), jsonval[i], preset);
      }
    }
  }
  else {
    ossia::presets::preset_pair pp(root, json_to_ossia_value(jsonval));
    preset->insert(pp);
  }
}

ossia::presets::preset ossia::presets::read_json(const std::string &str) {
  preset prst;

  rapidjson::Document doc;
  doc.Parse(str.c_str());

  if (!doc.HasParseError()) {
    explore("", doc, &prst);
    return prst;
  }
  else {
    ossiaException_InvalidJSON exc (__LINE__, __FILE__);
    throw exc;
  }
}

rapidjson::Value ossia_to_json_value(const ossia::value& val, rapidjson::Document::AllocatorType& docallocator) {
  rapidjson::Value jsonvalue;
  ossia::val_type tvalue = val.getType();

  switch (tvalue) {
    case ossia::val_type::BOOL: {
      auto typedval = val.get<bool>();
      jsonvalue.SetBool(typedval);
    }
      break;
    case ossia::val_type::INT: {
      auto typedval = val.get<int32_t>();
      jsonvalue.SetInt(typedval);
    }
      break;
    case ossia::val_type::FLOAT: {
      auto typedval = val.get<float>();
      jsonvalue.SetDouble(typedval);
    }
      break;
    case ossia::val_type::CHAR: {
      auto typedval = val.get<char>();
      char buff[2];
      std::sprintf(buff, "%c", (char)typedval);
      buff[1] = 0;
      jsonvalue.SetString(buff, docallocator);
    }
      break;
    case ossia::val_type::STRING: {
      auto typedval = val.get<std::string>();
      jsonvalue.SetString(typedval, docallocator);
    }
      break;
    default:
      break;
  }
  return jsonvalue;
}

std::vector<std::string> name_to_arraykeys(const std::string& currentkey)
{
  std::vector<std::string> arraykeys;
  auto it = currentkey.find_last_of('.');
  if(it < currentkey.size() - 1)
  {
    try {
      auto p2 = currentkey.substr(it + 1);
      boost::lexical_cast<int>(p2);

      arraykeys.push_back(currentkey.substr(0, it));
      arraykeys.push_back(std::move(p2));
    } catch(...) {}
  }

  return arraykeys;
}

void insert(
    rapidjson::Value& recipient,
    const std::vector<std::string>& keys,
    const ossia::value& val,
    rapidjson::Document& doc);
void insert_array(
    rapidjson::Value& array,
    const std::vector<std::string>& keys,
    const ossia::value& val,
    rapidjson::Document& doc,
    std::vector<std::string>& arraykeys) {

  if (!array.IsArray()) {
    array.SetArray();
  }

  if(arraykeys.empty())
  {
    arraykeys.push_back("0");
  }

  std::size_t index = std::atoi(arraykeys[0].c_str());
  if (index >= array.Size()) {
    auto missing = index - array.Size() + 1;
    for (std::size_t i = 0; i < missing; ++i) {
      rapidjson::Value dummy;
      array.PushBack(dummy, doc.GetAllocator());
    }
  }
  if (arraykeys.size() == 1) {
    if (keys.empty()) {
      rapidjson::Value v;
      v = ossia_to_json_value(val, doc.GetAllocator());
      array[index].Swap(v);
    }
    else {
      if (array[index].IsNull()) {
        array[index].SetObject();
      }
      insert(array[index], keys, val, doc);
    }
  }
  else {
    std::vector<std::string> newarraykeys (arraykeys);
    newarraykeys.erase(newarraykeys.begin());
    insert_array(array[index], keys, val, doc, newarraykeys);
  }
}

void insert(
    rapidjson::Value& recipient,
    const std::vector<std::string>& keys,
    const ossia::value& val,
    rapidjson::Document& doc) {
  if (!keys.empty()) {

    std::string currentkey = keys[0];
    std::vector<std::string> newkeys (keys);
    newkeys.erase(newkeys.begin());

    if (recipient.IsNull()) {
      recipient.SetObject();
    }

    std::vector<std::string> arraykeys = name_to_arraykeys(currentkey);
    if (arraykeys.size() > 1)
    {
      rapidjson::Value arrname (rapidjson::kStringType);
      std::string arrnamestr (arraykeys[0]);
      arrname.SetString(arrnamestr, doc.GetAllocator());
      arraykeys.erase(arraykeys.begin());

      if (!recipient.HasMember(arrname)) {
        rapidjson::Value array (rapidjson::kArrayType);
        recipient.AddMember(arrname, array, doc.GetAllocator());
      }

      arrname.SetString(arrnamestr, doc.GetAllocator());
      insert_array(recipient[arrname], newkeys, val, doc, arraykeys);
    }
    else
    {
      if (keys.size() == 1)
      {
        rapidjson::Value v = ossia_to_json_value(val, doc.GetAllocator());
        if (!recipient.HasMember(currentkey)) {
          rapidjson::Value name (rapidjson::kStringType);
          name.SetString(currentkey, doc.GetAllocator());
          recipient.AddMember(name, v, doc.GetAllocator());
        }
        else {
          recipient[currentkey].Swap(v);
        }
      }
      else
      {
        auto it = recipient.FindMember(currentkey);
        if (it == recipient.MemberEnd())
        {
          rapidjson::Value node (rapidjson::kObjectType);
          rapidjson::Value name (rapidjson::kStringType);
          name.SetString(currentkey, doc.GetAllocator());
          recipient.AddMember(name, node, doc.GetAllocator());
          insert(recipient[currentkey], newkeys, val, doc);
        }
        else
        {
          if(it->value.IsObject())
          {
            insert(it->value, newkeys, val, doc);
          }
          else if(it->value.IsArray())
          {
            insert_array(it->value, newkeys, val, doc, arraykeys);
          }
        }
      }
    }
  }
}

std::string ossia::presets::write_json(const preset & prst) {
  rapidjson::Document d;

  d.SetObject();
  for (const auto& e : prst) {
    std::cerr << "inserting: " << e.first << "\n";
    std::vector<std::string> keys;

    boost::split(keys, e.first, [](char c){return c == '/';}, boost::token_compress_on);
    keys.erase(keys.begin()); // first element is empty

    insert((rapidjson::Value&)d, keys, e.second, d);
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  return buffer.GetString();
}


std::string ossia_value_to_std_string(const ossia::value& val) {
  std::stringstream ss;
  switch (val.getType()) {
    case ossia::val_type::BOOL :
      ss << "Bool " << val.get<bool>();
      break;
    case ossia::val_type::INT :
      ss << "Int " << val.get<int32_t>();
      break;
    case ossia::val_type::FLOAT :
      ss << "Float " << val.get<float>();
      break;
    case ossia::val_type::CHAR :
      ss << "Char " << val.get<char>();
      break;
    case ossia::val_type::STRING :
      ss << "String " << val.get<std::string>();
      break;
    default:
      ss << "Other";
  }
  return ss.str();
}

std::string ossia::presets::to_string(const preset & p)
{
  rapidjson::StringBuffer buf;
  {
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartArray();
    for (const auto& pp : p) {
      write_json_key(w, pp.first);
      ::write_json(w, ossia_value_to_std_string(pp.second));
    }
    w.EndArray();
  }
  return std::string{buf.GetString(), buf.GetLength()};
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
rapidjson::Value export_nodes_to_json(const ossia::net::node_base& node, rapidjson::Document& d)
{
  rapidjson::Value v;
  v.SetObject();
  auto& alloc = d.GetAllocator();

  auto address = node.getAddress();
  if(address)
  {
    auto default_value = ossia::net::get_default_value(node);
    switch (address->getValueType())
    {
      case ossia::val_type::IMPULSE :
      {
        v.AddMember("valueType", "impulse", alloc);
        break;
      }
      case ossia::val_type::BOOL :
      {
        v.AddMember("valueType", "boolean", alloc);

        if(default_value)
          if (auto valueDefault = default_value->target<bool>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }
        break;
      }
      case ossia::val_type::INT :
      {
        // append type attribute
        v.AddMember("valueType", "integer", alloc);

        // append default value attribute
        if(default_value)
          if (auto valueDefault = default_value->target<int>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        // append domain attribute
        auto domain = domain_to_string(address->getDomain());
        if(!domain.empty())
        {
          rapidjson::Value s;
          s.SetString(domain, alloc);
          v.AddMember("valueDomain", s, alloc);
        }

        // append step size attribute
        auto valueStepSize = ossia::get_optional_attribute<ossia::net::value_step_size>(node, "valueStepSize");
        if (valueStepSize)
        {
          v.AddMember("valueStepSize", *valueStepSize, alloc);
        }

        break;
      }
      case ossia::val_type::FLOAT :
      {
        // append type attribute
        v.AddMember("valueType", "decimal", alloc);
        // append default value attribute
        if(default_value)
          if (auto valueDefault = default_value->target<float>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        // append domain attribute
        auto domain = domain_to_string(address->getDomain());
        if(!domain.empty())
        {
          rapidjson::Value s;
          s.SetString(domain, alloc);
          v.AddMember("valueDomain", s, alloc);
        }

        // append step size attribute
        auto valueStepSize = ossia::net::get_value_step_size(node);
        if (valueStepSize)
        {
          v.AddMember("valueStepSize", *valueStepSize, alloc);
        }
        break;
      }
      case ossia::val_type::CHAR :
      {
        v.AddMember("valueType", "char", alloc);
        if(default_value)
          if (auto valueDefault = default_value->target<char>())
          {
            v.AddMember("valueDefault", *valueDefault, alloc);
          }

        break;
      }
      case ossia::val_type::STRING :
      {
        v.AddMember("valueType", "string", alloc);

        if(default_value)
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
  }

  for (const auto& child : node.children())
  {
    rapidjson::Value s;
    s.SetString(child->getName(), alloc);
    v.AddMember(s, export_nodes_to_json(*child, d), alloc);
  }

  return v;
}

std::string ossia::devices::write_json(
    const ossia::net::device_base& deviceBase)
{
  rapidjson::Document d;
  d.SetObject();
  auto& alloc = d.GetAllocator();

  auto& node = deviceBase.getRootNode();

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
  auto appVersion = ossia::get_optional_attribute<std::string>(node, "appVersion");
  if (appVersion)
  {
    rapidjson::Value tmp;
    tmp.SetString(*appVersion, alloc);
    d.AddMember("appVersion", tmp, alloc);
  }
  // append app creator attribute
  auto appCreator = ossia::get_optional_attribute<std::string>(node, "appCreator");
  if (appCreator)
  {
    rapidjson::Value tmp;
    tmp.SetString(*appCreator, alloc);
    d.AddMember("appCreator", tmp, alloc);
  }

  // parse device node tree and export to json
  d.AddMember(rapidjson::StringRef(deviceBase.getName()), export_nodes_to_json(deviceBase.getRootNode(), d), d.GetAllocator());

  // return json string
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  auto output = buffer.GetString();

  return output;
}

void ossia::devices::write_file(
    ossia::string_view content,
    ossia::string_view filename)
{
  // output json file if needed
  if (!filename.empty())
  {
    FILE* fp = std::fopen(filename.data(), "wb"); // non-Windows use "w"
    if(fp)
    {
      std::fwrite(content.data(), 1, content.size(), fp);
      std::fclose(fp);
    }
  }
}

std::string preset_to_device_key(const std::string& presetkey) {
  return presetkey;
  std::vector<std::string> indexes;
  boost::split(indexes, presetkey, [](char c){return c == '.';}, boost::token_compress_on);

  for (std::size_t i = 1; i < indexes.size(); ++i) {
    int current_index = std::atoi((indexes[i]).c_str());
    std::stringstream ss;
    ss << (current_index + 1);
    indexes[i] = std::string (ss.str());
  }

  return boost::join(indexes, ".");
}

std::string device_to_preset_key(const std::string& devicekey) {
  return devicekey;
  std::vector<std::string> indexes;
  boost::split(indexes, devicekey, [](char c){return c == '.';}, boost::token_compress_on);

  for (std::size_t i = 1; i < indexes.size(); ++i) {
    int current_index = std::atoi((indexes[i]).c_str());
    std::stringstream ss;
    ss << (current_index - 1);
    indexes[i] = std::string (ss.str());
  }

  return boost::join(indexes, ".");
}

bool instance_string_compare(const std::string& str, const ossia::net::node_base& node)
{
  auto node_name = node.getName();
  auto opt = ossia::net::sanitize_name(str, {});
  return opt == node_name;
}

void apply_preset_node(
    ossia::net::node_base& root,
    std::vector<std::string> keys,
    const ossia::value& val,
    ossia::devices::keep_arch_type keeparch,
    std::vector<ossia::net::node_base*>& created_nodes)
{
  if (keys.size() == 0) {
    if (root.children().size() > 0) {
      std::string details = "Node " + root.getName() + " is not terminal";
      throw(ossia::ossiaException_InvalidAddress (__LINE__, __FILE__, details.c_str()));
    }
    else {
      root.getAddress()->pushValue(val);
    }
  }
  else {
    // TODO this is slow. Either use the last (fast) or use a list (less slow).
    std::string currentkey = preset_to_device_key(keys[0]);
    keys.erase(keys.begin());
    bool child_exists = false;

    for (auto& child : root.children()) {
      if(!child)
      {
        auto err = std::string("Null child of : ") + root.getName();
        ossia_log_error(err.c_str());
      }
      else
      {
        if (currentkey == child->getName()) {
          child_exists = true;
          apply_preset_node(*child, keys, val, keeparch, created_nodes);
        }
      }
    }

    if (!child_exists) {
      if (keeparch == ossia::devices::keep_arch_on) {
        throw ossia::ossiaException_InvalidAddress (__LINE__, __FILE__,
                                                    "Can't change device architecture");
      }
      else
      {
        if(auto newchild = root.createChild(currentkey))
        {
          // We push them in the vector in their order of creation
          // This way when iterating we will have the parents before the children.
          created_nodes.push_back(newchild);

          // addresses only on leaf nodes... maybe we should not have this restriction.
          if (keys.empty())
            newchild->createAddress(val.getType());

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

void ossia::devices::apply_preset(
    ossia::net::device_base& ossiadev,
    const ossia::presets::preset & preset,
    ossia::devices::keep_arch_type keeparch,
    presets::instance_functions functions)
{
  std::vector<ossia::net::node_base*> created_nodes;

  for (auto itpp = preset.begin(); itpp != preset.end(); ++itpp)
  {
    std::vector<std::string> keys;
    boost::split(keys, itpp->first, [](char c){return c == '/';}, boost::token_compress_on);
    keys.erase(keys.begin()); // first subtring is empty

    apply_preset_node(ossiadev.getRootNode(), keys, itpp->second, keeparch, created_nodes);
  }

  for(auto node : created_nodes)
  {
    on_instance_creation(*node, functions);
  }
}

void make_preset_node(ossia::net::node_base& node, ossia::presets::preset& preset, const std::string& key) {
  std::string currentkey = key;
  if (node.getParent() != nullptr) {
    currentkey += "/" + device_to_preset_key(node.getName());
  }

  const auto& children = node.children();

  if (children.size() == 0) {
    if(auto addr = node.getAddress())
      preset.insert(std::make_pair(currentkey, addr->cloneValue()));
  }
  else {
    for (auto& child : children) {
      make_preset_node(*child, preset, currentkey);
    }
  }
}

ossia::presets::preset ossia::devices::make_preset(ossia::net::device_base & ossiadev) {
  presets::preset preset;
  make_preset_node(ossiadev.getRootNode(), preset, "");
  return preset;
}

ossia::net::node_base* get_node_node(ossia::net::node_base& root, std::vector<std::string>& keys) {
  ossia::net::node_base* res = nullptr;

  if (keys.size() > 0) {
    std::string currentkey = keys[0];

    if (root.getName().compare(currentkey) == 0) {
      if (keys.size() == 1) {
        res = &root;
      }
      else {
        keys.erase(keys.begin());
        for (auto& child: root.children()) {
          auto childres = get_node_node(*child, keys);
          if (childres != nullptr) {
            res = childres;
          }
        }
      }
    }
  }
  return res;
}

ossia::net::node_base* ossia::devices::get_node(
    ossia::net::node_base& root,
    const std::string & nodeaddr)
{
  std::vector<std::string> keys;
  boost::split(keys, nodeaddr, [](char c){return c == '/';});
  keys.erase(keys.begin());

  return get_node_node(root, keys);
}

void to_string_node(
    const ossia::net::node_base& root,
    std::vector<std::string>& strnodes,
    std::vector<std::string>& keys)
{
  for (auto& child: root.children())
  {
    std::vector<std::string> newkeys = keys;
    newkeys.push_back(root.getName());
    to_string_node(*child, strnodes, newkeys);
  }

  if (root.getAddress() != nullptr && root.children().size() == 0)
  {
    auto v = root.getAddress()->cloneValue();
    if (v.valid())
    {
      keys.push_back(root.getName());
      std::string nodename = boost::join(keys, "/");
      strnodes.push_back(nodename + ": " + ossia_value_to_std_string(v));
    }
  }

}

std::string ossia::devices::to_string(const ossia::net::device_base& ossiadev) {
  auto& root = ossiadev.getRootNode();
  std::stringstream ss;
  std::vector<std::string> strnodes;
  std::vector<std::string> keys;
  to_string_node(root, strnodes, keys);
  ss << "[" << boost::join(strnodes, ", ") << "]";
  return std::string(ss.str());
}

/// Exception handling ///

static ossia_preset_result lippincott() {
  try {
    try {
      throw;
    }
    catch (ossia::ossiaException_InvalidAddress b) {
      ossia_log_error(b.what());
      return OSSIA_PRESETS_INVALID_ADDRESS;
    }
    catch (ossia::ossiaException_InvalidJSON b) {
      ossia_log_error(b.what());
      return OSSIA_PRESETS_INVALID_JSON;
    }
    catch (ossia::ossiaException_InvalidXML b) {
      ossia_log_error(b.what());
      return OSSIA_PRESETS_INVALID_XML;
    }
    catch (ossia::ossiaException b) {
      ossia_log_error(b.what());
      return OSSIA_PRESETS_GENERIC_EXCEPTION;
    }
    catch (std::exception e) {
      ossia_log_error(e.what());
      return OSSIA_PRESETS_STANDARD_EXCEPTION;
    }
    catch (...) {
      return OSSIA_PRESETS_UNKNOWN_ERROR;
    }
  }
  catch (...) {
    return OSSIA_PRESETS_UNKNOWN_ERROR;
  }
}

