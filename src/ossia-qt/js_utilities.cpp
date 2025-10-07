// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#if defined(QT_CORE_LIB)
#include "js_utilities.hpp"

#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/value/value_conversion.hpp>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#if __has_include(<QJSValue>)
#include <QJSValue>
#endif

namespace ossia::qt
{

#if defined(QT_QML_LIB)
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

value js_value_inbound_visitor::operator()(const std::string& v) const
{
  return val.toString().toStdString();
}

value js_value_inbound_visitor::operator()(const std::vector<ossia::value>& v) const
{
  std::vector<ossia::value> t;
  if(val.isArray())
  {
    QJSValueIterator it(val);
    while(it.hasNext())
    {
      it.next();
      t.push_back(value_from_js(it.value()));
    }
  }
  else
  {
    t = v;
  }
  return t;
}

value js_value_inbound_visitor::operator()(const value_map_type& v) const
{
  value_map_type t;
  if(val.isObject())
  {
    QJSValueIterator it(val);
    while(it.hasNext())
    {
      it.next();
      t.emplace_back(it.name().toStdString(), value_from_js(it.value()));
    }
  }
  else
  {
    t = v;
  }
  return t;
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
      i++;
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
      i++;
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
      i++;
    }
  }
  return v;
}
value js_value_inbound_visitor::operator()() const
{
  return {};
}
#endif

value variant_inbound_visitor::operator()(impulse) const
{
  return impulse{};
}

value variant_inbound_visitor::operator()(int32_t v) const
{
  return int32_t(val.toInt());
}

value variant_inbound_visitor::operator()(float v) const
{
  return float(val.toFloat());
}

value variant_inbound_visitor::operator()(bool v) const
{
  return bool(val.toBool());
}

value variant_inbound_visitor::operator()(char v) const
{
  return val.toChar().toLatin1();
}

value variant_inbound_visitor::operator()(const std::string& v) const
{
  return val.toString().toStdString();
}

value variant_inbound_visitor::operator()(const std::vector<ossia::value>& v) const
{
  auto qv = val.toList();
  std::vector<ossia::value> t;
  t.reserve(qv.size());
  for(auto& e : qv)
  {
    t.push_back(ossia::qt::qt_to_ossia{}(e));
  }

  return t;
}

value variant_inbound_visitor::operator()(vec2f v) const
{
  if(val.canConvert<QVector2D>())
    return qt_to_ossia{}(val.value<QVector2D>());
  else if(val.canConvert<QPoint>())
    return qt_to_ossia{}(val.value<QPoint>());
  else if(val.canConvert<QPointF>())
    return qt_to_ossia{}(val.value<QPointF>());
  else if(val.canConvert<QSize>())
    return qt_to_ossia{}(val.value<QSize>());
  else if(val.canConvert<QSizeF>())
    return qt_to_ossia{}(val.value<QSizeF>());
  return ossia::vec2f{};
}

value variant_inbound_visitor::operator()(vec3f v) const
{
  if(val.canConvert<QVector3D>())
    return qt_to_ossia{}(val.value<QVector3D>());
  return ossia::vec3f{};
}

value variant_inbound_visitor::operator()(vec4f v) const
{
  if(val.canConvert<QVector4D>())
    return qt_to_ossia{}(val.value<QVector4D>());
  if(val.canConvert<QColor>())
    return qt_to_ossia{}(val.value<QColor>());
  if(val.canConvert<QQuaternion>())
    return qt_to_ossia{}(val.value<QQuaternion>());
  if(val.canConvert<QLine>())
    return qt_to_ossia{}(val.value<QLine>());
  if(val.canConvert<QLineF>())
    return qt_to_ossia{}(val.value<QLineF>());
  if(val.canConvert<QRect>())
    return qt_to_ossia{}(val.value<QRect>());
  if(val.canConvert<QRectF>())
    return qt_to_ossia{}(val.value<QRectF>());
  return ossia::vec4f{};
}
value variant_inbound_visitor::operator()() const
{
  return ossia::impulse{};
}

#if defined(QT_QML_LIB)
ossia::complex_type get_type(const QJSValue& val)
{
  // TODO handle other cases ? string, extended, etc...
  auto opt_t = get_enum<ossia::val_type>(val);
  if(opt_t)
  {
    return *opt_t;
  }
  else if(val.isString())
  {
    return ossia::extended_type(val.toString().toStdString());
  }
  else
  {
    return complex_type{};
  }
}

net::parameter_data make_parameter_data(const QJSValue& js)
{
  using namespace ossia::net;
  parameter_data dat;

  QJSValue name = js.property("name");
  if(name.isString())
  {
    dat.name = name.toString().toStdString();
  }
  else
  {
    return dat;
  }

  if(js.hasProperty("type"))
  {
    dat.type = get_type(js.property("type"));
  }
  else if(js.hasProperty("unit"))
  {
    dat.unit = ossia::parse_pretty_unit(js.property("unit").toString().toStdString());
    dat.type = dat.unit;
  }

  if(dat.type)
  {
    ossia::val_type base = ossia::underlying_type(dat.type);
    auto base_v = init_value(base);
    auto domain = init_domain(base);
    if(js.hasProperty("values"))
    {
      auto val = value_from_js(js.property("values"));
      if(auto vec = val.target<std::vector<ossia::value>>())
        set_values(domain, *vec);
    }

    if(js.hasProperty("min") || js.hasProperty("max"))
    {
      set_min(domain, value_from_js(base_v, js.property("min")));
      set_max(domain, value_from_js(base_v, js.property("max")));
    }

    dat.value = value_from_js(base_v, js.property("value"));
    if(!dat.value.valid())
      dat.value = base_v;

    dat.domain = domain;
    dat.access = get_enum<ossia::access_mode>(js.property("access"));
    dat.bounding = get_enum<ossia::bounding_mode>(js.property("bounding"));
    dat.disabled = js.property("disabled").toBool();
    dat.muted = js.property("muted").toBool();
    dat.critical = js.property("critical").toBool();
    dat.rep_filter
        = get_enum<ossia::repetition_filter>(js.property("repetition_filter"));

    if(!dat.unit)
      dat.unit = ossia::parse_pretty_unit(js.property("unit").toString().toStdString());

    if(js.hasProperty("description"))
      ossia::net::set_description(
          dat.extended, js.property("description").toString().toStdString());

    if(js.hasProperty("extended_type"))
      ossia::net::set_extended_type(
          dat.extended, js.property("extended_type").toString().toStdString());

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

      //! \todo handle the other attributes. We should have a map of the
      //! "allowed" attributes in the qml api.
    }
  }

  return dat;
}

QJSValue js_value_outbound_visitor::to_enum(qml_val_type::val_type t) const
{
  return engine.toScriptValue(QVariant::fromValue(t));
}

QJSValue js_value_outbound_visitor::operator()(impulse) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::Impulse));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(int32_t val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::Int));
  v.setProperty("value", int32_t(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(float val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::Float));
  v.setProperty("value", val);
  return v;
}

QJSValue js_value_outbound_visitor::operator()(bool val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::Bool));
  v.setProperty("value", val);
  return v;
}

QJSValue js_value_outbound_visitor::operator()(const std::string& val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::String));
  v.setProperty("value", QString::fromStdString(val));
  return v;
}

QJSValue js_value_outbound_visitor::make_list(const std::vector<value>& arr) const
{
  auto array = engine.newArray(arr.size());
  int i = 0;

  for(const auto& child : arr)
  {
    array.setProperty(i++, value_to_js_value(child, engine));
  }

  return array;
}

QJSValue
js_value_outbound_visitor::operator()(const std::vector<ossia::value>& val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::List));
  v.setProperty("value", make_list(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(const value_map_type& val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::Map));
  {
    auto object = engine.newObject();

    for(const auto& [key, child] : val)
    {
      object.setProperty(QString::fromStdString(key), value_to_js_value(child, engine));
    }

    v.setProperty("value", object);
  }
  return v;
}

QJSValue js_value_outbound_visitor::operator()(vec2f val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::Vec2f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(vec3f val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::Vec3f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(vec4f val) const
{
  QJSValue v = engine.newObject();
  v.setProperty("type", to_enum(qml_val_type::val_type::Vec4f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()() const
{
  return {};
}

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

QString js_string_outbound_visitor::operator()(const std::string& val) const
{
  return "\"" % QString::fromStdString(val) % "\"";
}

QString
js_string_outbound_visitor::operator()(const std::vector<ossia::value>& val) const
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

QString js_string_outbound_visitor::operator()(const value_map_type& val) const
{
  QString s = "{";

  std::size_t n = val.size();
  if(n != 0)
  {
    auto it = val.begin();
    s += QString{"\"%1\": "}.arg(QLatin1String(it->first.data(), it->first.size()));
    s += value_to_js_string(it->second);

    for(++it; it != val.end(); ++it)
    {
      s += QString{", \"%1\": "}.arg(QLatin1String(it->first.data(), it->first.size()));
      s += value_to_js_string(it->second);
    }
  }

  s += "}";
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

QString js_string_outbound_visitor::operator()() const
{
  return (*this)(impulse{});
}

QString js_string_unquoted_outbound_visitor::operator()(char val) const
{
  return QString{val};
}

QString js_string_unquoted_outbound_visitor::operator()(const std::string& val) const
{
  return QString::fromStdString(val);
}

value value_from_js(const QJSValue& v)
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
      it.next();
      if(it.hasNext()) // we don't want to copy the last "length" property
      {
        t.push_back(value_from_js(it.value()));
      }
    }
    return t;
  }
  // OPTIMIZEME
  // else if(v.isObject())
  // {
  //   return qt_to_ossia{}(v.toVariant());
  // }
  else
  {
    return qt_to_ossia{}(v.toVariant());
  }
}
#endif

void set_parameter_type(QMetaType::Type type, net::parameter_base& addr)
{
  switch(type)
  {
    case QMetaType::Bool:
      addr.set_value_type(ossia::val_type::BOOL);
      break;
    case QMetaType::QTime:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Short:
    case QMetaType::UShort:
    case QMetaType::Long:
    case QMetaType::ULong:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Char:
    case QMetaType::UChar:
    case QMetaType::SChar:
      addr.set_value_type(ossia::val_type::INT);
      break;
    case QMetaType::QString:
    case QMetaType::QByteArray:
    case QMetaType::QUrl:
      addr.set_value_type(ossia::val_type::STRING);
      break;
    case QMetaType::Float:
    case QMetaType::Double:
      addr.set_value_type(ossia::val_type::FLOAT);
      break;
    case QMetaType::QColor:
      addr.set_unit(ossia::argb_u{});
      break;
    case QMetaType::QPoint:
    case QMetaType::QPointF:
    case QMetaType::QVector2D:
    case QMetaType::QSize:
    case QMetaType::QSizeF:
      addr.set_unit(ossia::cartesian_2d_u{});
      break;
    case QMetaType::QVector3D:
      addr.set_unit(ossia::cartesian_3d_u{});
      break;
    case QMetaType::QVector4D:
      addr.set_unit(ossia::axis_u{});
      break;
    case QMetaType::QQuaternion:
      addr.set_unit(ossia::quaternion_u{});
      break;
    case QMetaType::QLine:
    case QMetaType::QLineF:
    case QMetaType::QRect:
    case QMetaType::QRectF:
      addr.set_value_type(ossia::val_type::VEC4F);
      break;

    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QJsonObject:
      addr.set_value_type(ossia::val_type::MAP);
      break;

    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    case QMetaType::QByteArrayList:
    case QMetaType::QJsonArray:
    case QMetaType::QDate:
    default:
      addr.set_value_type(ossia::val_type::LIST);
      break;
  }
}

QVariant ossia_to_qvariant::operator()(QMetaType::Type type, const value& ossia_val)
{
  switch(type)
  {
    case QMetaType::Bool:
      return QVariant::fromValue(convert<bool>(ossia_val));
    case QMetaType::QTime:
      return QVariant::fromValue(QTime().addMSecs(convert<int>(ossia_val)));
    case QMetaType::Short:
      return QVariant::fromValue((qint16)convert<int>(ossia_val));
    case QMetaType::UShort:
      return QVariant::fromValue((quint16)convert<int>(ossia_val));
    case QMetaType::Int:
      return QVariant::fromValue((qint32)convert<int>(ossia_val));
    case QMetaType::UInt:
      return QVariant::fromValue((quint32)convert<int>(ossia_val));
    case QMetaType::LongLong:
      return QVariant::fromValue((qlonglong)convert<int>(ossia_val));
    case QMetaType::ULongLong:
      return QVariant::fromValue((qulonglong)convert<int>(ossia_val));
    case QMetaType::QChar:
      return QVariant::fromValue(QChar::fromLatin1(convert<int>(ossia_val)));
    case QMetaType::Char:
      return QVariant::fromValue(char(convert<int>(ossia_val)));
    case QMetaType::UChar:
      return QVariant::fromValue((unsigned char)(convert<int>(ossia_val)));
    case QMetaType::SChar:
      return QVariant::fromValue((signed char)(convert<int>(ossia_val)));
    case QMetaType::QString:
      return QVariant::fromValue(
          QString::fromStdString(convert<std::string>(ossia_val)));
    case QMetaType::QUrl:
      return QVariant::fromValue(
          QUrl{QString::fromStdString(convert<std::string>(ossia_val))});
    case QMetaType::QByteArray:
      return QVariant::fromValue(
          QByteArray::fromStdString(convert<std::string>(ossia_val)));
    case QMetaType::Float:
      return QVariant::fromValue(convert<float>(ossia_val));
    case QMetaType::Double:
      return QVariant::fromValue(convert<double>(ossia_val));
    case QMetaType::QColor: {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QColor::fromRgbF(val[1], val[2], val[3], val[0]));
    }
    case QMetaType::QPoint: {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QPoint(val[0], val[1]));
    }
    case QMetaType::QPointF: {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QPointF(val[0], val[1]));
    }
    case QMetaType::QVector2D: {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QVector2D(val[0], val[1]));
    }
    break;
    case QMetaType::QVector3D: {
      auto val = convert<vec3f>(ossia_val);
      return QVariant::fromValue(QVector3D(val[0], val[1], val[2]));
    }
    case QMetaType::QVector4D: {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QVector4D(val[0], val[1], val[2], val[3]));
    }
    case QMetaType::QQuaternion: {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QQuaternion(val[0], val[1], val[2], val[3]));
    }
    case QMetaType::QLine: {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QLine(val[0], val[1], val[2], val[3]));
    }
    case QMetaType::QLineF: {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QLineF(val[0], val[1], val[2], val[3]));
    }
    case QMetaType::QRect: {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QRect(val[0], val[1], val[2], val[3]));
    }
    case QMetaType::QRectF: {
      auto val = convert<vec4f>(ossia_val);
      return QVariant::fromValue(QRectF(val[0], val[1], val[2], val[3]));
    }
    case QMetaType::QSize: {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QSize(val[0], val[1]));
    }
    case QMetaType::QSizeF: {
      auto val = convert<vec2f>(ossia_val);
      return QVariant::fromValue(QSizeF(val[0], val[1]));
    }
    case QMetaType::QVariantList: {
      auto val = convert<std::vector<ossia::value>>(ossia_val);
      QVariantList vars;
      vars.reserve(val.size());
      for(auto& v : val)
      {
        vars.push_back(v.apply(ossia_to_qvariant{}));
      }
      return vars;
    }
    case QMetaType::QStringList: {
      auto val = convert<std::vector<ossia::value>>(ossia_val);
      QStringList vars;
      vars.reserve(val.size());
      for(auto& v : val)
      {
        vars.push_back(QString::fromStdString(convert<std::string>(v)));
      }
      return vars;
    }
    case QMetaType::QByteArrayList: {
      auto val = convert<std::vector<ossia::value>>(ossia_val);
      QByteArrayList vars;
      vars.reserve(val.size());
      for(auto& v : val)
      {
        vars.push_back(QByteArray::fromStdString(convert<std::string>(v)));
      }
      return QVariant::fromValue(std::move(vars));
    }
    case QMetaType::QDate:
    // TODO double ?
    default: {
      // Use the ossia type instead
      return ossia_val.apply(*this);
    }
  }
  return {};
}

value qt_to_ossia::operator()(const QVariant& v)
{
  switch(v.typeId())
  {
    case QMetaType::QTime:
      return operator()(v.toTime());
    case QMetaType::Bool:
      return operator()(v.toBool());
    case QMetaType::Int:
      return operator()(v.toInt());
    case QMetaType::UInt:
      return operator()(v.toUInt());
    case QMetaType::LongLong:
      return operator()(v.toLongLong());
    case QMetaType::ULongLong:
      return operator()(v.toLongLong());
    case QMetaType::Double:
      return operator()(v.toDouble());
    case QMetaType::Long:
      return operator()(v.toLongLong());
    case QMetaType::Short:
      return operator()(v.toInt());
    case QMetaType::ULong:
      return operator()(v.toLongLong());
    case QMetaType::UShort:
      return operator()(v.toInt());
    case QMetaType::Char:
      return operator()((char)v.value<char>());
    case QMetaType::UChar:
      return operator()((char)v.value<uchar>());
    case QMetaType::SChar:
      return operator()((char)v.value<signed char>());
    case QMetaType::Nullptr:
    case QMetaType::Void:
      return {};
    case QMetaType::QChar:
      return operator()(v.toChar());
    case QMetaType::QUrl:
      return operator()(v.toString());
    case QMetaType::QString:
      return operator()(v.toString());
    case QMetaType::QByteArray:
      return operator()(v.toByteArray());
    case QMetaType::Float:
      return operator()(v.toFloat());
    case QMetaType::QColor:
      return operator()(v.value<QColor>());
    case QMetaType::QPoint:
      return operator()(v.toPoint());
    case QMetaType::QPointF:
      return operator()(v.toPointF());
    case QMetaType::QVector2D:
      return operator()(v.value<QVector2D>());
    case QMetaType::QSize:
      return operator()(v.toSize());
    case QMetaType::QSizeF:
      return operator()(v.toSizeF());
    case QMetaType::QVector3D:
      return operator()(v.value<QVector3D>());
    case QMetaType::QVector4D:
      return operator()(v.value<QVector4D>());
    case QMetaType::QQuaternion:
      return operator()(v.value<QQuaternion>());
    case QMetaType::QLine:
      return operator()(v.toLine());
    case QMetaType::QLineF:
      return operator()(v.toLineF());
    case QMetaType::QRect:
      return operator()(v.toRect());
    case QMetaType::QRectF:
      return operator()(v.toRectF());
    case QMetaType::QVariantList:
      return operator()(v.toList());
    case QMetaType::QVariantMap:
      return operator()(v.toMap());
    case QMetaType::QVariantHash:
      return operator()(v.toHash());
    case QMetaType::QJsonArray:
      return operator()(v.value<QJsonArray>().toVariantList());
    case QMetaType::QJsonObject:
      return operator()(v.value<QJsonObject>().toVariantMap());
    case QMetaType::QJsonValue:
      return operator()(v.value<QJsonValue>().toVariant());
    case QMetaType::QStringList:
      return operator()(v.toStringList());
    case QMetaType::QByteArrayList:
      return operator()(v.value<QByteArrayList>());
    case QMetaType::QDate:
      return operator()(v.toDate());

    default: {
#if __has_include(<QJSValue>)
      const auto tid = v.typeId();
      static const auto qjsvalue_tid = qMetaTypeId<QJSValue>();
      static const auto qjsvaluelist_tid = qMetaTypeId<QJSValueList>();
      if(tid == qjsvalue_tid)
      {
        return value_from_js(v.value<QJSValue>());
      }
      else if(tid == qjsvaluelist_tid)
      {
        auto in_values = v.value<QList<QJSValue>>();
        std::vector<ossia::value> out_values;
        for(auto& in_val : in_values)
          out_values.push_back(value_from_js(in_val));
        return out_values;
      }
      else
      {
        qDebug() << "QVariant -> ossia::value: Unknown typeid! " << v.typeId()
                 << v.typeName();
      }
#endif
      return operator()();
    }
  }
}
}
#endif

QDebug operator<<(QDebug s, const ossia::value& v)
{
  return s << QString::fromStdString(ossia::value_to_pretty_string(v));
}
