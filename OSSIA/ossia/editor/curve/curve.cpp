#include <ossia/editor/curve/curve_abstract.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/curve/behavior.hpp>

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
          eggs::variants::apply(*this, b);
      }
  };
  eggs::variants::apply(behavior_reset{}, *this);
}
}
