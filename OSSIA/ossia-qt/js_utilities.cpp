#if defined(QT_CORE_LIB)
#include "js_utilities.hpp"

namespace ossia
{
namespace net
{
OSSIA_EXPORT void sanitize_name(QString& ret)
{
  const QChar underscore = '_';
  for(auto& c : ret)
  {
    if(ossia::net::is_valid_character_for_name(c))
      continue;
    else
      c = underscore;
  }
}
}
namespace qt
{

value js_value_inbound_visitor::operator()(impulse) const
{
  return impulse{};
}

value js_value_inbound_visitor::operator()(int32_t v) const
{
  return int32_t(val.toInt());
}

value js_value_inbound_visitor::operator()(float v) const
{
  return float(val.toNumber());
}

value js_value_inbound_visitor::operator()(bool v) const
{
  return bool(val.toBool());
}

value js_value_inbound_visitor::operator()(char v) const
{
  auto str = val.toString();
  if(str.size() > 0)
    return char(str[0].toLatin1());
  return v;
}

value js_value_inbound_visitor::operator()(const std::string &v) const
{
  return std::string(val.toString().toStdString());
}

value js_value_inbound_visitor::operator()(const std::vector<ossia::value> &v) const
{
  if(val.isArray())
  {
    std::vector<ossia::value> t;

    QJSValueIterator it(val);
    while(it.hasNext())
    {
      it.next();
      t.push_back(value_from_jsvalue(it.value()));
    }

    return std::move(t);
  }
  return v;
}

value js_value_inbound_visitor::operator()(vec2f v) const
{
  if(val.isArray())
  {
    QJSValueIterator it(val);
    int i = 0;
    const int N = v.size();
    while(it.hasNext() && i < N)
    {
      it.next();
      v[i] = it.value().toNumber();
    }
  }
  return v;
}

value js_value_inbound_visitor::operator()(vec3f v) const
{
  if(val.isArray())
  {
    QJSValueIterator it(val);
    int i = 0;
    const int N = v.size();
    while(it.hasNext() && i < N)
    {
      it.next();
      v[i] = it.value().toNumber();
    }
  }
  return v;
}

value js_value_inbound_visitor::operator()(vec4f v) const
{
  if(val.isArray())
  {
    QJSValueIterator it(val);
    int i = 0;
    const int N = v.size();
    while(it.hasNext() && i < N)
    {
      it.next();
      v[i] = it.value().toNumber();
    }
  }
  return v;
}

value js_value_inbound_visitor::operator()(const Destination &t) { return t; }

value js_value_inbound_visitor::operator()() const { return {}; }



net::generic_address_data make_address_data(const QJSValue &js)
{
  using namespace ossia::net;
  generic_address_data dat;

  QJSValue name = js.property("name");
  if(name.isString())
  {
    dat.node_name = name.toString().toStdString();
  }
  else
  {
    return dat;
  }

  dat.type = get_enum<ossia::val_type>(js.property("type"));

  if(dat.type)
  {
    auto base_v = init_value(*dat.type);
    auto domain = init_domain(*dat.type);
    set_min(domain, value_from_jsvalue(base_v, js.property("min")));
    set_max(domain, value_from_jsvalue(base_v, js.property("max")));

    dat.domain = domain;
    dat.access = get_enum<ossia::access_mode>(js.property("access"));
    dat.bounding = get_enum<ossia::bounding_mode>(js.property("bounding"));
    dat.repetition_filter = get_enum<ossia::repetition_filter>(js.property("repetition_filter"));
    dat.unit = ossia::parse_pretty_unit(js.property("unit").toString().toStdString());
    dat.description = js.property("description").toString().toStdString();
    QJSValue tags = js.property("tags");
    if(tags.isArray())
    {
      QJSValueIterator tags_it{tags};
      while(tags_it.hasNext())
      {
        tags_it.next();
        auto str = tags_it.value().toString();
        if(!str.isEmpty())
          dat.tags.push_back(str.toStdString());
      }
    }
  }

  return dat;
}




QJSValue js_value_outbound_visitor::to_enum(qml_context::val_type t) const
{
  return engine.toScriptValue(QVariant::fromValue(t));
}

QJSValue js_value_outbound_visitor::operator()(impulse) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Impulse));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(int32_t val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Int));
  v.setProperty("value", int32_t(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(float val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Float));
  v.setProperty("value", val);
  return v;
}

QJSValue js_value_outbound_visitor::operator()(bool val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Bool));
  v.setProperty("value", val);
  return v;
}

QJSValue js_value_outbound_visitor::operator()(char val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Char));
  v.setProperty("value", val);
  return v;
}

QJSValue js_value_outbound_visitor::operator()(const std::string &val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::String));
  v.setProperty("value", QString::fromStdString(val));
  return v;
}

QJSValue js_value_outbound_visitor::make_tuple(const std::vector<value> &arr) const
{
  auto array = engine.newArray(arr.size());
  int i = 0;

  for(const auto& child : arr)
  {
    array.setProperty(i++, value_to_js_value(child, engine));
  }

  return array;
}

QJSValue js_value_outbound_visitor::operator()(const std::vector<ossia::value> &val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Tuple));
  v.setProperty("value", make_tuple(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(vec2f val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Vec2f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(vec3f val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Vec3f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(vec4f val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Vec4f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(const Destination &t) { return {}; }

QJSValue js_value_outbound_visitor::operator()() const { return {}; }




QString js_string_outbound_visitor::operator()(impulse) const
{
  return QStringLiteral("\"\"");
}

QString js_string_outbound_visitor::operator()(int32_t val) const
{
  return QString::number(int32_t(val));
}

QString js_string_outbound_visitor::operator()(float val) const
{
  return QString::number(val);
}

QString js_string_outbound_visitor::operator()(bool val) const
{
  return val ? QStringLiteral("true") : QStringLiteral("false");
}

QString js_string_outbound_visitor::operator()(char val) const
{
  return "\"" % QString{val} % "\"";
}

QString js_string_outbound_visitor::operator()(const std::string &val) const
{
  return "\"" % QString::fromStdString(val) % "\"";
}

QString js_string_outbound_visitor::operator()(const std::vector<ossia::value> &val) const
{
  QString s = "[";

  std::size_t n = val.size();
  if(n != 0)
  {
    s += value_to_js_string(val[0]);
    for(std::size_t i = 1; i < n; i++)
    {
      s += ", " % value_to_js_string(val[i]);
    }
  }

  s += "]";
  return s;
}

QString js_string_outbound_visitor::operator()(vec2f val) const
{
  return make_array(val);
}

QString js_string_outbound_visitor::operator()(vec3f val) const
{
  return make_array(val);
}

QString js_string_outbound_visitor::operator()(vec4f val) const
{
  return make_array(val);
}

QString js_string_outbound_visitor::operator()(const Destination &t) { return (*this)(impulse{}); }

QString js_string_outbound_visitor::operator()() const { return (*this)(impulse{}); }

value value_from_jsvalue(const QJSValue& v)
{
  if(v.isNumber())
  {
    return v.toNumber();
  }
  else if(v.isBool())
  {
    return v.toBool();
  }
  else if(v.isString())
  {
    return v.toString().toStdString();
  }
  else if(v.isArray())
  {
    // TODO handle vec2/vec3/vec4
    QJSValueIterator it(v);
    std::vector<ossia::value> t;
    while(it.hasNext())
    {
      t.push_back(value_from_jsvalue(it.value()));
    }
    return std::move(t);
  }
  else
  {
    return {};
  }
}

void set_address_type(QVariant::Type type, net::address_base& addr)
{
  switch(type)
  {
    case QVariant::Bool:
      addr.setValueType(ossia::val_type::BOOL);
      break;
    case QVariant::Time:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::ULongLong:
      addr.setValueType(ossia::val_type::INT);
      break;
    case QVariant::Char:
      addr.setValueType(ossia::val_type::CHAR);
      break;
    case QVariant::String:
    case QVariant::ByteArray:
      addr.setValueType(ossia::val_type::STRING);
      break;
    case QVariant::Double:
      addr.setValueType(ossia::val_type::FLOAT);
      break;
    case QVariant::Color:
      addr.setUnit(ossia::argb_u{});
      break;
    case QVariant::Point:
    case QVariant::PointF:
    case QVariant::Vector2D:
    case QVariant::Size:
    case QVariant::SizeF:
      addr.setUnit(ossia::cartesian_2d_u{});
      break;
    case QVariant::Vector3D:
      addr.setUnit(ossia::cartesian_3d_u{});
      break;
    case QVariant::Vector4D:
      addr.setUnit(ossia::axis_u{});
      break;
    case QVariant::Quaternion:
      addr.setUnit(ossia::quaternion_u{});
      break;
    case QVariant::Line:
    case QVariant::LineF:
    case QVariant::Rect:
    case QVariant::RectF:
      addr.setValueType(ossia::val_type::VEC4F);
      break;
    case QVariant::List:
    case QVariant::StringList:
      addr.setValueType(ossia::val_type::TUPLE);
      break;
    case QVariant::Date:
    default:
      addr.setValueType(ossia::val_type::IMPULSE);
      break;
  }
}

QVariant ossia_to_qvariant::operator()(QVariant::Type type, const value& ossia_val)
{
  switch(type)
  {
    case QVariant::Bool:
      return QVariant::fromValue(ossia_val.get<bool>());
    case QVariant::Time:
      return QVariant::fromValue(QTime().addMSecs(ossia_val.get<int>()));
    case QVariant::Int:
      return QVariant::fromValue(ossia_val.get<int>());
    case QVariant::UInt:
      return QVariant::fromValue((quint32)ossia_val.get<int>());
    case QVariant::ULongLong:
      return QVariant::fromValue((qlonglong)ossia_val.get<int>());
    case QVariant::Char:
      return QVariant::fromValue(QChar::fromLatin1(ossia_val.get<char>()));
    case QVariant::String:
      return QVariant::fromValue(QString::fromStdString(ossia_val.get<std::string>()));
    case QVariant::ByteArray:
      return QVariant::fromValue(QByteArray::fromStdString(ossia_val.get<std::string>()));
    case QVariant::Double:
      return QVariant::fromValue((double)ossia_val.get<float>());
    case QVariant::Color:
    {
      auto val = ossia_val.get<vec4f>();
      return QVariant::fromValue(QColor::fromRgbF(val[1], val[2], val[3], val[0]));
    }
    case QVariant::Point:
    {
      auto val = ossia_val.get<vec2f>();
      return QVariant::fromValue(QPoint(val[0], val[1]));
    }
    case QVariant::PointF:
    {
      auto val = ossia_val.get<vec2f>();
      return QVariant::fromValue(QPointF(val[0], val[1]));
    }
    case QVariant::Vector2D:
    {
      auto val = ossia_val.get<vec2f>();
      return QVariant::fromValue(QVector2D(val[0], val[1]));
    }
      break;
    case QVariant::Vector3D:
    {
      auto val = ossia_val.get<vec3f>();
      return QVariant::fromValue(QVector3D(val[0], val[1], val[2]));
    }
    case QVariant::Vector4D:
    {
      auto val = ossia_val.get<vec4f>();
      return QVariant::fromValue(QVector4D(val[0], val[1], val[2], val[3]));
    }
    case QVariant::Quaternion:
    {
      auto val = ossia_val.get<vec4f>();
      return QVariant::fromValue(QQuaternion(val[0], val[1], val[2], val[3]));
    }
    case QVariant::Line:
    {
      auto val = ossia_val.get<vec4f>();
      return QVariant::fromValue(QLine(val[0], val[1], val[2], val[3]));
    }
    case QVariant::LineF:
    {
      auto val = ossia_val.get<vec4f>();
      return QVariant::fromValue(QLineF(val[0], val[1], val[2], val[3]));
    }
    case QVariant::Rect:
    {
      auto val = ossia_val.get<vec4f>();
      return QVariant::fromValue(QRect(val[0], val[1], val[2], val[3]));
    }
    case QVariant::RectF:
    {
      auto val = ossia_val.get<vec4f>();
      return QVariant::fromValue(QRectF(val[0], val[1], val[2], val[3]));
    }
    case QVariant::Size:
    {
      auto val = ossia_val.get<vec2f>();
      return QVariant::fromValue(QSize(val[0], val[1]));
    }
    case QVariant::SizeF:
    {
      auto val = ossia_val.get<vec2f>();
      return QVariant::fromValue(QSizeF(val[0], val[1]));
    }
    case QVariant::List:
    {
      break;
    }
    case QVariant::StringList:
    {
      break;
    }
    case QVariant::Date:
    default:
      break;
  }
  return {};
}

value qt_to_ossia::operator()(const QVariant& v)
{
  switch(v.type())
  {
    case QVariant::Bool:
      return operator()(v.value<bool>());
    case QVariant::Time:
      return operator()(v.value<QTime>());
    case QVariant::Int:
      return operator()(v.value<int>());
    case QVariant::UInt:
      return operator()(v.value<quint32>());
    case QVariant::ULongLong:
      return operator()(v.value<qulonglong>());
    case QVariant::Char:
      return operator()(v.value<QChar>());
    case QVariant::String:
      return operator()(v.value<QString>());
    case QVariant::ByteArray:
      return operator()(v.value<QByteArray>());
    case QVariant::Double:
      return operator()(v.value<double>());
    case QVariant::Color:
      return operator()(v.value<QColor>());
    case QVariant::Point:
      return operator()(v.value<QPoint>());
    case QVariant::PointF:
      return operator()(v.value<QPointF>());
    case QVariant::Vector2D:
      return operator()(v.value<QVector2D>());
    case QVariant::Size:
      return operator()(v.value<QSize>());
    case QVariant::SizeF:
      return operator()(v.value<QSizeF>());
    case QVariant::Vector3D:
      return operator()(v.value<QVector3D>());
    case QVariant::Vector4D:
      return operator()(v.value<QVector4D>());
    case QVariant::Quaternion:
      return operator()(v.value<QQuaternion>());
    case QVariant::Line:
      return operator()(v.value<QLine>());
    case QVariant::LineF:
      return operator()(v.value<QLineF>());
    case QVariant::Rect:
      return operator()(v.value<QRect>());
    case QVariant::RectF:
      return operator()(v.value<QRectF>());
    case QVariant::List:
      return operator()(v.value<QVariantList>());
    case QVariant::StringList:
      return operator()(v.value<QStringList>());
    case QVariant::Date:
      return operator()(v.value<QDate>());
    default:
      return operator()();
  }
}


}

}
#endif
