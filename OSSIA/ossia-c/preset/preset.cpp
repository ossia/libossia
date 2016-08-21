#include <memory>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/allocators.h>

#include <ossia-c/log/ossia_log.h>
#include <ossia-c/preset/preset.h>
#include <ossia-c/preset/preset.hpp>
#include <ossia-c/preset/exception.hpp>
#include <ossia-c/preset/result.h>

#include <ossia/ossia.hpp>
#include <ossia-c/ossia/ossia_utils.hpp>

struct ossia_preset
{
    ossia::presets::Preset impl;
    ossia_preset(): impl(ossia::presets::Preset ()) {}
    ossia_preset(const ossia::presets::Preset& prst) : impl(prst){}
};

/// C functions ///

extern "C" {
static ossia_preset_result lippincott();

ossia_preset_result ossia_presets_read_json(const char* str, ossia_preset_t * presetptr) {
    if (presetptr == nullptr) {
        return OSSIA_PRESETS_INVALID_PTR;
    }
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

ossia_preset_result ossia_presets_read_xml(const char * str, ossia_preset_t * presetptr) {
    if (presetptr == nullptr) {
        return OSSIA_PRESETS_INVALID_PTR;
    }
    if (str != nullptr) {
        try {
            *presetptr = new ossia_preset(ossia::presets::read_xml(std::string(str)));
            return OSSIA_PRESETS_OK;
        }
        catch (...) {
            return lippincott();
        }
    }
    return OSSIA_PRESETS_EMPTY_XML;
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
    if (preset != nullptr) {
        try {
            std::string str = ossia::presets::write_json(preset->impl);
            char * mbuffer = new char[str.size() + 1];
            std::sprintf(mbuffer, "%s", str.c_str());
            mbuffer[str.size()] = 0;
            *buffer = (const char*)mbuffer;
            return OSSIA_PRESETS_OK;
        }
        catch (...) {
            return lippincott();
        }
    }
    return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_write_xml(const ossia_preset_t preset, const char ** buffer) {
    if (preset != nullptr) {
        try {
            std::string str = ossia::presets::write_xml(preset->impl);
            char * mbuffer = new char[str.size() + 1];
            std::sprintf(mbuffer, "%s", str.c_str());
            mbuffer[str.size()] = 0;
            *buffer = (const char*)mbuffer;
            return OSSIA_PRESETS_OK;
        }
        catch (...) {
            return lippincott();
        }
    }
    return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_presets_size(const ossia_preset_t preset, int * size) {
    if (preset != nullptr) {
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
    if (preset != nullptr) {
        try {
            std::string str = ossia::presets::to_string(preset->impl);
            char * mbuffer = new char[str.size() + 1];
            std::sprintf(mbuffer, "%s", str.c_str());
            mbuffer[str.size()] = 0;
            *buffer = (const char*)mbuffer;
            return OSSIA_PRESETS_OK;
        }
        catch (...) {
            return lippincott();
        }
    }
    return OSSIA_PRESETS_NULL_PRESET;
}

ossia_preset_result ossia_devices_read_json(ossia_device_t* odevptr, const char * str) {
    if (odevptr == nullptr) {
        return OSSIA_PRESETS_INVALID_PTR;
    }
    if (str != nullptr) {
        try {
            ossia::devices::read_json(*(*odevptr)->device , str);
            return OSSIA_PRESETS_OK;
        }
        catch (...) {
            return lippincott();
        }
    }
    return OSSIA_PRESETS_EMPTY_JSON;
}

ossia_preset_result ossia_devices_read_xml(ossia_device_t* odevptr, const char * str) {
    if (str == nullptr) {
        return OSSIA_PRESETS_EMPTY_XML;
    }
    if (odevptr != nullptr) {
        try {
            ossia::devices::read_xml(*(*odevptr)->device, str);
            return OSSIA_PRESETS_OK;
        }
        catch (...) {
            return lippincott();
        }
    }
    return OSSIA_PRESETS_INVALID_PTR;
}

ossia_preset_result ossia_devices_write_json(const ossia_device_t odev, const char ** buffer) {
    if (odev != nullptr) {
        try {
            std::string str = ossia::devices::write_json(*(odev->device));
            char * mbuffer = new char[str.size() + 1];
            std::sprintf(mbuffer, "%s", str.c_str());
            mbuffer[str.size()] = 0;
            *buffer = (const char*)mbuffer;
            return OSSIA_PRESETS_OK;
        }
        catch (...) {
            return lippincott();
        }
    }
    return OSSIA_PRESETS_NULL_DEVICE;
}

ossia_preset_result ossia_devices_write_xml(const ossia_device_t odev, const char ** buffer) {
    if (odev != nullptr) {
        try {
            std::string str = ossia::devices::write_xml(*(odev->device));
            char * mbuffer = new char[str.size() + 1];
            std::sprintf(mbuffer, "%s", str.c_str());
            mbuffer[str.size()] = 0;
            *buffer = (const char*)mbuffer;
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
    if (odev != nullptr) {
        try {
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
            std::string str = ossia::devices::to_string(*(odev->device));
            char * mbuffer = new char[str.size() + 1];
            std::sprintf(mbuffer, "%s", str.c_str());
            mbuffer[str.size()] = 0;
            *buffer = (const char*)mbuffer;
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

ossia_preset_result ossia_free_string(const char * strptr) {
    if (strptr != nullptr) {
        try {
            delete strptr;
        }
        catch (...) {
            return lippincott();
        }
    }
    return OSSIA_PRESETS_OK;
}

} // extern "C"

/// C++ Implementations: Presets ///

ossia::value json_to_ossia_value(const rapidjson::Value& value){

    ossia::value val;
    enum rapidjson::Type tval = value.GetType();

    switch (tval) {
    case rapidjson::kFalseType:
        val = ossia::Bool(false);
        break;
    case rapidjson::kTrueType:
        val = ossia::Bool(true);
        break;
    case rapidjson::kNumberType:
        if (value.IsInt()) {
            val = ossia::Int(value.GetInt());
        }
        else {
            val = ossia::Float(value.GetDouble());
        }
        break;
    case rapidjson::kStringType:
        val = ossia::String(value.GetString());
        break;
    default:
        break;
    }

    return val;
}

bool is_nested(const rapidjson::Value& v) {
    return (v.IsArray() || v.IsObject());
}

void explore(std::string root, const rapidjson::Value& jsonval, ossia::presets::Preset* preset) {

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
        ossia::presets::PresetPair pp(root, json_to_ossia_value(jsonval));
        preset->insert(pp);
    }
}

ossia::presets::Preset ossia::presets::read_json(const std::string &str) {
    Preset prst;

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

ossia::presets::Preset ossia::presets::read_xml(const std::string &str) {
    // TODO
  return {};
}

rapidjson::Value ossia_to_json_value(const ossia::value& val, rapidjson::Document::AllocatorType& docallocator) {
    rapidjson::Value jsonvalue;
    ossia::val_type tvalue = val.getType();

    switch (tvalue) {
    case ossia::val_type::BOOL: {
        auto typedval = val.get<ossia::Bool>();
        jsonvalue.SetBool(typedval.value);
    }
        break;
    case ossia::val_type::INT: {
        auto typedval = val.get<ossia::Int>();
        jsonvalue.SetInt(typedval.value);
    }
        break;
    case ossia::val_type::FLOAT: {
        auto typedval = val.get<ossia::Float>();
        jsonvalue.SetDouble(typedval.value);
    }
        break;
    case ossia::val_type::CHAR: {
        auto typedval = val.get<ossia::Char>();
        char buff[2];
        std::sprintf(buff, "%c", typedval.value);
        jsonvalue.SetString(buff, docallocator);
    }
        break;
    case ossia::val_type::STRING: {
        auto typedval = val.get<ossia::String>();
        jsonvalue.SetString(typedval.value, docallocator);
    }
        break;
    default:
        break;
    }
    return jsonvalue;
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
    const std::vector<std::string>& arraykeys) {

    if (!array.IsArray()) {
        array.SetArray();
    }

    if (!arraykeys.empty()) {
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

        std::vector<std::string> arraykeys;
        boost::split(arraykeys, currentkey, [](char c){return c == '.';}, boost::token_compress_on);
        if (arraykeys.size() > 1) {

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

        else {
            if (keys.size() == 1) {
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

            else {
                if (!recipient.HasMember(currentkey)) {
                    rapidjson::Value node (rapidjson::kObjectType);
                    rapidjson::Value name (rapidjson::kStringType);
                    name.SetString(currentkey, doc.GetAllocator());
                    recipient.AddMember(name, node, doc.GetAllocator());
                }
                insert(recipient[currentkey], newkeys, val, doc);
            }
        }
    }
}

std::string ossia::presets::write_json(const Preset & prst) {
    rapidjson::Document d;

    d.SetObject();

    for (auto it = prst.begin(); it != prst.end(); ++it) {
        std::vector<std::string> keys;
        boost::split(keys, it->first, [](char c){return c == '/';}, boost::token_compress_on);
        keys.erase(keys.begin()); // first element is empty
        auto& val = it->second;
        insert((rapidjson::Value&)d, keys, val, d);
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    return buffer.GetString();
}

std::string ossia::presets::write_xml(const Preset &) {
    // Not implemented
    return {};
}

std::string ossia::presets::to_string(const Preset & preset) {
    std::string str = "[";
    std::vector<std::string> substrings;
    for (PresetPair pp : preset) {
        substrings.push_back(to_string(pp));
    }
    str += boost::join(substrings, ", ");
    str += "]";
    return str;
}

std::string ossia_value_to_std_string(const ossia::value& val) {
    std::stringstream ss;
    switch (val.getType()) {
    case ossia::val_type::BOOL :
        ss << "Bool " << val.get<ossia::Bool>().value;
        break;
    case ossia::val_type::INT :
        ss << "Int " << val.get<ossia::Int>().value;
        break;
    case ossia::val_type::FLOAT :
        ss << "Float " << val.get<ossia::Float>().value;
        break;
    case ossia::val_type::CHAR :
        ss << "Char " << val.get<ossia::Char>().value;
        break;
    case ossia::val_type::STRING :
        ss << "String " << val.get<ossia::String>().value;
        break;
    default:
        ss << "Other";
    }
    return ss.str();
}

std::string ossia::presets::to_string(const PresetPair & pp) {
    std::stringstream ss;
    ss << pp.first << ": ";
    auto& val = pp.second;
    ss << ossia_value_to_std_string(val);
    return std::string(ss.str());
}

/// C++ Implementation: Devices ///

void ossia::devices::read_json(ossia::net::device_base &odev, const std::string &str) {
    // Not implemented
}

void ossia::devices::read_xml(ossia::net::device_base &, const std::string &) {

}

std::string ossia::devices::write_json(const ossia::net::device_base &) {
    // Not implemented
    return "";
}

std::string ossia::devices::write_xml(const ossia::net::device_base &) {
    std::string xml;
    return xml;
}

std::string preset_to_device_key(const std::string& presetkey) {
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

void apply_preset_node(
    ossia::net::node_base& root,
    std::vector<std::string> keys,
    const ossia::value& val,
    ossia::devices::keep_arch_type keeparch)
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
            if (child->getName().compare(currentkey) == 0) {
              child_exists = true;
              apply_preset_node(*child, keys, val, keeparch);
            }
          }
        }

        if (!child_exists) {
            if (keeparch == ossia::devices::keep_arch_on) {
                throw ossia::ossiaException_InvalidAddress (__LINE__, __FILE__, "Can't change device architecture");
            }
            else {
                auto newchild = root.createChild(currentkey);
                if (keys.size() == 0) {
                    newchild->createAddress(val.getType());
                }
                apply_preset_node(*newchild, keys, val, keeparch);
            }
        }
    }
}

void ossia::devices::apply_preset(
    ossia::net::device_base& ossiadev,
    const ossia::presets::Preset & preset,
    ossia::devices::keep_arch_type keeparch) {
    for (auto itpp = preset.begin(); itpp != preset.end(); ++itpp) {
        std::vector<std::string> keys;
        boost::split(keys, itpp->first, [](char c){return c == '/';}, boost::token_compress_on);
        keys.erase(keys.begin()); // first subtring is empty

        apply_preset_node(ossiadev.getRootNode(), keys, itpp->second, keeparch);
    }
}

void make_preset_node(ossia::net::node_base& node, ossia::presets::Preset& preset, const std::string& key) {
    std::string currentkey = key;
    if (node.getParent() != nullptr) {
        currentkey += "/" + device_to_preset_key(node.getName());
    }

    auto& children = node.children();

    if (children.size() == 0) {
        preset.insert(std::make_pair(currentkey, node.getAddress()->cloneValue()));
    }
    else {
        for (auto& child : children) {
            make_preset_node(*child, preset, currentkey);
        }
    }
}

ossia::presets::Preset ossia::devices::make_preset(ossia::net::device_base & ossiadev) {
    presets::Preset preset;
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

