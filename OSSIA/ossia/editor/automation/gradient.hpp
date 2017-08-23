#pragma once
#include <ossia/detail/optional.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/value/destination.hpp>
#include <boost/container/flat_map.hpp>

namespace ossia
{
class OSSIA_EXPORT color_automation : public ossia::time_process
{
public:
  using grad_type = boost::container::flat_map<double, ossia::hunter_lab>;

  color_automation();

  void set_destination(ossia::Destination a);
  void set_gradient(grad_type t);

  ossia::state_element state(ossia::time_value date, double pos) override;

  void start(ossia::state& st) override;
  void stop() override;
  void pause() override;
  void resume() override;

private:
  optional<ossia::Destination> m_parameter;
  grad_type m_data;
};
}
