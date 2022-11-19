#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/preset/preset.hpp>

#include <nano_observer.hpp>

#include <unordered_set>
namespace ossia
{
namespace net {
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

  int size() const noexcept { return m_cues.size(); }
  cue& current_cue() noexcept { return m_cues[m_current]; }

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

  void on_node_removed(const ossia::net::node_base& n);

  //private:
  ossia::net::device_base* dev{};
  int get_cue(std::string_view name);
  std::optional<int> find_cue(std::string_view name);
  int m_current{0};
  std::vector<std::string> selection{"/"};
  std::unordered_set<ossia::net::node_base*> m_selection;
  std::vector<cue> m_cues{{.name{"Init"}}};
};
}
