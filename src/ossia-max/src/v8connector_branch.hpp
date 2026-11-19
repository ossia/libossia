// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>

#pragma once

// Max-independent part of ossia.v8connector: a branch of an ossia device
// whose leaves (parameters) are declared at runtime by a script.
//
// The branch does not own its node. The host gives it a node factory; the
// branch asks for a node when its first leaf is declared, as an ossia.model
// creates its node, and keeps it after clear, as an ossia.model without
// children. A branch that only binds never asks for a node, as an ossia.remote
// creates nothing. The host detaches the branch before the node disappears;
// the declarations are kept, so that the next node recreates every leaf.
//
// A branch can also share a node it does not own (an ossia.model's node, the
// root of a device): its leaves then sit next to the other parameters of that
// node. A leaf whose address is taken is refused, never renamed; the branch
// removes only the nodes it created, and removes its leaves when it detaches.
//
// A branch can also follow parameters that exist elsewhere (bind), the way
// ossia.remote does: nothing is created, the subscription follows the
// parameter when it disappears and comes back, and set pushes to it.

#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/value.hpp>

#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace ossia
{
namespace max_binding
{
namespace v8connector_detail
{

// A value as it arrives from the host: integer, float or symbol.
using scalar = std::variant<int, float, std::string>;
using scalars = std::vector<scalar>;

// One "@name values..." group of a declaration.
struct attribute
{
  std::string name;
  scalars values;
};

// Validated declaration of a leaf.
struct leaf_spec
{
  std::string address; // relative to the branch, "a" or "a/b", no leading slash
  std::string type;    // float int bool impulse string list vec2f vec3f vec4f
  std::vector<float> min, max;  // one value per component, numeric types only
  std::vector<std::string> enumeration; // range of a string parameter
  std::string unit;
  std::string description;
  std::vector<std::string> tags;
  std::optional<scalars> default_value;
  std::string clip{"free"};
  std::string mode{"bi"};
  std::optional<float> priority;
  std::optional<float> rate;
  bool critical{false};
  bool invisible{false};
  bool mute{false};
  bool repetitions{true};
  bool enable{true};
  bool recall_safe{false};

  bool operator==(const leaf_spec&) const = default;
};

// A value received for a leaf, to be output by the host.
struct inbound
{
  std::string address; // relative to the branch
  ossia::value value;
};

// Normalize a relative address: trims, removes leading and trailing slashes,
// refuses empty components. Returns an empty string on refusal.
std::string normalize_relative_address(std::string_view address);

// Build a leaf declaration from an address and its attributes, using the
// attribute names of ossia.parameter. Returns an empty string on success,
// otherwise the reason of the refusal. Numeric types require min and max
// (or range).
std::string make_leaf_spec(
    std::string_view address, const std::vector<attribute>& attrs, leaf_spec& out);

// Convert host scalars to a value of the parameter's type. Returns an empty
// string on success, otherwise the reason.
std::string
scalars_to_value(const ossia::net::parameter_base& param, const scalars& in, ossia::value& out);

class branch
{
public:
  branch();
  ~branch();
  branch(const branch&) = delete;
  branch& operator=(const branch&) = delete;

  // How the branch obtains its node when it needs one. The factory returns
  // nullptr when no node can be created yet (not registered, no device).
  void set_node_factory(std::function<ossia::net::node_base*()> factory);
  // Ask the factory for a node if the branch has leaves and no node yet.
  void reattach();
  // Attach to a node of the device tree and recreate every declared leaf.
  void attach(ossia::net::node_base& node);
  // Forget the node and the leaves it holds. The declarations are kept. A
  // branch that shares its node removes its leaves from it first.
  void detach();
  // false: the node given by the factory belongs to someone else (see above).
  // Set before the first attach.
  void set_owns_node(bool owns) { m_owns_node = owns; }
  bool attached() const { return m_node != nullptr; }
  ossia::net::node_base* node() const { return m_node; }

  // Declare a leaf. Declaring the same leaf again with the same attributes is
  // accepted and does nothing, so that a script can replay its declarations.
  // Refused if the address is declared with other attributes, bound, or if the
  // branch is attached and the address already holds a parameter.
  std::string add(const leaf_spec& spec);
  // Validate the attributes of an add message and declare the leaf. On
  // refusal, the reason is returned and queued as a refusal (see refuse).
  std::string declare(std::string_view address, const std::vector<attribute>& attrs);
  // Remove a leaf. Returns false if the address is not declared.
  bool remove(std::string_view address);
  // Queue a refusal: drain returns it as the address "refused" with the
  // refused address and the reason as value, on the host's main thread, like
  // any value. add, declare, set, bind, remove and unbind call it themselves
  // when they refuse; the host calls it for the refusals it detects first.
  void refuse(std::string_view address, std::string_view reason);
  // Remove every leaf of this branch.
  void clear();
  // Push a value from the host, to a declared leaf or to a bound address. The
  // parameter stores what is sent, as ossia.parameter does; values are bounded
  // to the parameter's range when they leave the branch, then filtered by the
  // parameter (disabled, muted, repetition), as ossia-max does at the outlet of
  // each object. The value comes back to the host only when the bounded value
  // differs from what was sent (clip, type conversion).
  std::string set(std::string_view address, const scalars& values);

  // Bundles: when on, set stores the value and queues the parameter instead
  // of sending it; flush sends what was queued since the last flush, in OSC
  // bundles, one group per protocol: a client's OSCQuery mirror splits them
  // itself under 1400 bytes over UDP, other protocols (an OSCQuery server
  // under a multiplex) get groups of at most 1400 bytes of messages. A value
  // equal to the parameter's current one is not queued when the parameter's
  // repetition filter is on. Off (the default here), set sends at once.
  void set_bundle(bool on);
  bool bundle() const { return m_bundle; }
  // Main thread, from the host's deferred queue. Returns the number of
  // parameters sent.
  std::size_t flush();

  // Follow a parameter that exists elsewhere. The address follows the rules of
  // ossia.remote: relative to the parents given by set_resolution_context,
  // absolute ("/a/b") in the parents' device, or global ("device:/a/b");
  // patterns are accepted. Binding an address twice is accepted and does
  // nothing. Refused if the address is empty or declared as a leaf of this
  // branch. A bind to a parameter that does not exist yet is kept and resolved
  // when the parameter appears.
  std::string bind(std::string_view address);
  // Drop a subscription. Returns false if the address is not bound.
  bool unbind(std::string_view address);
  // Number of parameters currently followed by a bound address.
  std::size_t bound_count(std::string_view address) const;

  // Where relative and absolute bound addresses are resolved: the nodes an
  // ossia.remote placed next to the host would register under, and the devices
  // known to the host. Resolves the pending bindings, and listens to parameter
  // creation on these devices to resolve them later.
  void set_resolution_context(
      std::vector<ossia::net::node_base*> parents,
      std::vector<ossia::net::device_base*> devices);
  // Forget a device before it is destroyed. A device destroyed without this
  // call is forgotten anyway: the branch listens to the destruction of the
  // root of every device given by set_resolution_context.
  void drop_device(ossia::net::device_base* dev);
  // Called when the number of parameters followed by a bound address changes.
  void set_binding_changed_callback(
      std::function<void(const std::string& address, std::size_t count)> cb);

  std::vector<std::string> addresses() const;
  std::size_t size() const { return m_leaves.size(); }

  // Called from the thread that pushed a value, possibly not the host's main
  // thread. It must only schedule a drain on the main thread.
  void set_wake_callback(std::function<void()> cb);
  // Main thread only: take every value received since the last call. The
  // default of a leaf comes out here once the leaf is created, and the current
  // value of a parameter comes out here when a bind reaches it, as an
  // ossia.remote outputs the value it finds when it registers.
  std::vector<inbound> drain();

  // Called when the node the branch is attached to is destroyed by its owner
  // (for instance when the enclosing model registers again). The branch is
  // detached when it fires; the host should attach it to a new node later.
  void set_node_lost_callback(std::function<void()> cb);

private:
  struct leaf
  {
    leaf_spec spec;
    ossia::net::node_base* node{};
    ossia::net::parameter_base* param{};
    // Shared node only: the topmost node this leaf created, nullptr if the
    // leaf took a node that existed.
    ossia::net::node_base* created{};
  };

  struct target
  {
    ossia::net::node_base* node{};
    ossia::net::parameter_base* param{};
    ossia::net::parameter_base::callback_index callback{};
    std::string out_address;
  };

  struct binding
  {
    std::string address;
    std::vector<target> targets;
  };

  std::string set_impl(std::string_view address, const scalars& values);
  leaf* find(std::string_view address);
  binding* find_binding(std::string_view address);
  void ensure_node();
  std::string create(leaf& l);
  void destroy(leaf& l);
  void resolve(binding& b);
  void resolve_bindings();
  void release(binding& b);
  void on_value(
      const std::string& address, ossia::net::parameter_base* param, const ossia::value& v);
  void queue_current_value(const std::string& address, ossia::net::parameter_base* param);
  void on_node_removing(const ossia::net::node_base& n);
  void watch_device(ossia::net::device_base& dev);
  void unwatch_device(ossia::net::device_base& dev, bool root_alive);
  void forget_device(ossia::net::device_base* dev, bool root_alive);
  void on_parameter_created(const ossia::net::parameter_base& p);

  ossia::net::node_base* m_node{};
  std::vector<leaf> m_leaves;
  std::vector<binding> m_bindings;
  std::vector<ossia::net::node_base*> m_parents;

  // Index of each address in m_leaves and m_bindings: a script sets
  // thousands of addresses per tick.
  struct address_hash
  {
    using is_transparent = void;
    std::size_t operator()(std::string_view s) const noexcept
    {
      return std::hash<std::string_view>{}(s);
    }
  };
  using address_index = std::unordered_map<std::string, std::size_t, address_hash, std::equal_to<>>;
  address_index m_leaf_index;
  address_index m_binding_index;
  // Bindings by the last component of their address, and the bindings with
  // a pattern: a parameter created can only satisfy those.
  std::unordered_map<std::string, std::vector<std::size_t>> m_binding_by_name;
  std::vector<std::size_t> m_pattern_bindings;
  void index_binding(std::size_t i);
  void reindex_leaves();
  void reindex_bindings();
  std::vector<ossia::net::device_base*> m_parent_devices; // device of each parent
  std::vector<ossia::net::device_base*> m_devices;
  std::function<void(const std::string&, std::size_t)> m_binding_changed;

  // A queued value, or a parameter whose current value is read when the host
  // drains: the value of a parameter being created is only final once its
  // declaration has completed.
  struct queued
  {
    std::string address;
    ossia::value value;
    ossia::net::parameter_base* read_from{};
  };

  std::mutex m_queue_mutex;
  std::vector<queued> m_queue;
  std::function<void()> m_wake;
  std::function<void()> m_node_lost;
  std::function<ossia::net::node_base*()> m_node_factory;
  bool m_owns_node{true};
  std::string m_attach_refusal; // reason of the last leaf refused by attach

  // Parameters set since the last flush, each once, in the order of their
  // first set; the value sent is the one they hold at flush.
  bool m_bundle{false};
  std::vector<ossia::net::parameter_base*> m_outgoing;
  std::unordered_set<ossia::net::parameter_base*> m_outgoing_set;
  bool push(ossia::net::parameter_base& param, ossia::value&& v);
};

}
}
}
