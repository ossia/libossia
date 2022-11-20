#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/preset/preset.hpp>

#include <nano_observer.hpp>

#include <unordered_set>
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

class OSSIA_EXPORT cues : Nano::Observer
{
public:
  void set_device(ossia::net::device_base* dev);

  int size() const noexcept { return cues.size(); }
  cue& current_cue() noexcept { return cues[m_current]; }
  cue* get_cue(int idx) noexcept
  {
    return (idx >= 0 && idx < std::ssize(cues)) ? &cues[idx] : nullptr;
  }
  std::optional<int> find_cue(std::string_view name);
  std::vector<cue> cues{{.name{"Init"}}};

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
  void namespace_grab(std::string_view pat);

  void on_node_created(const ossia::net::node_base& n);
  void on_node_removed(const ossia::net::node_base& n);

  //private:
  ossia::net::device_base* dev{};
  int get_cue(std::string_view name);
  int m_current{0};

  struct selector
  {
    enum
    {
      Add,
      Remove
    } mode{Add};
    std::string pattern{"/"};
  };
  std::vector<selector> m_selectors{selector{}};
  std::unordered_set<ossia::net::node_base*> m_selection;
};
}
