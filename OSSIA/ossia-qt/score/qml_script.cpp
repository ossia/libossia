#include "qml_script.hpp"
#include <ossia-qt/js_utilities.hpp>
#include <ossia/network/base/node_functions.hpp>
namespace ossia
{
namespace qt
{
qml_script::qml_script(QQuickItem* parent)
  : qml_process{parent}
{
  reset();
}

qml_script::~qml_script()
{

}

void qml_script::setup()
{
  m_impl = std::make_shared<qml_script_process>();
  m_impl->expr = new QQmlExpression(m_script, qmlContext(this), this);
}

std::shared_ptr<time_process> qml_script::process() const
{
  return m_impl;
}

void qml_script::reset_impl()
{

}


state_element qml_script_process::state(time_value date, double pos)
{
  auto script = static_cast<qml_script*>(expr->scopeObject());
  script->setDate(date);
  script->setPosition(pos);
  script->dateChanged(date.impl);
  script->positionChanged(pos);

  auto val = expr->evaluate();

  if(val.isValid())
  {
    if(val.canConvert<ossia::qt::qml_message>())
    {
      const auto& mess = val.value<ossia::qt::qml_message>();
      return mess.toState();
    }
    else if(val.canConvert<QJSValue>())
    {
      auto jsv = val.value<QJSValue>();
      if(jsv.isArray())
      {
        ossia::state s;
        QJSValueIterator arr_it{jsv};
        while (arr_it.hasNext()) {
          arr_it.next();
          auto var = arr_it.value().toVariant();
          if(var.canConvert<ossia::qt::qml_message>()) {
            s.add(var.value<ossia::qt::qml_message>().toState());
          }
        }
        return s;
      }
      else if(jsv.isObject())
      {

      }

    }
  }
  return {};
}

state_element qml_message::toState() const
{
  auto cld = ossia::net::find_nodes(device()->device().get_root_node(), address().toStdString());

  auto to_message = [&] (const ossia::net::node_base* node) -> ossia::state_element {
    auto param = node->get_parameter();
    if(param)
    {
      return ossia::message{*param, param->value().apply(ossia::qt::variant_inbound_visitor{value()})};
    }
    return {};
  };
  switch(cld.size())
  {
    case 0:
    {
      return {};
    }
    case 1:
    {
      return to_message(cld.front());
    }
    default:
    {
      ossia::state s;
      for(auto p : cld)
      {
        if(auto m = to_message(p))
          s.add(std::move(m));
      }
      return s;
    }
  }
}

}
}
