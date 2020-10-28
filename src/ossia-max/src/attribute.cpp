// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/attribute.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <ossia/network/common/path.hpp>

using namespace ossia::max;

extern "C" void ossia_attribute_setup()
{
  auto c = class_new( "ossia.attribute",
      (method)attribute::create,
      (method)attribute::destroy,
      (long)sizeof(attribute), 0L,
      A_GIMME, 0);

  parameter_base::class_setup(c);

  class_addmethod(
        c, (method)attribute::assist,
        "assist", A_CANT, 0);
  class_addmethod(
        c, (method)attribute::notify,
        "notify", A_CANT, 0);

  class_addmethod(c, (method) parameter_base::get_mess_cb, "get",  A_SYM, 0);
  class_addmethod(c, (method) address_mess_cb<attribute>, "address",   A_SYM, 0);


  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_attribute_class = c;
}

namespace ossia
{
namespace max
{


void attribute::assist(attribute* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Remote parameter attribute messages");
  }
  else
  {
    switch(a)
    {
      case 0:
        sprintf(s, "Remote parameter attribute value");
        break;
      default:
        ;
    }
  }
}

t_max_err attribute::notify(attribute *x, t_symbol *s,
                            t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  if (!x->m_lock && msg == gensym("attr_modified")) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if ( attrname == gensym("unit") )
      x->set_unit();
    else if ( attrname == gensym("type") )
      x->set_type();
    else
      parameter_base::notify(x, s, msg, sender, data);
  }
  return 0;
}

bool attribute::register_node(const std::vector<std::shared_ptr<matcher>>& node)
{
  if (m_mute) return false;

  update_path();

  bool res = do_registration(node);
  if (res)
  {
    ossia_max::instance().nr_attributes.remove_all(this);
  }
  else
  {
    ossia_max::instance().nr_attributes.push_back(this);
  }

  if (!node.empty() && m_is_pattern){

    // assume all nodes refer to the same device
    auto& dev = node[0]->get_node()->get_device();
    if (&dev != m_dev)
    {
      if (m_dev) {
          m_dev->on_parameter_created.disconnect<&attribute::on_parameter_created_callback>(this);
          m_dev->get_root_node().about_to_be_deleted.disconnect<&attribute::on_device_deleted>(this);
      }
      m_dev = &dev;
      m_dev->on_parameter_created.connect<&attribute::on_parameter_created_callback>(this);
      m_dev->get_root_node().about_to_be_deleted.connect<&attribute::on_device_deleted>(this);

      ossia_max::instance().nr_attributes.remove_all(this);
    }
  }

  return res;
}

bool attribute::do_registration(const std::vector<std::shared_ptr<matcher>>& matchers)
{
  m_registered = true;

  std::string name = m_name->s_name;

  for (auto& m : matchers)
  {
    auto node = m->get_node();
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
      nodes = ossia::max::find_global_nodes(name);
    else
      nodes = ossia::net::find_nodes(*node, name);

    m_matchers.reserve(m_matchers.size() + nodes.size());

    for (auto n : nodes){
      if (n->get_parameter()){
        m_matchers.emplace_back(std::make_shared<matcher>(n,this));
      } else {
        // if there is a node without address it might be a model
        // then look if that node have an eponyme child
        auto node = ossia::net::find_node(*n, fmt::format("{}/{}", name, name));
        if (node){
          m_matchers.emplace_back(std::make_shared<matcher>(node, this));
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

  ossia_max::instance().nr_attributes.push_back(this);

  m_parent_node = nullptr;
  if(m_dev)
  {
    m_dev->on_parameter_created.disconnect<&attribute::on_parameter_created_callback>(this);
    m_dev->get_root_node().about_to_be_deleted.disconnect<&attribute::on_device_deleted>(this);
  }
  m_dev = nullptr;
  return true;
}

void attribute::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();

  if ( m_path )
  {
    if( ossia::traversal::match(*m_path, node) )
    {
      if(m_addr_scope == net::address_scope::relative)
      {
        m_parent_node = node.get_parent();
        std::string name(m_name->s_name);
        size_t pos = name.find('/', 0);
        while(pos != std::string::npos)
        {
          m_parent_node = node.get_parent();
          pos = name.find('/',pos+1);
        }
      }
      m_matchers.emplace_back(std::make_shared<matcher>(&node,this));
      fill_selection();
    }
  }
}

void attribute::on_device_deleted(const net::node_base &)
{
  m_dev = nullptr;
}

void* attribute::create(t_symbol* name, int argc, t_atom* argv)
{
  auto x = make_ossia<attribute>();

  if (x)
  {
    auto patcher = x->m_patcher;
    ossia_max::instance().patchers[patcher].attributes.push_back(x);

    x->m_otype = object_class::attribute;
    x->m_dumpout = outlet_new(x, NULL);

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    // Register object to istself so it can receive notification when attribute changed
    // This is not documented anywhere, please look at :
    // https://cycling74.com/forums/notify-when-attribute-changes
    object_attach_byptr_register(x, x, CLASS_BOX);

    // need to schedule a loadbang because objects only receive a loadbang when patcher loads.
    x->m_reg_clock = clock_new(x, (method) object_base::loadbang);
    clock_set(x->m_reg_clock, 1);
  }

  return (x);
}

void attribute::destroy(attribute* x)
{
  x->m_dead = true;
  x->unregister();
  ossia_max::instance().attributes.remove_all(x);

  if(x->m_is_pattern && x->m_dev)
  {
    x->m_dev->on_parameter_created.disconnect<&attribute::on_parameter_created_callback>(x);
    x->m_dev->get_root_node().about_to_be_deleted.disconnect<&attribute::on_device_deleted>(x);
  }

  outlet_delete(x->m_dumpout);
  x->~attribute();
}

} // pd namespace
} // ossia namespace
