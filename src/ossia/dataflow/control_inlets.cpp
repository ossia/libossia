#include "control_inlets.hpp"

namespace ossia
{
namespace
{
void pre_process_minmax_ports(
    ossia::value_port& self,
    ossia::value_port& min_port,
    ossia::value_port& max_port)
{
  std::optional<float> min;
  std::optional<float> max;

  {
    auto& recv = min_port.get_data();
    if(!recv.empty()) {
      min = ossia::convert<float>(recv.back().value);
    }
  }
  {
    auto& recv = max_port.get_data();
    if(!recv.empty()) {
      max = ossia::convert<float>(recv.back().value);
    }
  }

  if (min && max)
  {
    self.domain = ossia::domain_base<float>{*min, *max};
  }
  else if (min)
  {
    ossia::set_min(self.domain, *min);
  }
  else if (max)
  {
    ossia::set_max(self.domain, *max);
  }
}
}

minmax_float_inlet::minmax_float_inlet()
{
  this->child_inlets.push_back(&min_inlet);
  this->child_inlets.push_back(&max_inlet);

  (*this)->domain = ossia::domain_base<float>(0.f, 1.f);
}

minmax_float_inlet::~minmax_float_inlet()
{

}

void minmax_float_inlet::pre_process()
{
  // TODO pre_process_minmax_ports(**this, *min_inlet, *max_inlet);
}

minmax_float_outlet::minmax_float_outlet()
{
  this->child_inlets.push_back(&min_inlet);
  this->child_inlets.push_back(&max_inlet);

  (*this)->domain = ossia::domain_base<float>(0.f, 1.f);
}

minmax_float_outlet::~minmax_float_outlet()
{

}

void minmax_float_outlet::pre_process()
{
  // TODO pre_process_minmax_ports(**this, *min_inlet, *max_inlet);
}

}
