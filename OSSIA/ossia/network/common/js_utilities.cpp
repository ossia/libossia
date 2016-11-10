#include "js_utilities.hpp"
namespace ossia
{
namespace net
{

value js_value_inbound_visitor::operator()(Impulse) const
{
  return Impulse{};
}

value js_value_inbound_visitor::operator()(Int v) const
{
  return ossia::Int(val.toInt());
}

value js_value_inbound_visitor::operator()(Float v) const
{
  return ossia::Float(val.toNumber());
}

value js_value_inbound_visitor::operator()(Bool v) const
{
  return ossia::Bool(val.toBool());
}

value js_value_inbound_visitor::operator()(Char v) const
{
  auto str = val.toString();
  if(str.size() > 0)
    return ossia::Char(str[0].toLatin1());
  return v;
}

value js_value_inbound_visitor::operator()(const String &v) const
{
  return ossia::String(val.toString().toStdString());
}

value js_value_inbound_visitor::operator()(const Tuple &v) const
{
  if(val.isArray())
  {
    Tuple t;

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

value js_value_inbound_visitor::operator()(Vec2f v) const
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

value js_value_inbound_visitor::operator()(Vec3f v) const
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

value js_value_inbound_visitor::operator()(Vec4f v) const
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



generic_address_data make_address_data(const QJSValue &js)
{
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

QJSValue js_value_outbound_visitor::operator()(Impulse) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Impulse));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(Int val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Int));
  v.setProperty("value", int32_t(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(Float val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Float));
  v.setProperty("value", val);
  return v;
}

QJSValue js_value_outbound_visitor::operator()(Bool val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Bool));
  v.setProperty("value", val);
  return v;
}

QJSValue js_value_outbound_visitor::operator()(Char val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Char));
  v.setProperty("value", val);
  return v;
}

QJSValue js_value_outbound_visitor::operator()(const String &val) const
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

QJSValue js_value_outbound_visitor::operator()(const Tuple &val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Tuple));
  v.setProperty("value", make_tuple(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(Vec2f val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Vec2f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(Vec3f val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Vec3f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(Vec4f val) const
{
  QJSValue v;
  v.setProperty("type", to_enum(qml_context::val_type::Vec4f));
  v.setProperty("value", make_array(val));
  return v;
}

QJSValue js_value_outbound_visitor::operator()(const Destination &t) { return {}; }

QJSValue js_value_outbound_visitor::operator()() const { return {}; }




QString js_string_outbound_visitor::operator()(Impulse) const
{
  return QStringLiteral("\"\"");
}

QString js_string_outbound_visitor::operator()(Int val) const
{
  return QString::number(int32_t(val));
}

QString js_string_outbound_visitor::operator()(Float val) const
{
  return QString::number(val);
}

QString js_string_outbound_visitor::operator()(Bool val) const
{
  return val ? QStringLiteral("true") : QStringLiteral("false");
}

QString js_string_outbound_visitor::operator()(Char val) const
{
  return "\"" % QString{val} % "\"";
}

QString js_string_outbound_visitor::operator()(const String &val) const
{
  return "\"" % QString::fromStdString(val) % "\"";
}

QString js_string_outbound_visitor::operator()(const Tuple &val) const
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

QString js_string_outbound_visitor::operator()(Vec2f val) const
{
  return make_array(val);
}

QString js_string_outbound_visitor::operator()(Vec3f val) const
{
  return make_array(val);
}

QString js_string_outbound_visitor::operator()(Vec4f val) const
{
  return make_array(val);
}

QString js_string_outbound_visitor::operator()(const Destination &t) { return (*this)(Impulse{}); }

QString js_string_outbound_visitor::operator()() const { return (*this)(Impulse{}); }

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
    Tuple t;
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




}

}
