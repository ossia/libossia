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

class OSSIA_EXPORT namespace_selection : Nano::Observer
{
  public:
    void set_device(ossia::net::device_base* dev);

    void namespace_select(std::string_view pat);
    void namespace_deselect(std::string_view pat);
    void namespace_switch(std::string_view pat);
    void namespace_filter_all(const selection_filters& pat);
    void namespace_filter_any(const selection_filters& pat);
    void namespace_grab(std::string_view pat);

    void on_node_created(const ossia::net::node_base& n);
    void on_node_removed(const ossia::net::node_base& n);


    ossia::net::device_base* dev{};
    ossia::hash_set<ossia::net::node_base*> m_selection;
};

class OSSIA_EXPORT cues
{
public:
  std::vector<cue> m_cues{{.name{"Init"}, .preset = {}}};

  int size() const noexcept { return this->m_cues.size(); }

  int current_index() const noexcept { return m_current; }
  cue* current_cue() noexcept {
    if(has_cue(m_current))
      return &this->m_cues[m_current];
    else
      return nullptr;
  }
  cue* get_cue(int idx) noexcept
  {
    return (idx >= 0 && idx < std::ssize(this->m_cues)) ? &this->m_cues[idx] : nullptr;
  }
  std::optional<int> find_cue(std::string_view name);
  int get_cue(std::string_view name);

  void create(std::string_view name);

  void update(ossia::net::node_base& root, const namespace_selection& sel, std::string_view name);
  void update(ossia::net::node_base& root, const namespace_selection& sel, int idx); // update cue by index
  void update(ossia::net::node_base& root, const namespace_selection& sel);        // update current cue

  void recall(ossia::net::node_base& root, namespace_selection& sel, std::string_view name);
  void recall(ossia::net::node_base& root, namespace_selection& sel, int idx);
  void recall(ossia::net::node_base& root, namespace_selection& sel);

  void remove(std::string_view name);
  void remove(int idx);
  void remove();
  void clear();

  void rename(std::string_view oldname, std::string_view newname);
  void rename(int idx, std::string_view newname);
  void rename(std::string_view newname);

  void move(std::string_view name, int index);
  void move(int from, int to);

  void output(std::string_view name);
  void output(std::string_view name, std::string_view pattern);

  bool has_cue(int cue) const noexcept;
  //private:
  int m_current{0};
};
}
