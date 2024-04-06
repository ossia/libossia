#include <ossia/network/common/value_bounding.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/value/value_conversion.hpp>
#include <ossia/preset/preset.hpp>

#include <ossia-max/src/matcher.hpp>
#include <ossia-max/src/object_base.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <regex>

namespace ossia
{
namespace max_binding
{
namespace {
  struct logger_file
  {
    FILE* f{};

    logger_file()
    {
      buf.reserve(1024);
      using namespace std;
      time_t rawtime{};
      struct tm * timeinfo{};

      time(&rawtime);
      timeinfo = localtime (&rawtime);
      std::string time_str(64, '\0');
      strftime(time_str.data(), 64, "%X", timeinfo);
      time_str.resize(strlen(time_str.data()));

      f = fopen(("/tmp/"s + time_str + ".txt"s).data(), "w");
    }

    ~logger_file()
    {
      fclose(f);
    }

    void log(ossia::net::parameter_base* p, const ossia::value& v)
    {
      std::lock_guard l{mut};
      buf.clear();
      buf += p->get_node().osc_address();
      buf += ossia::value_to_pretty_string(v);
      buf += '\n';
      fwrite(buf.data(), 1, buf.size(), f);
    }

    std::mutex mut;
    std::string buf;

  };
  static logger_file logfile;
}
matcher::matcher(ossia::net::node_base* n, object_base* p)
    : node{n}
    , owner{p}
    , callbackit{std::nullopt}
{
  if(owner)
  {
    switch(owner->m_otype)
    {
      case object_class::remote:
      case object_class::param:
        orig_param = node->get_parameter();
        if(orig_param)
        {
          callbackit = orig_param->add_callback(
              [=](const ossia::value& v) { 
                //logfile.log(orig_param, v);
                output_value(v); 
          });

          auto& dev = n->get_device();
          dev.on_parameter_removing.connect<&object_base::on_parameter_removing>(owner);
        }
        break;
      default:
        break;
    }

    node->about_to_be_deleted.connect<&object_base::on_node_removing>(owner);
  }

  set_addr_symbol();

  auto& omax = ossia_max::instance();
  std::lock_guard _{omax.s_node_matchers_mut};
  omax.s_node_matchers_map[node].push_back(this);
}

matcher::~matcher()
{
  auto& omax = ossia_max::instance();
  std::lock_guard _{omax.s_node_matchers_mut};

  auto& map = omax.s_node_matchers_map;
  {
    auto it = map.find(node);
    if(it != map.end())
    {
      it->second.remove_all(this);
      if(it->second.empty())
        map.erase(it);
    }
  }

  if(owner)
  {
    // purge selection
    ossia::remove_one(owner->m_node_selection, this);

    auto& dev = node->get_device();
    if(orig_param)
      dev.on_parameter_removing.disconnect<&object_base::on_parameter_removing>(owner);
    node->about_to_be_deleted.disconnect<&object_base::on_node_removing>(owner);
    remove_callback();

    if(owner->m_otype == object_class::param || owner->m_otype == object_class::model)
    {
      if(!owner->m_is_deleted)
      {
        if(owner->m_otype == object_class::param)
        {
          node->remove_parameter();
        }

        auto parent = node->get_parent();
        if(owner->m_otype == object_class::model)
        {
          parent->remove_child(node->get_name());
        }
        else
        {
          auto _node = node;
          while(parent)
          {
            if(_node->children().size() > 0)
              break;

            parent->remove_child(_node->get_name());

            if(parent->get_parameter())
              break;

            if(map.find(parent) != map.end())
              break;

            _node = parent;
            parent = parent->get_parent();
          }
        }
      }
    }
    owner = nullptr;
  }
  node = nullptr;
}
struct set_local_mute
{
  bool& m;
  set_local_mute(bool& mute)
      : m{mute}
  {
    m = true;
  }
  ~set_local_mute() { m = false; }
};
void matcher::output_value(ossia::value v)
{
  if(owner->m_otype == object_class::param || owner->m_otype == object_class::remote)
  {
    auto pbase = static_cast<parameter_base*>(owner);
    if(pbase->m_set_flag)
      return;
  }

  if(!owner->m_local_mute)
  {
    set_local_mute lm{owner->m_local_mute};

    auto param = node->get_parameter();
    auto filtered
        = ossia::bound_value(param->get_domain(), std::move(v), param->get_bounding());

    // FIXME filter_value return false but an invalid filtered value
    if(!param->filter_value(filtered) && filtered.valid())
    {
      auto x = (parameter_base*)owner;

      ossia::value val;
      if(x->m_local_unit == std::nullopt)
      {
        val = std::move(filtered);
      }
      else
      {
        val = ossia::convert(std::move(filtered), param->get_unit(), *x->m_local_unit);
      }

      if(owner->m_dumpout)
      {
        t_atom a[2];
        a[0] = m_addr;
        A_SETLONG(a + 1, m_index);
        outlet_anything(owner->m_dumpout, gensym("address"), 2, a);
      }

      value_visitor<object_base> vm;
      vm.x = (object_base*)owner;
      val.apply(vm);
    }
  }
}

void matcher::remove_callback()
{
  auto param = node->get_parameter();
  if(param && callbackit)
  {
    auto cb = std::move(callbackit);
    callbackit.reset();
    param->remove_callback(*cb);
  }
}

void matcher::set_addr_symbol()
{
  if(!m_dead && node && owner)
  {
    std::string addr = ossia::net::address_string_from_node(*node);

    if(owner->m_trim_addr > 0)
    {
      switch(owner->m_addr_scope)
      {
        case ossia::net::address_scope::relative: {
          auto parent = owner->find_parent_object();
          if(parent)
          {
            for(const auto& m : parent->m_matchers)
            {
              std::string node_addr
                  = ossia::net::address_string_from_node(*m->get_node());

              size_t offset = node_addr.back() == '/' ? 0 : 1;

              if(addr.rfind(node_addr, 0) == 0)
              {
                addr = addr.substr(node_addr.size() + offset);
                break;
              }
            }
          }
          else
          {
            auto pos = addr.find(":");
            addr = addr.substr(pos + 2);
          }
          break;
        }
        case ossia::net::address_scope::absolute: {
          auto pos = addr.find(":");
          addr = addr.substr(pos + 1);
          break;
        }
        default:;
      }
    }

    A_SETSYM(&m_addr, gensym(addr.c_str()));
  }
}

} // namespace max
} // namespace ossia
