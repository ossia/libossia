#pragma once
#include <ossia/ossia.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <QMetaType>
#include <QTest>

#include "../Network/TestUtils.hpp"

namespace QTest {
template<>
inline char* toString(const ossia::value &point)
{
    QByteArray ba = QByteArray::fromStdString(ossia::value_to_pretty_string(point));
    return qstrdup(ba.data());
}
template<>
inline char* toString(const std::string& str)
{
    QByteArray ba = QByteArray::fromStdString(str);
    return qstrdup(ba.data());
}
}

struct root_scenario
{
  std::shared_ptr<ossia::time_sync> start_node{std::make_shared<ossia::time_sync>()};
  std::shared_ptr<ossia::time_sync> end_node{std::make_shared<ossia::time_sync>()};

  std::shared_ptr<ossia::time_event> start_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *start_node, ossia::expressions::make_expression_true())};
  std::shared_ptr<ossia::time_event> end_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *end_node, ossia::expressions::make_expression_true())};

  std::shared_ptr<ossia::time_interval> interval{ossia::time_interval::create(
            [] (auto&&...) {},
            *start_event, *end_event,
            ossia::time_value(15000), ossia::time_value(15000), ossia::time_value(15000))};
  std::shared_ptr<ossia::scenario> scenario{std::make_shared<ossia::scenario>()};
  ossia::clock clck{*interval};

  root_scenario()
  {
    using namespace ossia;
    start_node->insert(start_node->get_time_events().end(), start_event);
    end_node->insert(end_node->get_time_events().end(), end_event);

    using namespace std::literals;
    clck.set_granularity(50ms);

    interval->add_time_process(scenario);
    auto scen_sn = scenario->get_start_time_sync();
    auto scen_se = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *scen_sn, ossia::expressions::make_expression_true());
    scen_sn->insert(scen_sn->get_time_events().end(), scen_se);
  }
};

inline
std::ostream& operator<<(std::ostream& s, ossia::time_event::status st)
{
  switch(st)
  {
    case ossia::time_event::status::NONE: s << "none"; break;
    case ossia::time_event::status::PENDING: s << "pending"; break;
    case ossia::time_event::status::HAPPENED: s << "happened"; break;
    case ossia::time_event::status::DISPOSED: s << "disposed"; break;
  }
  return s;
}
