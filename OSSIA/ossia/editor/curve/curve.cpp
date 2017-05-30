#include <ossia/editor/curve/curve_abstract.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/curve/behavior.hpp>

namespace ossia
{
template<typename X, typename Y>
void curve<X, Y>::reset()
{
  m_y0_cacheUsed = false;
  if(m_originalPoints)
    m_points = *m_originalPoints;
}


template<typename X, typename Y>
bool curve<X,Y>::add_point(ossia::curve_segment<Y> segment, X abscissa, Y value)
{
  if(m_scaleBounds)
    m_originalPoints->emplace(abscissa, std::make_pair(value, segment));
  m_points.emplace(abscissa, std::make_pair(value, std::move(segment)));

  return true;
}

template<typename X, typename Y>
bool curve<X,Y>::remove_point(X abscissa)
{
  return m_points.erase(abscissa) > 0;
}

template<typename X, typename Y>
Y curve<X,Y>::value_at(X abscissa) const
{
  X lastAbscissa = get_x0();
  Y lastValue = get_y0();

  auto end = m_points.end();
  for (auto it = m_points.begin(); it != end; ++it)
  {
    if (abscissa > lastAbscissa && abscissa <= it->first)
    {
      lastValue = it->second.second(
            ((double)abscissa - (double)lastAbscissa)
            / ((double)it->first - (double)lastAbscissa),
            lastValue, it->second.first);
      break;
    }
    else if (abscissa > it->first)
    {
      lastAbscissa = it->first;
      lastValue = it->second.first;
    }
    else
      break;
  }

  return lastValue;
}

template<typename X, typename Y>
curve_type curve<X,Y>::get_type() const
{
  return std::make_pair(curve_segment_type_map<X>, curve_segment_type_map<Y>);
}

template<typename X, typename Y>
X curve<X,Y>::get_x0() const
{
  if(!m_x0_destination)
    return m_x0;
  else
  {
    auto& address = m_x0_destination->value.get();
    address.pull_value();
    auto val = address.value();
    auto res = convert_to_template_type_value(
          val, m_x0_destination->index.begin());
    return res;
  }
}

template<typename X, typename Y>
Y curve<X,Y>::get_y0() const
{
  if(!m_y0_destination)
    return m_y0;
  else
  {
    if (m_y0_cacheUsed)
      return m_y0_cache;

    const Destination& dest = *m_y0_destination;
    m_y0_cacheUsed = true;
    m_y0_cache = convert_to_template_type_value(
          dest.address().fetch_value(), dest.index.begin());

    if(m_scaleBounds)
    {
      scale_info& bounds = *m_scaleBounds;
      bounds.start = m_y0_cache;

      // Rescale the whole curve with the acquired information.
      // It is currently in [0; 1].
      if(bounds.start < bounds.min)
        bounds.min = bounds.start;
      else if(bounds.start > bounds.max)
        bounds.max = bounds.start;

      if (bounds.start < bounds.end)
      {
        for(auto& pt : m_points)
        {
          auto& y = pt.second.first;
          y = ossia::easing::ease{}(bounds.min, bounds.max, y);
        }
      }
      else if (bounds.start == bounds.end)
      {
        m_points.clear();
        m_x0 = 0;
      }
      else // start > end
      {
        for(auto& pt : m_points)
        {
          auto& y = pt.second.first;
          const auto fun = [=] (Y val) -> Y { return bounds.max - val * (bounds.max - bounds.min); };
          y = fun(y);
        }
      }

    }
    return m_y0_cache;
  }
}

template<typename X, typename Y>
void curve<X,Y>::set_x0(X value)
{
  m_x0 = value;
}

template<typename X, typename Y>
void curve<X,Y>::set_y0(Y value)
{
  m_y0 = value;
}

template<typename X, typename Y>
ossia::optional<Destination> curve<X,Y>::get_x0_destination() const
{
  return m_x0_destination;
}

template<typename X, typename Y>
ossia::optional<Destination> curve<X,Y>::get_y0_destination() const
{
  return m_y0_destination;
}

template<typename X, typename Y>
void curve<X,Y>::set_x0_destination(const Destination& destination)
{
  m_x0_destination = destination;
}

template<typename X, typename Y>
void curve<X,Y>::set_y0_destination(const Destination& destination)
{
  m_y0_destination = destination;
}

template<typename X, typename Y>
typename curve<X,Y>::map_type curve<X,Y>::get_points() const
{
  return m_points;
}

template<typename X, typename Y>
void curve<X,Y>::set_scale_bounds(Y min, Y max, Y end)
{
  m_scaleBounds = scale_info{min, max, Y{}, end};
  m_originalPoints = map_type{};
}

template<typename X, typename Y>
Y curve<X,Y>::convert_to_template_type_value(const value& v, ossia::destination_index::const_iterator idx)
{
  using namespace ossia;
  using namespace std;
  struct visitor
  {
    destination_index::const_iterator index;
    Y operator()(int32_t i) const
    {
      return i;
    }
    Y operator()(float f) const
    {
      return f;
    }
    Y operator()(bool b) const
    {
      return b;
    }
    Y operator()(char c) const
    {
      return c;
    }
    Y operator()(vec2f vec) const
    {
      return vec[*index];
    }
    Y operator()(vec3f vec) const
    {
      return vec[*index];
    }
    Y operator()(vec4f vec) const
    {
      return vec[*index];
    }
    Y operator()(const std::vector<ossia::value>& t) const
    {
      auto& val = t[*index];
      return convert_to_template_type_value(val, index + 1);
    }

    Y operator()(impulse) const
    {
      throw invalid_value_type_error("curve_impl::convertToTemplateTypeValue: "
                                     "Cannot convert Impulse to a numeric type");
      return {};
    }
    Y operator()(const std::string& str) const
    {
      throw invalid_value_type_error("curve_impl::convertToTemplateTypeValue: "
                                     "Cannot convert String to a numeric type");
      return {};
    }
    Y operator()() const
    {
      throw invalid_value_type_error("curve_impl::convertToTemplateTypeValue: "
                                     "No type provided");
      return {};
    }
  } vis{idx};

  return v.apply(vis);
}

}

template class ossia::curve<int, int>;
template class ossia::curve<int, float>;
template class ossia::curve<int, double>;
template class ossia::curve<int, bool>;
template class ossia::curve<float, int>;
template class ossia::curve<float, float>;
template class ossia::curve<float, double>;
template class ossia::curve<float, bool>;
template class ossia::curve<double, int>;
template class ossia::curve<double, float>;
template class ossia::curve<double, double>;
template class ossia::curve<double, bool>;
template class ossia::curve<bool, int>;
template class ossia::curve<bool, float>;
template class ossia::curve<bool, double>;
template class ossia::curve<bool, bool>;
namespace ossia
{
curve_abstract::~curve_abstract()
{
}

void behavior::reset()
{
  struct behavior_reset
  {
    void operator()(const curve_ptr& p)
    {
      if(p)
        p->reset();
    }

    void operator()(const std::vector<behavior>& p)
    {
      for(auto& b : p)
        ossia::apply_nonnull(*this, b);
    }

    void operator()() { }
  };

  ossia::apply(behavior_reset{}, *this);
}


}
