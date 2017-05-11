#if defined(QT_CORE_LIB)
#include "js_utilities.hpp"
#include <ossia/editor/value/value_conversion.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/base/address_data.hpp>
namespace ossia
{
namespace net
{
OSSIA_EXPORT void sanitize_name(QString& ret)
{
  // Keep in sync with node.cpp
  const QChar underscore = '_';
  for(auto& c : ret)
  {
    if(ossia::net::is_valid_character_for_name(c))
      continue;
    else
      c = underscore;
  }
}

QString sanitize_name(QString name, const std::vector<QString>& brethren)
{
  sanitize_name(name);
  bool is_here = false;
  ossia::optional<int> name_instance;
  chobo::small_vector<int, 16> instance_num;
  const auto b_size = brethren.size();
  instance_num.reserve(b_size);

  // First get the root name : the first part of the "a.b"
  QString root_name = name;
  {
    auto pos = name.lastIndexOf('.');
    if(pos != -1)
    {
      bool res = false;
      name_instance = name.right(pos + 1).toInt(&res);
      if(res)
        root_name = name.mid(0, pos);
    }
  }

  const auto root_len = root_name.size();
  for (const QString& n_name : brethren)
  {
    if (n_name == name)
    {
      is_here = true;
    }

    if(n_name.size() < (root_len + 1))
      continue;

    bool same_root = true;
    for(int i = 0; i < root_len; i++)
    {
      if(n_name[i] != root_name[i])
      {
        break;
        same_root = false;
      }
    }

    if (same_root && (n_name[root_len] == '.'))
    {
      // Instance
      bool b = false;
      int n = n_name.rightRef(root_len + 1).toInt(&b);
      if(b)
        instance_num.push_back(n);
    }
    // case where we have the "default" instance without .0
    else if(same_root && root_len == n_name.length())
    {
      instance_num.push_back(0);
    }
  }

  if (!is_here)
  {
    return name;
  }
  else
  {
    auto n = instance_num.size();
    if ((n == 0) || ((n == 1) && (instance_num[0] == 0)))
    {
      return root_name + QStringLiteral(".1");
    }
    else
    {
      std::sort(instance_num.begin(), instance_num.end());
      return root_name % "." % QString::number(instance_num.back() + 1);
    }
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
value js_value_inbound_visitor::operator()() const { return {}; }


ossia::complex_type get_type(const QJSValue& val)
{
  // TODO handle other cases ? string, extended, etc...
  auto opt_t = get_enum<ossia::val_type>(val);
  if(opt_t) return *opt_t;
  return complex_type{};
}

net::address_data make_address_data(const QJSValue &js)
{
  using namespace ossia::net;
  address_data dat;

  QJSValue name = js.property("name");
  if(name.isString())
  {
    dat.name = name.toString().toStdString();
  }
  else
  {
    return dat;
  }

  dat.type = get_type(js.property("type"));
  if(dat.type)
  {
    ossia::val_type base = ossia::underlying_type(dat.type);
    auto base_v = init_value(base);
    auto domain = init_domain(base);
    set_min(domain, value_from_jsvalue(base_v, js.property("min")));
    set_max(domain, value_from_jsvalue(base_v, js.property("max")));

    dat.domain = domain;
    dat.access = get_enum<ossia::access_mode>(js.property("access"));
    dat.bounding = get_enum<ossia::bounding_mode>(js.property("bounding"));
    dat.repetition_filter = get_enum<ossia::repetition_filter>(js.property("repetition_filter"));
    dat.unit = ossia::parse_pretty_unit(js.property("unit").toString().toStdString());
    ossia::net::set_description(dat.extended, js.property("description").toString().toStdString());
    QJSValue tags = js.property("tags");
    if(tags.isArray())
    {
      ossia::net::tags t;

      QJSValueIterator tags_it{tags};
      while(tags_it.hasNext())
      {
        tags_it.next();
        auto str = tags_it.value().toString();
        if(!str.isEmpty())
          t.push_back(str.toStdString());
      }

      if(!t.empty())
        ossia::net::set_tags(dat.extended, std::move(t));

      //! \todo handle the other attributes. We should have a map of the "allowed" attributes in the qml api.
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
      addr.set_value_type(ossia::val_type::BOOL);
      break;
    case QVariant::Time:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::ULongLong:
      addr.set_value_type(ossia::val_type::INT);
      break;
    case QVariant::Char:
      addr.set_value_type(ossia::val_type::CHAR);
      break;
    case QVariant::String:
    case QVariant::ByteArray:
      addr.set_value_type(ossia::val_type::STRING);
      break;
    case QVariant::Double:
      addr.set_value_type(ossia::val_type::FLOAT);
      break;
    case QVariant::Color:
      addr.set_unit(ossia::argb_u{});
      break;
    case QVariant::Point:
    case QVariant::PointF:
    case QVariant::Vector2D:
    case QVariant::Size:
    case QVariant::SizeF:
      addr.set_unit(ossia::cartesian_2d_u{});
      break;
    case QVariant::Vector3D:
      addr.set_unit(ossia::cartesian_3d_u{});
      break;
    case QVariant::Vector4D:
      addr.set_unit(ossia::axis_u{});
      break;
    case QVariant::Quaternion:
      addr.set_unit(ossia::quaternion_u{});
      break;
    case QVariant::Line:
    case QVariant::LineF:
    case QVariant::Rect:
    case QVariant::RectF:
      addr.set_value_type(ossia::val_type::VEC4F);
      break;
    case QVariant::List:
    case QVariant::StringList:
    case QVariant::Date:
    default:
      addr.set_value_type(ossia::val_type::TUPLE);
      break;
  }
}

QVariant ossia_to_qvariant::operator()(QVariant::Type type, const value& ossia_val)
{
  switch(type)
  {
    case QVariant::Bool:
      return QVariant::fromValue(convert<bool>(ossia_val));
    case QVariant::Time:
      return QVariant::fromValue(QTime().addMSecs(convert<int>(ossia_val)));
    case QVariant::Int:
      return QVariant::fromValue(convert<int>(ossia_val));
    case QVariant::UInt:
      return QVariant::fromValue((quint32)convert<int>(ossia_val));
    case QVariant::ULongLong:
      return QVariant::fromValue((qlonglong)convert<int>(ossia_val));
    case QVariant::Char:
      return QVariant::fromValue(QChar::fromLatin1(convert<char>(ossia_val)));
    case QVariant::String:
      return QVariant::fromValue(QString::fromStdString(convert<std::string>(ossia_val)));
    case QVariant::ByteArray:
      return QVariant::fromValue(QByteArray::fromStdString(convert<std::string>(ossia_val)));
    case QVariant::Double:
      return QVariant::fromValue(convert<double>(ossia_val));
    case QVariant::Color:
    {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QColor::fromRgbF(val[1], val[2], val[3], val[0]));
    }
    case QVariant::Point:
    {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QPoint(val[0], val[1]));
    }
    case QVariant::PointF:
    {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QPointF(val[0], val[1]));
    }
    case QVariant::Vector2D:
    {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QVector2D(val[0], val[1]));
    }
      break;
    case QVariant::Vector3D:
    {
      auto val = convert<vec3f>(ossia_val);
      return QVariant::fromValue(QVector3D(val[0], val[1], val[2]));
    }
    case QVariant::Vector4D:
    {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QVector4D(val[0], val[1], val[2], val[3]));
    }
    case QVariant::Quaternion:
    {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QQuaternion(val[0], val[1], val[2], val[3]));
    }
    case QVariant::Line:
    {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QLine(val[0], val[1], val[2], val[3]));
    }
    case QVariant::LineF:
    {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QLineF(val[0], val[1], val[2], val[3]));
    }
    case QVariant::Rect:
    {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QRect(val[0], val[1], val[2], val[3]));
    }
    case QVariant::RectF:
    {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QRectF(val[0], val[1], val[2], val[3]));
    }
    case QVariant::Size:
    {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QSize(val[0], val[1]));
    }
    case QVariant::SizeF:
    {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QSizeF(val[0], val[1]));
    }
    case QVariant::List:
    {
      // TODO
    }
    case QVariant::StringList:
    {
      // TODO tuple of string
    }
    case QVariant::Date:
      // TODO double ?
    default:
    {
      // Use the ossia type instead
      return ossia_val.apply(*this);
      break;
    }
  }
  return {};
}

value qt_to_ossia::operator()(const QVariant& v)
{
  switch(v.type())
  {
    case QVariant::Bool:
      return operator()(v.toBool());
    case QVariant::Time:
      return operator()(v.toTime());
    case QVariant::Int:
      return operator()(v.toInt());
    case QVariant::UInt:
      return operator()(v.toUInt());
    case QVariant::ULongLong:
      return operator()(v.toLongLong());
    case QVariant::Char:
      return operator()(v.toChar());
    case QVariant::String:
      return operator()(v.toString());
    case QVariant::ByteArray:
      return operator()(v.toByteArray());
    case QVariant::Double:
      return operator()(v.toDouble());
    case QVariant::Color:
      return operator()(v.value<QColor>());
    case QVariant::Point:
      return operator()(v.toPoint());
    case QVariant::PointF:
      return operator()(v.toPointF());
    case QVariant::Vector2D:
      return operator()(v.value<QVector2D>());
    case QVariant::Size:
      return operator()(v.toSize());
    case QVariant::SizeF:
      return operator()(v.toSizeF());
    case QVariant::Vector3D:
      return operator()(v.value<QVector3D>());
    case QVariant::Vector4D:
      return operator()(v.value<QVector4D>());
    case QVariant::Quaternion:
      return operator()(v.value<QQuaternion>());
    case QVariant::Line:
      return operator()(v.toLine());
    case QVariant::LineF:
      return operator()(v.toLineF());
    case QVariant::Rect:
      return operator()(v.toRect());
    case QVariant::RectF:
      return operator()(v.toRectF());
    case QVariant::List:
      return operator()(v.toList());
    case QVariant::StringList:
      return operator()(v.toStringList());
    case QVariant::Date:
      return operator()(v.toDate());
//    case 1024: // QJSValue -> seems to crash
//      return value_from_jsvalue(v.value<QJSValue>());
    default:
      return operator()();
  }
}


}

}
#endif
