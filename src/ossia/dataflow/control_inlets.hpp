#pragma once
#include <ossia/dataflow/port.hpp>

namespace ossia
{
class OSSIA_EXPORT minmax_float_inlet : public value_inlet
{
public:
  minmax_float_inlet();
  ~minmax_float_inlet();

  void pre_process() override;

  ossia::value_inlet min_inlet;
  ossia::value_inlet max_inlet;
};

class OSSIA_EXPORT minmax_float_outlet : public value_outlet
{
public:
  minmax_float_outlet();
  ~minmax_float_outlet();

  void pre_process() override;

  ossia::value_inlet min_inlet;
  ossia::value_inlet max_inlet;
};
}
