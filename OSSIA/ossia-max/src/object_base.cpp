// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/object_base.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <regex>

namespace ossia
{
namespace max
{

#pragma mark t_matcher

t_matcher::t_matcher(t_matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  parent = other.parent;
  other.parent = nullptr;

  callbackit = other.callbackit;
  other.callbackit = ossia::none;

  if(node)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { enqueue_value(v); });

      set_parent_addr();
    }
  }
}

t_matcher& t_matcher::operator=(t_matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  parent = other.parent;
  other.parent = nullptr;

  callbackit = other.callbackit;
  other.callbackit = ossia::none;

  if(node)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { enqueue_value(v); });

      set_parent_addr();
    }
  }

  return *this;
}

t_matcher::t_matcher(ossia::net::node_base* n, object_base* p) :
  node{n}, parent{p}, callbackit{ossia::none}
{
  if (auto param = node->get_parameter())
    callbackit = param->add_callback(
      [=](const ossia::value& v) { enqueue_value(v); });

  node->about_to_be_deleted.connect<object_base, &object_base::is_deleted>(
        parent);

  set_parent_addr();

  //clock_delay(x_regclock, 0);
}

t_matcher::~t_matcher()
{
  std::cout << "destructor ===============>" << this << std::endl;
  if(node && parent)
  {
    if (parent->m_otype == object_class::param)
    {
      if (!parent->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<object_base, &object_base::is_deleted>(parent);

        for (auto remote : ossia_max::instance().remotes.copy())
        {
          ossia::remove_one(remote->m_matchers,*this);
        }

        if (node->get_parent())
          node->get_parent()->remove_child(*node);
      }
      // if there vector is empty
      // remote should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        object_quarantining<parameter>((parameter*) parent);
      }
    } else {

      if (!parent->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<object_base, &object_base::is_deleted>(parent);
      }

      // if there vector is empty
      // remote should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        object_quarantining<remote>((remote*) parent);
      }
    }
    node = nullptr;
    parent = nullptr;
  }
}

void t_matcher::enqueue_value(const ossia::value& v)
{
  m_queue_list.enqueue(v);
}

void t_matcher::output_value()
{
  ossia::value v;
  while(m_queue_list.try_dequeue(v)) {

    outlet_anything(parent->m_dumpout,gensym("address"),1,&m_addr);

    auto param = node->get_parameter();

    auto filtered = ossia::net::filter_value(
          param->get_domain(),
          v,
          param->get_bounding());

    ossia::value converted;
    if ( parent->m_ounit != ossia::none )
      converted = ossia::convert(filtered, param->get_unit(), *parent->m_ounit);
    else
      converted = filtered;

    value_visitor<object_base> vm;
    vm.x = (object_base*)parent;
    converted.apply(vm);
  }
}

void t_matcher::set_parent_addr()
{
  if (parent->m_parent_node){
    std::string addr = ossia::net::address_string_from_node(*node);
    std::string parent_addr = ossia::net::address_string_from_node(*parent->m_parent_node);
    if ( parent_addr.back() != '/' ) parent_addr += "/";

    std::regex addr_regex(parent_addr);
    std::smatch addr_match;

    if (std::regex_search(addr, addr_match, addr_regex))
    {
      A_SETSYM(&m_addr, gensym(addr_match.suffix().str().c_str()));
    } else {
      A_SETSYM(&m_addr, gensym(node->get_name().c_str()));
    }
  }
  else
  {
    A_SETSYM(&m_addr, gensym("."));
  }
}

void object_base::is_deleted(const ossia::net::node_base& n)
{
    m_is_deleted= true;
    ossia::remove_one_if(
      m_matchers,
      [&] (const auto& m) {
        return m.get_node() == &n;
    });
    m_is_deleted = false;
}

void object_base::set_priority()
{
  for (auto n : m_nodes)
    ossia::net::set_priority(*n, m_priority);
}

void object_base::set_description()
{
  for (auto n : m_nodes)
    ossia::net::set_description(*n, m_description->s_name);
}

void object_base::set_tags()
{
  std::vector<std::string> tags;
  for (int i = 0; i < m_tags_size; i++)
    tags.push_back(m_tags[i]->s_name);

  for (auto n : m_nodes)
    ossia::net::set_tags(*n, tags);
}

void object_base::defer_set_output(object_base*x, t_symbol*s ,int argc, t_atom* argv){
  outlet_anything(x->m_set_out, s, argc, argv);
}

} // max namespace
} // ossia namespace
