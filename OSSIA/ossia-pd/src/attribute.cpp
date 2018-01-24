// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-pd/src/attribute.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/utils.hpp>

namespace ossia
{
namespace pd
{

attribute::attribute():
  parameter_base{ossia_pd::attribute_class}
{ }

bool attribute::register_node(const std::vector<t_matcher>& matchers)
{
  if (m_mute) return false;

  update_path();

  bool res = do_registration(matchers);
  if (res)
  {
    obj_dequarantining<attribute>(this);
  }
  else
    obj_quarantining<attribute>(this);

  if (!matchers.empty() && m_is_pattern){
    // assume all nodes refer to the same device
    auto& dev = matchers[0].get_node()->get_device();
    if (&dev != m_dev)
    {
      if (m_dev) {
          m_dev->on_parameter_created.disconnect<attribute, &attribute::on_parameter_created_callback>(this);
          m_dev->get_root_node().about_to_be_deleted.disconnect<attribute, &attribute::on_device_deleted>(this);
      }
      m_dev = &dev;
      m_dev->on_parameter_created.connect<attribute, &attribute::on_parameter_created_callback>(this);
      m_dev->get_root_node().about_to_be_deleted.connect<attribute, &attribute::on_device_deleted>(this);
    }
  }

  return res;
}

bool attribute::do_registration(const std::vector<t_matcher>& matchers)
{
  unregister();

  std::string name = m_name->s_name;

  for (auto& m : matchers)
  {
    auto node = m.get_node();

    if (m_addr_scope == net::address_scope::absolute)
    {
      // get root node
      node = &node->get_device().get_root_node();
      // and remove starting '/'
      name = name.substr(1);
    }

    m_parent_node = node;

    std::vector<ossia::net::node_base*> nodes{};

    if (m_addr_scope == net::address_scope::global)
      nodes = ossia::pd::find_global_nodes(name);
    else
      nodes = ossia::net::find_nodes(*node, name);

    m_matchers.reserve(m_matchers.size() + nodes.size());

    for (auto n : nodes){
      if (n->get_parameter()){
        m_matchers.emplace_back(n,this);
      } else {
        // if there is a node without address it might be a model
        // then look if that node have an eponyme child
        fmt::MemoryWriter path;
        fmt::BasicStringRef<char> name_fmt(name.data(), name.size());
        path << name_fmt << "/" << name_fmt;
        auto node = ossia::net::find_node(*n, path.str());
        if (node){
          m_matchers.emplace_back(node, this);
        }
      }
    }
  }

  fill_selection();

  // do not put it in quarantine if it's a pattern
  // and even if it can't find any matching node
  return (!m_matchers.empty() || m_is_pattern);
}

bool attribute::unregister()
{

  m_matchers.clear();

  obj_quarantining<attribute>(this);

  m_parent_node = nullptr;
  if(m_dev)
  {
    m_dev->on_parameter_created.disconnect<attribute, &attribute::on_parameter_created_callback>(this);
    m_dev->get_root_node().about_to_be_deleted.disconnect<attribute, &attribute::on_device_deleted>(this);
  }
  m_dev = nullptr;
  return true;
}

void attribute::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();

  if ( m_path && ossia::traversal::match(*m_path, node) )
  {
    m_parent_node = node.get_parent();
    m_matchers.emplace_back(&node,this);

    // TODO optimize : don't clear selection and iterate through all matchers
    // just add it if it matches instead
    fill_selection();
  }
}

void attribute::on_device_deleted(const net::node_base &)
{
  m_dev = nullptr;
}

t_pd_err attribute::notify(attribute*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
    if (msg == gensym("attr_modified"))
    {
      if( s == gensym("range") )
        x->set_range();
      else if ( s == gensym("clip") )
        x->set_bounding_mode();
      else if ( s == gensym("min") || s == gensym("max") )
        x->set_minmax();
      else if ( s == gensym("default") )
        x->set_default();
      else if ( s == gensym("unit") )
        x->set_unit();
      else if ( s == gensym("hidden") )
        x->set_hidden();
      else if ( s == gensym("priority") )
        x->set_priority();
      else if ( s == gensym("mode") )
        x->set_access_mode();
      else if ( s == gensym("repetitions") )
        x->set_repetition_filter();
      else if ( s == gensym("tags") )
        x->set_tags();
      else if ( s == gensym("description") )
        x->set_description();
      else if ( s == gensym("enable") )
        x->set_enable();
      else if ( s == gensym("type") )
        x->set_type();
      else if ( s == gensym("rate") )
        x->set_rate();
      else if ( s == gensym("mute") )
        x->set_mute();
  }
  return {};
}

void attribute::click(
    attribute* x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift,
    t_floatarg ctrl, t_floatarg alt)
{

  using namespace std::chrono;
  milliseconds ms
      = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  milliseconds diff = (ms - x->m_last_click);
  if (diff.count() < 200)
  {
    x->m_last_click = milliseconds(0);

    int l;

    ossia::pd::device* device = find_parent<ossia::pd::device>(x, 0, &l);

    if (!object_base::find_and_display_friend(x))
      pd_error(x, "sorry I can't find a connected friend :-(");
  }
  else
  {
    x->m_last_click = ms;
  }
}

void* attribute::create(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  attribute* x = new attribute();

  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    x->m_otype = object_class::attribute;
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      t_symbol* address = atom_getsymbol(argv);
      std::string name = replace_brackets(address->s_name);
      x->m_name = gensym(name.c_str());
      x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
    }
    else
    {
      error("You have to pass a name as the first argument");
      x->m_name = gensym("untitled_attribute");
    }

    ebox_attrprocess_viabinbuf(x, d);

    obj_register<attribute>(x);
    ossia_pd.attributes.push_back(x);
  }

  return (x);
}

void attribute::destroy(attribute* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<attribute>(x);
  ossia_pd::instance().attributes.remove_all(x);

  if(x->m_is_pattern && x->m_dev)
  {
    x->m_dev->on_parameter_created.disconnect<attribute, &attribute::on_parameter_created_callback>(x);
    x->m_dev->get_root_node().about_to_be_deleted.disconnect<attribute, &attribute::on_device_deleted>(x);
  }

  outlet_free(x->m_dumpout);

  x->~attribute();
}

extern "C" void setup_ossia0x2eattribute(void)
{
  t_eclass* c = eclass_new("ossia.attribute",
      (method)ossia::pd::attribute::create,
      (method)ossia::pd::attribute::destroy,
      (short)sizeof(attribute), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)attribute::create,gensym("ø.attribute"), A_GIMME, 0);
    class_addcreator((t_newmethod)attribute::create,gensym("ossia.attr"), A_GIMME, 0);
    class_addcreator((t_newmethod)attribute::create,gensym("ø.attr"), A_GIMME, 0);

    parameter_base::class_setup(c);

    eclass_addmethod(c, (method) attribute::click,           "click",       A_NULL,   0);
    eclass_addmethod(c, (method) attribute::notify,          "notify",      A_NULL,   0);
    eclass_addmethod(c, (method) parameter_base::get_mess_cb, "get", A_SYMBOL, 0);
    eclass_addmethod(c, (method) address_mess_cb<attribute>, "address",   A_SYMBOL, 0);
  }

  eclass_register(CLASS_OBJ, c);
  ossia_pd::attribute_class = c;
}

ossia::safe_set<attribute*>& attribute::quarantine()
{
  return ossia_pd::instance().attribute_quarantine;
}

} // pd namespace
} // ossia namespace
