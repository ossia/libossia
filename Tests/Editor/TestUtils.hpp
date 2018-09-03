#pragma once
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/graph_node.hpp>
#if defined(OSSIA_QT)
#include <ossia-qt/js_utilities.hpp>
#endif

#include "../Network/TestUtils.hpp"
/*
namespace QTest {
inline char* toString(const ossia::value &point)
{
  QByteArray ba = QByteArray::fromStdString(ossia::value_to_pretty_string(point));
  return qstrdup(ba.data());
}
inline char* toString(const ossia::token_request& tk)
{
  QString s = "["
              + QString::number(tk.date.impl) + ", "
              + QString::number(tk.position) + ", "
              + QString::number(tk.offset.impl) + "]";
  QByteArray ba = s.toLatin1();
  return qstrdup(ba.data());
}
inline char* toString(const std::string& str)
{
  QByteArray ba = QByteArray::fromStdString(str);
  return qstrdup(ba.data());
}
inline char* toString(const ossia::time_event::status& str)
{
  switch(str)
  {
    case ossia::time_event::status::DISPOSED: return qstrdup("disposed");
    case ossia::time_event::status::HAPPENED: return qstrdup("happened");
    case ossia::time_event::status::NONE: return qstrdup("none");
    case ossia::time_event::status::PENDING: return qstrdup("pending");
  }
  return qstrdup("error");
}
inline char* toString(const ossia::time_value& val)
{
  return QTest::toString(QByteArray::number((qint64)val.impl));
}

inline char* toString(const ossia::small_vector<ossia::token_request, 4>& val)
{
  QString s = "[";
  for(auto& token : val)
    s += toString(token) + QString(", ");
  s += "]";
  QByteArray ba = s.toLatin1();
  return qstrdup(ba.data());
}
}

namespace ossia
{
using QTest::toString;
}
*/
struct root_scenario
{
  std::shared_ptr<ossia::time_sync> start_node{std::make_shared<ossia::time_sync>()};
  std::shared_ptr<ossia::time_sync> end_node{std::make_shared<ossia::time_sync>()};

  std::shared_ptr<ossia::time_event> start_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *start_node, ossia::expressions::make_expression_true())};
  std::shared_ptr<ossia::time_event> end_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *end_node, ossia::expressions::make_expression_true())};

  std::shared_ptr<ossia::time_interval> interval{ossia::time_interval::create(
            {},
            *start_event, *end_event,
          ossia::time_value{15000}, ossia::time_value{15000}, ossia::time_value{15000})};
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

/*
inline QDebug operator<<(QDebug d, ossia::time_event::status st)
{
  switch(st)
  {
    case ossia::time_event::status::NONE: d << "none"; break;
    case ossia::time_event::status::PENDING: d << "pending"; break;
    case ossia::time_event::status::HAPPENED: d << "happened"; break;
    case ossia::time_event::status::DISPOSED: d << "disposed"; break;
  }
  return d;
}
inline QDebug operator<<(QDebug d, const ossia::token_request& tk)
{
  QString str = "["
              + QString::number(tk.date.impl) + ", "
              + QString::number(tk.position) + ", "
              + QString::number(tk.offset.impl) + "]";
  return d << str;
}
inline QDebug operator<<(QDebug d, const ossia::small_vector<ossia::token_request, 4>& val)
{
  QString s = "[";
  for(auto& tk : val)
  {
    QString str = "["
                + QString::number(tk.date.impl) + ", "
                + QString::number(tk.position) + ", "
                + QString::number(tk.offset.impl) + "]";
    s += str;
    s += QString(", ");
  }
  s += "]";
  return d << s;
}
inline QDebug operator<<(QDebug d, const ossia::time_value&  s)
{
  return d << s.impl;
}
*/

inline
std::ostream& operator<<(std::ostream& d, ossia::token_request t)
{
  d << (int64_t)t.date << " "
    << t.position << " "
    << (int64_t)t.offset << " "
    << t.start_discontinuous << " "
    << t.end_discontinuous << std::endl;
  return d;
}

inline
std::ostream& operator<<(std::ostream& d, decltype(ossia::graph_node::requested_tokens) t)
{
  for(auto tk : t) d << tk << ", ";
  return d;
}
