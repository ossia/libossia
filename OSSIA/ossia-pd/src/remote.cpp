// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "remote.hpp"
#include "device.hpp"
#include "parameter.hpp"
#include "utils.hpp"

#include <ossia/network/common/path.hpp>

namespace ossia
{
namespace pd
{

#pragma mark t_obj_pattern

t_matcher::t_matcher(ossia::net::node_base* n, t_remote* p) :
  node{n}, parent{p}, callbackit{ossia::none}
{
  std::cout << "x_matchers content (" << parent->x_matchers.size() << " items) :" << std::endl;
  for (auto& m : parent->x_matchers)
  {
    std::cout << m.node << "\t" << m.parent << std::endl;
  }
  std::cout << node << "\t" << parent << std::endl;

  callbackit = node->get_address()->add_callback(
      [=](const ossia::value& v) { set_value(v); });

  node->about_to_be_deleted.connect<t_remote, &t_remote::is_deleted>(
        parent);

  //clock_delay(x_regclock, 0);
}

t_matcher::~t_matcher()
{
  std::cout << "desctructor node: " << node << " t_remote: " << parent << std::endl;
  if(node)
  {
    auto addr = node->get_address();
    if (addr && callbackit) addr->remove_callback(*callbackit);
    node->about_to_be_deleted.disconnect<t_remote, &t_remote::is_deleted>(parent);
  }
}

void t_matcher::set_value(const ossia::value& v){
  std::cout << "set value, node: " << node << " t_remote: " << parent << std::endl;
  /*
  std::string addr = node->get_name();
  t_atom a;
  SETSYMBOL(&a, gensym(addr.c_str()));
  outlet_anything(parent->x_dumpout,gensym("address"),1,&a);

  value_visitor<t_obj_base> vm;
  vm.x = (t_obj_base*)parent;
  v.apply(vm);
  */
}

#pragma mark t_remote

static void remote_free(t_remote* x);

bool t_remote::register_node(ossia::net::node_base* node)
{
  bool res = do_registration(node);
  if (res)
  {
    obj_dequarantining<t_remote>(this);
  }
  else
    obj_quarantining<t_remote>(this);

  auto& dev = node->get_device();
  if (&dev != x_dev)
  {
    if (x_dev) x_dev->on_address_created.disconnect<t_remote, &t_remote::on_address_created_callback>(this);
    x_dev = &dev;
    x_dev->on_address_created.connect<t_remote, &t_remote::on_address_created_callback>(this);
  }

  return res;
}

bool t_remote::do_registration(ossia::net::node_base* node)
{

  if (x_node && x_node->get_parent() == node)
  {
    return true; // already registered to this node;
  }

  std::cout << "size before unregister(): " << x_matchers.size() << std::endl;
  unregister();
  std::cout << "size after unregister(): " << x_matchers.size() << std::endl;

  if (node)
  {
    std::string name = x_name->s_name;

    if ( x_is_pattern )
    {
      auto nodes = ossia::net::find_nodes(*node, name);
      for (auto n : nodes){
        if (n->get_address()){
          t_matcher matcher{n,this};
          if (ossia::find(x_matchers,matcher) == x_matchers.end())
            x_matchers.push_back(std::move(matcher));
        }
        std::cout << "size size after populate: " << x_matchers.size() << std::endl;
      }
    }
    else
    {
      if (x_absolute)
      {
        x_node = ossia::net::find_node(node->get_device().get_root_node(), name);
      }
      else
      {
        std::string absolute_path = get_absolute_path<t_remote>(this);
        std::string address_string = ossia::net::address_string_from_node(*node);
        if (absolute_path != address_string)
          return false;
        x_node = ossia::net::find_node(*node, name);

        // if there is a node without address it might be a model
        // then look if that node have an eponyme child
        if (x_node && !x_node->get_address()){
          fmt::MemoryWriter path;
          path << name << "/" << name;
          x_node = ossia::net::find_node(*node, path.str());
        }
      }

      if (x_node && x_node->get_address())
      {
        t_matcher matcher{x_node,this};
        if (ossia::find(x_matchers,matcher) == x_matchers.end())
          x_matchers.push_back(std::move(matcher));
      }
      clock_delay(x_regclock, 0);
    }
  }

  // do not put it in quarantine if it's a pattern
  // and even if it can't find any node
  return (!x_matchers.empty() || x_is_pattern);
}

bool t_remote::unregister()
{
  x_matchers.clear();

  obj_quarantining<t_remote>(this);

  x_node = nullptr;
  return true;
}


void t_remote::on_address_created_callback(const ossia::net::address_base& addr)
{
  auto& node = addr.get_node();
  auto path = ossia::traversal::make_path(x_name->s_name);

  // FIXME check for path validity
  if ( path && ossia::traversal::match(path.value(), node) )
  {
    x_matchers.emplace_back(&node,this);
  }
}

void t_remote::is_deleted(const ossia::net::node_base& n)
{
  std::cout << "node " << &n << " is deleted" << std::endl;
  if (!x_dead)
  {
    ossia::remove_one_if(
      x_matchers, 
      [&] (const auto& m) { 
        return m.node == &n;      
    }); 
  }
}

static void remote_click(
    t_remote* x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift,
    t_floatarg ctrl, t_floatarg alt)
{

  using namespace std::chrono;
  milliseconds ms
      = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  milliseconds diff = (ms - x->x_last_click);
  if (diff.count() < 200)
  {
    x->x_last_click = milliseconds(0);

    int l;

    t_device* device
        = (t_device*)find_parent(&x->x_obj, "ossia.device", 0, &l);
    /*
    if (!device || !x->x_node || obj_isQuarantined<t_remote>(x)){
      pd_error(x, "sorry no device found, or not connected or quarantined...");
      return;
    }
    */

    t_canvas* root = x->x_obj.o_canvas;
    while (root->gl_owner)
      root = root->gl_owner;
    if (!find_and_display_friend(x, root))
      pd_error(x, "sorry I can't find a connected friend :-(");
  }
  else
  {
    x->x_last_click = ms;
  }
}

static void remote_push(t_remote* x, t_symbol* s, int argc, t_atom* argv)
{
  if(x->x_node) t_obj_base::obj_push(x,s,argc,argv);
  else
  {
    for (auto& m : x->x_matchers)
    {
        x->x_node = m.node;
        t_obj_base::obj_push(x,s,argc,argv);
    }
    x->x_node = nullptr;
  }
}

static void* remote_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_remote* x = (t_remote*)eobj_new(ossia_pd.remote);

  if (x)
  {
    ossia_pd.remotes.push_back(x);

    x->x_otype = Type::remote;
    x->x_setout = outlet_new((t_object*)x, nullptr);
    x->x_dataout = outlet_new((t_object*)x, nullptr);
    x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
    new (&x->x_callbackit) decltype(x->x_callbackit);
    new (&x->x_callbackits) decltype(x->x_callbackits);
    new (&x->x_matchers) decltype(x->x_matchers);
    x->x_callbackit = ossia::none;
    x->x_dev = nullptr;

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->x_name = atom_getsymbol(argv);
      if (std::string(x->x_name->s_name) != "" && x->x_name->s_name[0] == '/')
        x->x_absolute = true;
    }
    else
    {
      error("You have to pass a name as the first argument");
      x->x_name = gensym("untitledRemote");
    }

    x->x_is_pattern = ossia::traversal::is_pattern(x->x_name->s_name);

    x->x_clock = clock_new(x, (t_method)obj_tick);
    x->x_regclock = clock_new(x, (t_method)t_obj_base::obj_bang);

    obj_register<t_remote>(x);
  }

  return (x);
}

static void remote_free(t_remote* x)
{
  x->x_dead = true;
  x->unregister();
  obj_dequarantining<t_remote>(x);
  ossia_pd::instance().remotes.remove_all(x);

  if(ossia::traversal::is_pattern(x->x_name->s_name)
     && x->x_node)
  {
    auto& dev = x->x_node->get_device();
    dev.on_address_created.disconnect<t_remote, &t_remote::on_address_created_callback>(x);
  }

  outlet_free(x->x_setout);
  outlet_free(x->x_dataout);
  outlet_free(x->x_dumpout);
  x->~t_remote();
}

extern "C" void setup_ossia0x2eremote(void)
{
  t_eclass* c = eclass_new(
      "ossia.remote", (method)remote_new, (method)remote_free,
      (short)sizeof(t_remote), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)remote_new,gensym("ø.remote"), A_GIMME, 0);

    eclass_addmethod(c, (method)remote_push, "anything", A_GIMME, 0);
    eclass_addmethod(c, (method)t_obj_base::obj_bang, "bang", A_NULL, 0);
    eclass_addmethod(c, (method)obj_dump<t_remote>, "dump", A_NULL, 0);
    eclass_addmethod(c, (method)remote_click, "click", A_NULL, 0);
  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.remote = c;
}
} // pd namespace
} // ossia namespace
