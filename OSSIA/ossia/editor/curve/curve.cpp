// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/curve/behavior.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/curve/curve_abstract.hpp>

namespace ossia
{
constant_curve::~constant_curve()
{

}

}

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
      if (p)
        p->reset();
    }

    void operator()(const std::vector<behavior>& p)
    {
      for (auto& b : p)
        ossia::apply_nonnull(*this, b);
    }

    void operator()()
    {
    }
  };

  ossia::apply(behavior_reset{}, *this);
}
}
