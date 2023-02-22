#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/hash_map.hpp>
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/preset/preset.hpp>

#include <nano_observer.hpp>

#include <optional>

namespace ossia
{
namespace net
{
class node_base;
class device_base;
}
struct cue
{
  std::string name{};
  ossia::presets::preset preset;
};

struct selection_filters
{
  std::vector<std::string> selection;
  std::vector<ossia::val_type> type;
  std::vector<ossia::access_mode> access;
  std::vector<ossia::bounding_mode> bounding;
  std::vector<std::string> tags;

  enum visibility_t
  {
    visible,
    invisible
  };
  std::optional<visibility_t> visibility;
};

class OSSIA_EXPORT cues : Nano::Observer
{
public:
  std::vector<cue> m_cues{{.name{"Init"}}};

  void set_device(ossia::net::device_base* dev);

  int size() const noexcept { return this->m_cues.size(); }
  cue& current_cue() noexcept { return this->m_cues[m_current]; }
  cue* get_cue(int idx) noexcept
  {
    return (idx >= 0 && idx < std::ssize(this->m_cues)) ? &this->m_cues[idx] : nullptr;
  }
  std::optional<int> find_cue(std::string_view name);
  int get_cue(std::string_view name);

  void create(std::string_view name);

  void update(std::string_view name);
  void update(int idx); // update cue by index
  void update();        // update current cue

  void recall(std::string_view name);
  void recall(int idx);
  void recall();

  void remove(std::string_view name);
  void remove(int idx);
  void remove();
  void clear();

  void move(std::string_view name, int index);
  void move(int from, int to);

  void output(std::string_view name);
  void output(std::string_view name, std::string_view pattern);

  void namespace_select(std::string_view pat);
  void namespace_deselect(std::string_view pat);
  void namespace_filter_all(const selection_filters& pat);
  void namespace_filter_any(const selection_filters& pat);
  void namespace_grab(std::string_view pat);

  void on_node_created(const ossia::net::node_base& n);
  void on_node_removed(const ossia::net::node_base& n);

  //private:
  ossia::net::device_base* dev{};
  int m_current{0};

  ossia::hash_set<ossia::net::node_base*> m_selection;
};
}
