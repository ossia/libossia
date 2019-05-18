#pragma once
#include <ossia/network/base/parameter.hpp>
#include <wobjectdefs.h>
#include <QObject>
#include <QQmlExpression>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <QQmlScriptString>
#include <ossia-qt/device/qml_node_base.hpp>
#include <ossia-qt/qml_context.hpp>
#include <ossia-qt/value_metatypes.hpp>
namespace ossia
{
namespace qt
{
class qml_node;
class qml_device;
class qml_property_context : public QObject
{
  W_OBJECT(qml_property_context)
public:
  qml_property_context(
      QQmlProperty& p, ossia::net::parameter_base& addr, QObject* parent);

  QQmlProperty& targetProperty;
  ossia::net::parameter_base& address;
public:
  void qtVariantChanged(); W_SLOT(qtVariantChanged);
};

class qml_property_reader : public qml_node_base,
                            public QQmlPropertyValueSource
{
  W_OBJECT(qml_property_reader)
  W_INTERFACE(QQmlPropertyValueSource)

public:
  qml_property_reader(QQuickItem* parent = nullptr);
  ~qml_property_reader() override;

  void setTarget(const QQmlProperty& prop) override;
  void setDevice(QObject* device) override;
  void resetNode() override;

private:
  void setupAddress(bool reading);
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  QQmlProperty m_targetProperty;
  ossia::net::parameter_base* m_param{};
  qml_property_context* m_propCtx{};
};

class qml_property_writer : public qml_node_base,
                            public QQmlPropertyValueSource
{
  W_OBJECT(qml_property_writer)
  W_INTERFACE(QQmlPropertyValueSource)

public:
  qml_property_writer(QQuickItem* parent = nullptr);
  ~qml_property_writer() override;

  void setTarget(const QQmlProperty& prop) override;
  void setDevice(QObject* device) override;
  void resetNode() override;

public:
  void setValue_sig(const value& arg_1) E_SIGNAL(OSSIA_EXPORT, setValue_sig, arg_1);

private:
  void setValue_slot(const value&); W_SLOT(setValue_slot);

private:
  void setupAddress(bool reading);
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  QQmlProperty m_targetProperty;
  ossia::net::parameter_base* m_param{};
  ossia::net::parameter_base::iterator m_cb;
};

class qml_binding : public qml_node_base
{
  W_OBJECT(qml_binding)

public:
  qml_binding(QQuickItem* parent = nullptr);
  ~qml_binding() override;

  void setDevice(QObject* device) override;
  void resetNode() override;

  QQmlScriptString on() const;

public:
  void setOn(QQmlScriptString on); W_SLOT(setOn);

public:
  void onChanged(QQmlScriptString on) E_SIGNAL(OSSIA_EXPORT, onChanged, on);

private:
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  std::unique_ptr<QQmlExpression> m_expr;
  ossia::net::parameter_base* m_param{};
  QQmlScriptString m_on;

W_PROPERTY(QQmlScriptString, on READ on WRITE setOn NOTIFY onChanged)
};

class qml_callback : public qml_node_base
{
  W_OBJECT(qml_callback)

public:
  qml_callback(QQuickItem* parent = nullptr);
  ~qml_callback() override;

  void setDevice(QObject* device) override;
  void resetNode() override;

  QVariant value() const;

public:
  void valueChanged(QVariant arg_1) E_SIGNAL(OSSIA_EXPORT, valueChanged, arg_1);
  void setValue_sig(const ossia::value& arg_1) E_SIGNAL(OSSIA_EXPORT, setValue_sig, arg_1);

public:
  void setValue(QVariant value); W_SLOT(setValue);
  void setValue_slot(const ossia::value&); W_SLOT(setValue_slot);

private:
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  ossia::net::parameter_base* m_param{};
  QVariant m_value;
  ossia::net::parameter_base::iterator m_cb;

W_PROPERTY(QVariant, value READ value WRITE setValue NOTIFY valueChanged)
};
}
}

W_REGISTER_ARGTYPE(QQmlScriptString)
