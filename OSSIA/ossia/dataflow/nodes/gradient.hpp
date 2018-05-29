#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/editor/curve/curve_segment/easing.hpp>
#include <ossia/network/dataspace/color.hpp>
#include <boost/container/flat_map.hpp>

namespace ossia::nodes
{
class gradient final : public ossia::graph_node
{
  public:
    using grad_type = boost::container::flat_map<double, ossia::hunter_lab>;

    gradient()
    {
      ossia::outlet_ptr vp = ossia::make_outlet<ossia::value_port>();
      vp->data.target<ossia::value_port>()->type = ossia::argb_u{};
      m_outlets.push_back(std::move(vp));
    }

    void set_gradient(grad_type t)
    {
      m_data = std::move(t);
    }

    void handle_before_first(double position)
    {
      auto& out = *m_outlets[0]->data.target<ossia::value_port>();
      auto beg = m_data.begin();
      if(beg->first >= position)
      {
        out.add_raw_value(ossia::argb{beg->second}.dataspace_value);
      }
      else if(!mustTween)
      {
        out.add_raw_value(ossia::argb{beg->second}.dataspace_value);
      }
      else
      {
        if(!tween)
        {
          auto addr = m_outlets[0]->address.target<ossia::net::parameter_base*>();
          if(addr && *addr)
          {
            // TODO if the curve is in another unit, we have to convert it to the correct unit.
            tween = ossia::argb{ossia::convert<ossia::vec4f>((*addr)->value())};
          }
          else
          {
            tween = ossia::argb{beg->second};
          }
        }
        out.add_raw_value(ease_color(0., *tween, beg->first, beg->second, position).dataspace_value);
      }
    }

    void run(ossia::token_request t, ossia::execution_state& e) noexcept override
    {
      auto& out = *m_outlets[0]->data.target<ossia::value_port>();

      switch (m_data.size())
      {
        case 0:
          out.add_raw_value(ossia::vec4f{0., 0., 0., 0.});
          return;
        case 1:
          handle_before_first(t.position);
          return;
        default:
        {
          auto it_next = m_data.lower_bound(t.position);
          // Before start
          if (it_next == m_data.begin())
          {
            handle_before_first(t.position);
            return;
          }
          // past end
          if (it_next == m_data.end())
          {
            out.add_raw_value(ossia::argb{m_data.rbegin()->second}.dataspace_value);
            return;
          }

          auto it_prev = it_next;
          --it_prev;

          out.add_raw_value(
                ease_color(it_prev->first, it_prev->second,
                           it_next->first, it_next->second, t.position).dataspace_value);
          return;
        }
      }
    }

    ossia::argb ease_color(double prev_pos, ossia::hunter_lab prev, double next_pos, ossia::hunter_lab next, double pos)
    {
      // Interpolate in La*b* domain
      const auto coeff
          = (pos - prev_pos) / (next_pos - prev_pos);

      ossia::hunter_lab res;
      ossia::easing::ease e{};
      res.dataspace_value = ossia::make_vec(
                              e(prev.dataspace_value[0], next.dataspace_value[0], coeff),
          e(prev.dataspace_value[1], next.dataspace_value[1], coeff),
          e(prev.dataspace_value[2], next.dataspace_value[2], coeff));

      return res;
    }
  public:
    optional<ossia::argb> tween;
  private:
    grad_type m_data;
  public:
    bool mustTween{};
};

class gradient_process final : public ossia::node_process
{
public:
    using ossia::node_process::node_process;
  void start() override
  {
      static_cast<gradient*>(node.get())->tween = ossia::none;
  }
};
}
