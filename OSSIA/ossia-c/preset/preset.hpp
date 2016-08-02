#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <preset/exception.hpp>

namespace OSSIA
{
    class Device;
    class Value;
    class Node;
    class Address;
}

namespace ossia
{
    namespace presets
    {
        using Preset = std::unordered_map<std::string, OSSIA::Value*>;
        using PresetPair = std::pair<std::string, OSSIA::Value*>;

        Preset read_json(const std::string&);
        Preset read_xml(const std::string&);

        std::string write_json(const Preset&);
        std::string write_xml(const Preset&);

        std::string to_string(const Preset&);
        std::string to_string(const PresetPair&);
    }

    namespace devices
    {

    enum keep_arch_type {keep_arch_on, keep_arch_off};

      void read_json(OSSIA::Device&, const std::string&);
      void read_xml(OSSIA::Device&, const std::string&);

      std::string write_json(const OSSIA::Device&);
      std::string write_xml(const OSSIA::Device&);

      void apply_preset(std::shared_ptr<OSSIA::Device>, const presets::Preset&, keep_arch_type t = keep_arch_on);
      presets::Preset make_preset(OSSIA::Device&);

      std::shared_ptr<OSSIA::Node> get_node(const std::shared_ptr<OSSIA::Node>, const std::string&);
      std::string to_string(OSSIA::Device&);
      std::shared_ptr<OSSIA::Address> get_node_address(const std::shared_ptr<OSSIA::Node>);
    }
}
