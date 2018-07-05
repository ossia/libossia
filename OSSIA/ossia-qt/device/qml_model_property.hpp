
#pragma once
#include <ossia/network/base/parameter.hpp>
#include <wobjectdefs.h>
#include <QAbstractItemModel>
#include <QObject>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <ossia-qt/device/qml_node_base.hpp>
#include <ossia-qt/value_metatypes.hpp>
namespace ossia
{
namespace qt
{
class OSSIA_EXPORT qml_model_property : public QAbstractItemModel
{
  W_OBJECT(qml_model_property)

public:
  qml_model_property(QObject* parent = nullptr);
  ~qml_model_property() override;

public:
  QModelIndex
  index(int row, int column, const QModelIndex& parent) const override;
  QModelIndex parent(const QModelIndex& child) const override;
  int rowCount(const QModelIndex& parent) const override;
  int columnCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  int count() const;
  QString node() const;
  QObject* parentNode() const;
  QObject* device() const;
  auto parentOssiaNode() const { return m_parentOssiaNode; }

  void updateCount();
  void reloadParentNode();
public:
  void setValue_sig(const ossia::value& arg_1) E_SIGNAL(OSSIA_EXPORT, setValue_sig, arg_1);
  void countChanged(int count) E_SIGNAL(OSSIA_EXPORT, countChanged, count);
  void nodeChanged(QString node) E_SIGNAL(OSSIA_EXPORT, nodeChanged, node);
  void parentNodeChanged(QObject* parentNode) E_SIGNAL(OSSIA_EXPORT, parentNodeChanged, parentNode);
  void deviceChanged(QObject* device) E_SIGNAL(OSSIA_EXPORT, deviceChanged, device);

public:
  void setCount(int count); W_SLOT(setCount);
  void setNode(QString node); W_SLOT(setNode);
  void setParentNode(QObject* parentNode); W_SLOT(setParentNode);
  void setDevice(QObject* device); W_SLOT(setDevice);

private:
  void on_device_deleted(QObject*);
  void on_node_deleted(const ossia::net::node_base&);

  QString m_node;

  QObject* m_parentNode{};
  ossia::net::node_base* m_parentOssiaNode{};
  qml_device* m_device{};
  int m_count{};

W_PROPERTY(QObject*, parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged)

W_PROPERTY(QObject*, device READ device WRITE setDevice NOTIFY deviceChanged)

W_PROPERTY(QString, node READ node WRITE setNode NOTIFY nodeChanged)

W_PROPERTY(int, count READ count WRITE setCount NOTIFY countChanged)
};
}
}
