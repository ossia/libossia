#include "qml_autom.hpp"
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/editor/curve/curve_segment/linear.hpp>
#include <ossia/editor/curve/curve.hpp>


namespace ossia
{
namespace qt
{

qml_process::qml_process(QQuickItem* parent):
  QQuickItem{parent}
{
  connect(this, &QQuickItem::parentChanged,
          this, &qml_process::reset);
}

qml_process::~qml_process()
{

}

void qml_process::setup()
{

}

std::shared_ptr<time_process> qml_process::process() const
{
  return {};
}

void qml_process::reset()
{
  if(m_cur_parent)
  {
    m_cur_parent->unregisterProcess(this);
  }
  if(auto p = qobject_cast<qml_interval*>(parent()))
  {
    p->registerProcess(this);
    m_cur_parent = p;
  }
}

void qml_process::reset_impl()
{

}


qml_autom::qml_autom(QQuickItem* parent)
  : qml_process{parent}
{
  reset();
}

qml_autom::~qml_autom()
{

}

void qml_autom::setup()
{
  if(!m_target)
    return;

  auto n = m_target->ossiaNode();
  if(!n)
    return;

  auto p = n->get_parameter();
  if(!p)
    return;

  auto curve = std::make_shared<ossia::curve<double, float>>();
  curve->set_scale_bounds(0, 1, 1);
  const auto items = childItems();
  if(items.empty())
  {
    curve->set_x0(0.);
    curve->set_y0(m_yMin);
    curve->add_point(ossia::curve_segment_linear<float>{}, 1.0, m_yMax);
  }
  else if(items.size() == 1)
  {
    if(auto cd = qobject_cast<qml_segment*>(items[0]))
    {
      curve->set_x0(0.);
      curve->set_y0(m_yMin);
      curve->add_point(cd->segment(), 1.0, m_yMax);
    }
  }
  for(auto cld : childItems())
  {
    if(auto bp = qobject_cast<qml_breakpoint*>(cld))
    {

    }
    else if(auto cd = qobject_cast<qml_segment*>(cld))
    {

    }
  }

}

qml_node_base*qml_autom::target() const
{
  return m_target;
}

double qml_autom::xMin() const
{
  return m_xMin;
}

double qml_autom::xMax() const
{
  return m_xMax;
}

double qml_autom::yMin() const
{
  return m_yMin;
}

double qml_autom::yMax() const
{
  return m_yMax;
}

void qml_autom::setTarget(qml_node_base* target)
{
  if (m_target == target)
    return;

  m_target = target;
  emit targetChanged(m_target);
}

void qml_autom::setXMin(double xMin)
{
  if (qFuzzyCompare(m_xMin, xMin))
    return;

  m_xMin = xMin;
  emit xMinChanged(m_xMin);
}

void qml_autom::setXMax(double xMax)
{
  if (qFuzzyCompare(m_xMax, xMax))
    return;

  m_xMax = xMax;
  emit xMaxChanged(m_xMax);
}

void qml_autom::setYMin(double yMin)
{
  if (qFuzzyCompare(m_yMin, yMin))
    return;

  m_yMin = yMin;
  emit yMinChanged(m_yMin);
}

void qml_autom::setYMax(double yMax)
{
  if (qFuzzyCompare(m_yMax, yMax))
    return;

  m_yMax = yMax;
  emit yMaxChanged(m_yMax);
}

void qml_autom::reset_impl()
{

}

std::function<float (double, float, float)> qml_segment::segment()
{
  return curve_segment_linear<float>();
}

}
}
