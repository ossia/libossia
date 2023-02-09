// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/common/path.hpp>

#include <ossia-max/src/monitor.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <boost/algorithm/string/predicate.hpp>

using namespace ossia::max_binding;

#pragma mark -
#pragma mark ossia_monitor class methods

t_symbol* monitor::s_monitor = gensym("monitor");

t_symbol* monitor::s_parameter = gensym("parameter");
t_symbol* monitor::s_node = gensym("node");
t_symbol* monitor::s_created = gensym("created");
t_symbol* monitor::s_removing = gensym("removing");
t_symbol* monitor::s_renamed = gensym("renamed");

extern "C" void ossia_monitor_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.monitor class
  ossia_library.ossia_monitor_class = class_new(
      "ossia.monitor", (method)ossia_monitor_new, (method)monitor::free,
      (long)sizeof(monitor), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_monitor_class;
  class_addmethod(c, (method)monitor::execute_method, "monitor", A_GIMME, 0);
  class_addmethod(c, (method)monitor::assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)monitor::notify, "notify", A_CANT, 0);

  search_filter::setup_attribute<monitor>(c);

  CLASS_ATTR_LONG(c, "depth", 0, monitor, m_depth);
  CLASS_ATTR_LABEL(c, "depth", 0, "Limit exploration depth");
  CLASS_ATTR_FILTER_MIN(c, "depth", 0);

  class_register(CLASS_BOX, ossia_library.ossia_monitor_class);
}

extern "C" void* ossia_monitor_new(t_symbol*, long argc, t_atom* argv)
{
  auto x = make_ossia<monitor>();
  x->m_dumpout = outlet_new(x, NULL);
  x->m_otype = object_class::monitor;

  object_attach_byptr_register(x, x, CLASS_BOX);

  if(argc > 1 && argv[0].a_type == A_SYM && argv[1].a_type == A_SYM)
  {
    x->parse_args(argv[0].a_w.w_sym, argc - 1, argv + 1);
    // need to schedule a loadbang because objects only receive a loadbang when patcher
    // loads.
    defer_low(x, (method)object_base::loadbang, nullptr, 0, nullptr);
  }
  return x;
}

void monitor::free(monitor* x)
{
  if(x)
  {
    x->~monitor();
  }
}

t_max_err
monitor::notify(monitor* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  return 0;
}

void monitor::assist(monitor* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}

#pragma mark -
#pragma mark t_monitor structure functions

monitor::monitor() = default;

monitor::~monitor()
{
  stop_monitoring();
  outlet_delete(m_dumpout);
}

void monitor::stop_monitoring()
{
  for(const auto& dev : m_devices)
  {
    dev->on_node_created.disconnect<&monitor::on_node_created_callback>(this);
    dev->on_node_removing.disconnect<&monitor::on_node_removing_callback>(this);
    dev->on_node_renamed.disconnect<&monitor::on_node_renamed_callback>(this);
    dev->on_parameter_created.disconnect<&monitor::on_parameter_created_callback>(this);
    dev->on_parameter_removing.disconnect<&monitor::on_parameter_removing_callback>(
        this);
    dev->get_root_node().about_to_be_deleted.disconnect<&monitor::on_device_deleted>(
        this);
  }
  m_devices.clear();
}

void monitor::parse_args(t_symbol* s, long argc, t_atom* argv)
{
  stop_monitoring();
  m_method = s;
  m_name = _sym_nothing;
  if(argc > 0 && argv->a_type == A_SYM)
  {
    m_name = argv->a_w.w_sym;
    m_addr_scope = ossia::net::get_address_scope(m_name->s_name);
  }
}

void monitor::execute_method(monitor* x, t_symbol* s, long argc, t_atom* argv)
{
  x->parse_args(s, argc, argv);
  auto matchers = x->find_or_create_matchers();

  for(const auto& n : matchers)
  {
    x->m_devices.insert(&n->get_node()->get_device());
  }

  for(const auto& dev : x->m_devices)
  {
    // TODO register other signals according to attributes' values
    dev->on_node_created.connect<&monitor::on_node_created_callback>(x);
    dev->on_node_removing.connect<&monitor::on_node_removing_callback>(x);
    dev->on_node_renamed.connect<&monitor::on_node_renamed_callback>(x);
    dev->on_parameter_created.connect<&monitor::on_parameter_created_callback>(x);
    dev->on_parameter_removing.connect<&monitor::on_parameter_removing_callback>(x);
    dev->get_root_node().about_to_be_deleted.connect<&monitor::on_device_deleted>(x);
  }
}

bool monitor::unregister()
{
  m_node_selection.clear();
  m_matchers.clear();

  stop_monitoring();
  return true;
}

void monitor::on_node_created_callback(const ossia::net::node_base& node)
{
  handle_modification(node, s_node, s_created);
}

void monitor::on_node_removing_callback(const ossia::net::node_base& node)
{
  handle_modification(node, s_node, s_created);
}

void monitor::on_node_renamed_callback(
    const ossia::net::node_base& node, const std::string&)
{
  handle_modification(node, s_node, s_created);
}

void monitor::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  handle_modification(param.get_node(), s_parameter, s_created);
}

void monitor::on_parameter_removing_callback(const ossia::net::parameter_base& param)
{
  handle_modification(param.get_node(), s_parameter, s_removing);
}

void monitor::handle_modification(
    const ossia::net::node_base& node, t_symbol* type, t_symbol* action)
{
  for(auto& p : m_paths)
  {
    auto path = ossia::traversal::make_path(p);
    {
      if(path && ossia::traversal::match(*path, node))
      {
        t_atom a[3];
        A_SETSYM(a, s_parameter);
        A_SETSYM(a + 1, s_created);
        std::string address = ossia::net::osc_parameter_string_with_device(node);
        A_SETSYM(a + 2, gensym(address.c_str()));
        outlet_anything(m_dumpout, s_monitor, 3, a);
      }
      else
      {
        std::string address = ossia::net::osc_parameter_string_with_device(node);
        if(address == p)
        {
          t_atom a[3];
          A_SETSYM(a, s_parameter);
          A_SETSYM(a + 1, s_created);
          A_SETSYM(a + 2, gensym(address.c_str()));
          outlet_anything(m_dumpout, s_monitor, 3, a);
        }
      }
    }
  }
}

void monitor::on_device_deleted(const ossia::net::node_base& node)
{
  auto it = m_devices.find(&node.get_device());
  if(it != m_devices.end())
  {
    m_devices.erase(it);
  }
}
