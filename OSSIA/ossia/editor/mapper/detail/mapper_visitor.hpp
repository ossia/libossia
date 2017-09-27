#pragma once
#include <ossia/editor/curve/behavior.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/curve/curve_abstract.hpp>
#include <ossia/misc_visitors.hpp>
#include <ossia/detail/apply.hpp>

namespace ossia
{
namespace detail
{
struct mapper_compute_visitor
{
  ossia::value
  operator()(float driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->get_type();

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<float, float>*>(base_curve);
        return float{c->value_at(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<float, int>*>(base_curve);
        return int32_t{c->value_at(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<float, bool>*>(base_curve);
        return bool{c->value_at(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value
  operator()(int32_t driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->get_type();

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<int, float>*>(base_curve);
        return float{c->value_at(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<int, int>*>(base_curve);
        return int32_t{c->value_at(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<int, bool>*>(base_curve);
        return bool{c->value_at(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value
  operator()(bool driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->get_type();

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<bool, float>*>(base_curve);
        return float{c->value_at(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<bool, int>*>(base_curve);
        return int32_t{c->value_at(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<bool, bool>*>(base_curve);
        return bool{c->value_at(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value operator()(
      const std::vector<ossia::value>& t_driver,
      const std::shared_ptr<curve_abstract>& c)
  {
    std::vector<ossia::value> t_value = t_driver;
    for (auto& v : t_value)
    {
      if (v.valid())
      {
        v = ossia::apply_nonnull(
            [&](const auto& e) { return this->operator()(e, c); },
            std::move(v.v));
      }
    }

    return t_value;
  }

  template <std::size_t N>
  ossia::value operator()(
      std::array<float, N> driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->get_type();
    if (t.first == ossia::curve_segment_type::FLOAT
        && t.second == ossia::curve_segment_type::FLOAT)
    {
      auto c = static_cast<curve<float, float>*>(base_curve);
      for (std::size_t i = 0; i < N; i++)
      {
        driver[i] = c->value_at(driver[i]);
      }
      return driver;
    }
    else
    {
      return {};
    }
  }

  template <std::size_t N>
  ossia::value operator()(
      std::array<float, N> driver, const std::vector<ossia::behavior>& t_drive)
  {
    if (t_drive.size() != N)
      return {};

    for (std::size_t i = 0; i < N; i++)
    {
      auto curve_p = t_drive[i].target<std::shared_ptr<curve_abstract>>();
      if (!curve_p)
        return {};

      auto c = curve_p->get();
      if (!c)
        return {};

      auto t = c->get_type();
      if (t.first == ossia::curve_segment_type::FLOAT
          && t.second == ossia::curve_segment_type::FLOAT)
        driver[i] = static_cast<curve<float, float>*>(c)->value_at(driver[i]);
      else
        return {};
    }

    return driver;
  }

  ossia::value operator()(
      const std::vector<ossia::value>& t_driver,
      const std::vector<ossia::behavior>& t_drive)
  {
    std::vector<ossia::value> t_value;
    t_value.reserve(t_drive.size());
    auto it_driver = t_driver.begin();

    for (const auto& e_drive : t_drive)
    {
      if (it_driver == t_driver.end())
        break;

      if(it_driver->valid() && e_drive)
      {
        t_value.push_back(ossia::apply(*this, it_driver->v, e_drive.v));
      }
      else
      {
        t_value.push_back({});
      }
      it_driver++;
    }

    return t_value;
  }

  template <typename T, typename U>
  ossia::value operator()(const T& driver, const U& t_drive)
  {
    throw invalid_value_type_error(
        "mapper_compute_visitor_2: "
        "invalid case");
    return {};
  }
};

}
}
