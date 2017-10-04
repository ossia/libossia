#include "qml_autom.hpp"
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/editor/curve/curve_segment/linear.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia-qt/device/qml_device.hpp>


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
  m_impl = std::make_shared<ossia::automation>();
  reset();
}

qml_autom::~qml_autom()
{

}

void qml_autom::setup()
{
  if(!m_targetNode)
    return;

  auto p = m_targetNode->get_parameter();
  if(!p)
    return;

  auto curve = std::make_shared<ossia::curve<double, float>>();
  //curve->set_scale_bounds(0, 1, 1);
  const auto items = childItems();
  curve->set_x0(m_xMin);
  curve->set_y0(m_yMin);
  if(items.empty())
  {
    curve->add_point(ossia::curve_segment_linear<float>{}, m_xMax, m_yMax);
  }
  else
  {
    for(auto cld : childItems())
    {
      if(auto bp = qobject_cast<qml_breakpoint*>(cld))
      {
        curve->add_point(bp->segment(), bp->x(), bp->y());
      }
    }
  }

  m_impl->set_behavior(curve);
  m_impl->set_destination(*p);
}

QVariant qml_autom::target() const
{
  return QVariant::fromValue(m_target);
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

std::shared_ptr<time_process> qml_autom::process() const
{
  return m_impl;
}

void qml_autom::on_node_deleted(const ossia::net::node_base& n) {
  m_targetNode = nullptr;
  qDebug( ) << "deleted: " << n.get_name().c_str();
}

void qml_autom::setTarget(QVariant var)
{
  qDebug() << var;
  if(m_target)
  {
    disconnect(m_death);
  }
  if(m_targetNode)
  {
    m_targetNode->about_to_be_deleted.disconnect<qml_autom, &qml_autom::on_node_deleted>(*this);
  }

  if(var.canConvert<qt::qml_node_base*>())
  {
    auto target = var.value<qt::qml_node_base*>();
    if (m_target == target)
      return;

    m_target = target;
    m_targetNode = m_target->ossiaNode();
    m_death = connect(m_target, &QObject::destroyed,
            this, [=] {
      m_target = nullptr;
      m_targetNode = nullptr;
    });
    emit targetChanged(QVariant::fromValue(m_target));
  }
  else if(var.canConvert<QString>())
  {
    const auto addr = var.toString().toStdString();
    ossia::net::address_scope scope = ossia::net::get_address_scope(addr);
    switch(scope)
    {
      case net::address_scope::absolute:
      {
        auto& root = qml_singleton_device::instance().device().get_root_node();

        auto node = ossia::net::find_node(root, addr);
        if(node && node->get_parameter())
        {
          m_target = nullptr;
          m_targetNode = node;
        }
        break;
      }
      case net::address_scope::relative:
      case net::address_scope::global:
        // TODO
        break;
    }
  }
  else
  {
    m_target = nullptr;
    m_targetNode = nullptr;
  }

  if(m_targetNode)
  {
    m_targetNode->about_to_be_deleted.connect<qml_autom, &qml_autom::on_node_deleted>(*this);
  }
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

std::function<float (double, float, float)> qml_breakpoint::segment()
{
  switch(m_type)
  {
    case QEasingCurve::Linear:
      return curve_segment_linear<float>();
      break;

    case QEasingCurve::InQuad:
      return ossia::curve_segment_ease<float, easing::quadraticIn>{};
    case QEasingCurve::OutQuad:
      return ossia::curve_segment_ease<float, easing::quadraticOut>{};
    case QEasingCurve::InOutQuad:
      return ossia::curve_segment_ease<float, easing::quadraticInOut>{};
    case QEasingCurve::OutInQuad:
      //TODO return ossia::curve_segment_ease<float, easing::quadraticOutIn>{};
      break;

    case QEasingCurve::InCubic:
      return ossia::curve_segment_ease<float, easing::cubicIn>{};
    case QEasingCurve::OutCubic:
      return ossia::curve_segment_ease<float, easing::cubicOut>{};
    case QEasingCurve::InOutCubic:
      return ossia::curve_segment_ease<float, easing::cubicInOut>{};
    case QEasingCurve::OutInCubic:
      //TODO return ossia::curve_segment_ease<float, easing::cubicOutIn>{};
      break;

    case QEasingCurve::InQuart:
      return ossia::curve_segment_ease<float, easing::quarticIn>{};
    case QEasingCurve::OutQuart:
      return ossia::curve_segment_ease<float, easing::quarticOut>{};
    case QEasingCurve::InOutQuart:
      return ossia::curve_segment_ease<float, easing::quarticInOut>{};
    case QEasingCurve::OutInQuart:
      //TODO return ossia::curve_segment_ease<float, easing::quarticOutIn>{};
      break;

    case QEasingCurve::InQuint:
      return ossia::curve_segment_ease<float, easing::quinticIn>{};
    case QEasingCurve::OutQuint:
      return ossia::curve_segment_ease<float, easing::quinticOut>{};
    case QEasingCurve::InOutQuint:
      return ossia::curve_segment_ease<float, easing::quinticInOut>{};
    case QEasingCurve::OutInQuint:
      //TODO return ossia::curve_segment_ease<float, easing::quinticOutIn>{};
      break;

    case QEasingCurve::InSine:
      return ossia::curve_segment_ease<float, easing::sineIn>{};
    case QEasingCurve::OutSine:
      return ossia::curve_segment_ease<float, easing::sineOut>{};
    case QEasingCurve::InOutSine:
      return ossia::curve_segment_ease<float, easing::sineInOut>{};
    case QEasingCurve::OutInSine:
      //TODO return ossia::curve_segment_ease<float, easing::sineOutIn>{};
      break;

    case QEasingCurve::InExpo:
      return ossia::curve_segment_ease<float, easing::exponentialIn>{};
    case QEasingCurve::OutExpo:
      return ossia::curve_segment_ease<float, easing::exponentialOut>{};
    case QEasingCurve::InOutExpo:
      return ossia::curve_segment_ease<float, easing::exponentialInOut>{};
    case QEasingCurve::OutInExpo:
      //TODO return ossia::curve_segment_ease<float, easing::exponentialOutIn>{};
      break;

    case QEasingCurve::InCirc:
      return ossia::curve_segment_ease<float, easing::circularIn>{};
    case QEasingCurve::OutCirc:
      return ossia::curve_segment_ease<float, easing::circularOut>{};
    case QEasingCurve::InOutCirc:
      return ossia::curve_segment_ease<float, easing::circularInOut>{};
    case QEasingCurve::OutInCirc:
      //TODO return ossia::curve_segment_ease<float, easing::circularOutIn>{};
      break;

    case QEasingCurve::InElastic:
      return ossia::curve_segment_ease<float, easing::elasticIn>{};
    case QEasingCurve::OutElastic:
      return ossia::curve_segment_ease<float, easing::elasticOut>{};
    case QEasingCurve::InOutElastic:
      return ossia::curve_segment_ease<float, easing::elasticInOut>{};
    case QEasingCurve::OutInElastic:
      //TODO return ossia::curve_segment_ease<float, easing::elasticOutIn>{};
      break;

    case QEasingCurve::InBack:
      return ossia::curve_segment_ease<float, easing::backIn>{};
    case QEasingCurve::OutBack:
      return ossia::curve_segment_ease<float, easing::backOut>{};
    case QEasingCurve::InOutBack:
      return ossia::curve_segment_ease<float, easing::backInOut>{};
    case QEasingCurve::OutInBack:
      //TODO return ossia::curve_segment_ease<float, easing::backOutIn>{};
      break;

    case QEasingCurve::InBounce:
      return ossia::curve_segment_ease<float, easing::bounceIn>{};
    case QEasingCurve::OutBounce:
      return ossia::curve_segment_ease<float, easing::bounceOut>{};
    case QEasingCurve::InOutBounce:
      return ossia::curve_segment_ease<float, easing::bounceInOut>{};
    case QEasingCurve::OutInBounce:
      //TODO return ossia::curve_segment_ease<float, easing::bounceOutIn>{};
      break;

    case QEasingCurve::InCurve:
    case QEasingCurve::OutCurve:
    case QEasingCurve::SineCurve:
    case QEasingCurve::CosineCurve:
    case QEasingCurve::BezierSpline:
    case QEasingCurve::TCBSpline:
    case QEasingCurve::Custom:
    case QEasingCurve::NCurveTypes:
      break;
  }
  return curve_segment_linear<float>();
}

}
}
