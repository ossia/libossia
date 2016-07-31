#include <ossia/editor/curve/detail/Curve_impl.tpp>
#include <ossia/editor/value/value.hpp>

namespace ossia
{
curve_abstract::~curve_abstract()
{

}

// Curve implementation
template<class X, class Y>
std::shared_ptr<curve<X, Y>> curve<X, Y>::create()
{
  return std::make_shared<detail::curve_impl<X, Y>>();
}
template<class X, class Y>
curve<X, Y>::~curve() = default;

}

// Explicit instantiation
template class OSSIA_EXPORT ossia::curve<double, bool>;
template class OSSIA_EXPORT ossia::curve<double, int>;
template class OSSIA_EXPORT ossia::curve<double, float>;

template class OSSIA_EXPORT ossia::curve<bool, bool>;
template class OSSIA_EXPORT ossia::curve<bool, int>;
template class OSSIA_EXPORT ossia::curve<bool, float>;

template class OSSIA_EXPORT ossia::curve<int, bool>;
template class OSSIA_EXPORT ossia::curve<int, int>;
template class OSSIA_EXPORT ossia::curve<int, float>;

template class OSSIA_EXPORT ossia::curve<float, bool>;
template class OSSIA_EXPORT ossia::curve<float, int>;
template class OSSIA_EXPORT ossia::curve<float, float>;
