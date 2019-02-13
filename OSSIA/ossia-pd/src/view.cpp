// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-pd/src/view.hpp>
#include <ossia-pd/src/utils.hpp>
#include <ossia-pd/src/ossia-pd.hpp>

namespace ossia
{
namespace pd
{

view::view():
  node_base{ossia_pd::view_class}
{ }

//****************//
// Member methods //
//****************//
bool view::register_node(const std::vector<t_matcher>& matchers)
{
  bool res = do_registration(matchers);
  if (res)
  {
    obj_dequarantining<view>(this);
    std::vector<object_base*> viewnode
        = find_child_to_register(this, m_obj.o_canvas->gl_list, ossia_pd::o_sym_view);
    for (auto v : viewnode)
    {
      if (v->m_otype == object_class::view)
      {
        ossia::pd::view* view = (ossia::pd::view*)v;

        // not registering itself
        if (view == this)
          continue;

        // register only object with a relative scope
        // other subscribed to on_parameter_created callback
        if (view->m_addr_scope == ossia::net::address_scope::relative)
          view->register_node(m_matchers);
      }
      else if (v->m_otype == object_class::remote)
      {
        ossia::pd::remote* remote = (ossia::pd::remote*)v;

        if (remote->m_addr_scope == ossia::net::address_scope::relative )
          remote->register_node(m_matchers);
      }
    }
  }
  else
    obj_quarantining<view>(this);

  return res;
}

bool view::do_registration(const std::vector<t_matcher>& matchers)
{

  // we should unregister here because we may have add a node
  // between the registered node and the remote
  unregister();

  if(!m_name)
    return false;

  std::string name = m_name->s_name;

  for (auto& m : matchers)
  {
    auto _node = m.get_node();

    if (m_addr_scope == ossia::net::address_scope::absolute)
    {
      // get root node
      _node = &_node->get_device().get_root_node();
      // and remove starting '/'
      name = name.substr(1);
    }

    m_parent_node = _node;

    std::vector<ossia::net::node_base*> nodes{};

    if (m_addr_scope == ossia::net::address_scope::global)
      nodes = ossia::pd::find_global_nodes(name);
    else
      nodes = ossia::net::find_nodes(*_node, name);

    m_matchers.reserve(m_matchers.size() + nodes.size());

    for (auto n : nodes){
      // we may have found a node with the same name
      // but with a parameter, in that case it's an ø.param
      // then forget it
      if (!n->get_parameter()){
        m_matchers.emplace_back(n, this);
      }
    }
  }

  fill_selection();

  return (!m_matchers.empty() || m_is_pattern);
}

static void register_children(view* x)
{
  std::vector<object_base*> viewnode
      = find_child_to_register(x, x->m_obj.o_canvas->gl_list, ossia_pd::o_sym_view);
  for (auto v : viewnode)
  {
    assert(!v->m_dead);
    if (v->m_otype == object_class::view)
    {
      ossia::pd::view* view = (ossia::pd::view*)v;
      if (view == x)
        continue;
      ossia_register<ossia::pd::view>(view);
    }
    else if (v->m_otype == object_class::remote)
    {
      ossia::pd::remote* remote = (ossia::pd::remote*)v;
      ossia_register<ossia::pd::remote>(remote);
    }
  }
}

bool view::unregister()
{
  m_matchers.clear();

  std::vector<object_base*> viewnode
      = find_child_to_register(this, m_obj.o_canvas->gl_list, ossia_pd::o_sym_view);
  for (auto v : viewnode)
  {
    if (v->m_otype == object_class::view)
    {
      ossia::pd::view* view = (ossia::pd::view*)v;
      if (view == this)
        continue;
      view->unregister();
    }
    else if (v->m_otype == object_class::remote)
    {
      ossia::pd::remote* remote = (ossia::pd::remote*)v;
      remote->unregister();
    }
  }
  obj_quarantining<view>(this);

  register_children(this);

  return true;
}

void view::click(
    view* x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift,
    t_floatarg ctrl, t_floatarg alt)
{

  using namespace std::chrono;
  milliseconds ms
      = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  milliseconds diff = (ms - x->m_last_click);
  if (diff.count() < 200)
  { // 200 ms double click
    x->m_last_click = milliseconds(0);

    int l;
    ossia::pd::device* device
        = find_parent<ossia::pd::device>(x, 0, &l);

    if (!ossia::pd::object_base::find_and_display_friend(x))
      pd_error(x, "sorry I can't find a connected friend :-(");
  }
  else
  {
    x->m_last_click = ms;
  }
}

void* view::create(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  ossia::pd::view* x = new ossia::pd::view();

  if (x)
  {
    ossia_pd.views.push_back(x);

    x->m_otype = object_class::view;
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      t_symbol* address = atom_getsymbol(argv);
      std::string name = replace_brackets(address->s_name);
      x->m_name = gensym(name.c_str());
      x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
    }
    else
      x->m_name = nullptr;

    if (find_peer(x))
    {
      error(
          "Only one [ø.model]/[ø.view] intance per patcher is allowed.");
      view::destroy(x);
      free(x);
      x = nullptr;
    }

#ifdef OSSIA_PD_BENCHMARK
    std::cout << measure<>::execution(obj_register<view>, x) / 1000. << " ms "
              << " " << x << " view " << x->m_name->s_name
              << " " << x->m_reg_count << std::endl;
#else
    ossia_check_and_register(x);
#endif
  }

  return x;
}

void view::destroy(view* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<view>(x);
  ossia_pd::instance().views.remove_all(x);
  outlet_free(x->m_dumpout);
  x->m_dumpout = nullptr;

  x->~view();
}

extern "C" void setup_ossia0x2eview(void)
{
  t_eclass* c = eclass_new(
      "ossia.view", (method)view::create, (method)view::destroy, (short)sizeof(view),
      CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)view::create,gensym("ø.view"), A_GIMME, 0);

    node_base::class_setup(c);

    eclass_addmethod(c, (method) view::click, "click", A_NULL,   0);
    eclass_addmethod(c, (method) address_mess_cb<view>, "address",   A_SYMBOL, 0);
    eclass_addmethod(c, (method) view::get_mess_cb, "get",   A_SYMBOL, 0);
    eclass_addmethod(c, (method) view::notify,    "notify",   A_NULL,  0);

    eclass_register(CLASS_OBJ, c);
  }

  ossia_pd::view_class = c;
}

ossia::safe_set<view*>& view::quarantine()
{
    return ossia_pd::instance().view_quarantine;
}

} // pd namespace
} // ossia namespace
