
#pragma once
#include <QAbstractItemModel>
#include <QObject>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <ossia/network/base/address.hpp>
#include <ossia-qt/device/qml_node_base.hpp>
namespace ossia
{
namespace qt
{
class qml_model_property :
    public QAbstractItemModel
{
  Q_OBJECT
  Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
  Q_PROPERTY(QString node READ node WRITE setNode NOTIFY nodeChanged)
  Q_PROPERTY(QObject* device READ device WRITE setDevice NOTIFY deviceChanged)
  Q_PROPERTY(qml_node_base* parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged)
public:
  qml_model_property(QObject* parent = nullptr);
  ~qml_model_property();


signals:
  void setValue_sig(const value&);

  void countChanged(int count);

  void nodeChanged(QString node);

  void parentNodeChanged(qml_node_base* parentNode);

  void deviceChanged(QObject* device);

public slots:

  void setCount(int count)
  {
    if (m_count == count)
      return;

    m_count = count;
    emit countChanged(count);
  }

  void setNode(QString node)
  {
    if (m_node == node)
      return;

    m_node = node;
    emit nodeChanged(node);
  }

  void setParentNode(qml_node_base* parentNode)
  {
    if (m_parentNode == parentNode)
      return;

    m_parentNode = parentNode;
    emit parentNodeChanged(parentNode);
  }

  void setDevice(QObject* device)
{
  if (m_device == device)
  return;

m_device = device;
emit deviceChanged(device);
}

private:
  void on_node_deleted(const ossia::net::node_base&);

  QQmlProperty m_targetProperty;

  // QAbstractItemModel interface
  int m_count{};

  QString m_node;

  qml_node_base* m_parentNode;

  QObject* m_device;

public:
  QModelIndex index(int row, int column, const QModelIndex& parent) const override
  {
    return createIndex(row, column);
  }
  QModelIndex parent(const QModelIndex& child) const override
  {
    return {};
  }
  int rowCount(const QModelIndex& parent) const override
  {
    return m_count;
  }
  int columnCount(const QModelIndex& parent) const override
  {
    return 0;
  }
  QVariant data(const QModelIndex& index, int role) const override
  {
    return {};
  }
  int count() const
  {
    return m_count;
  }
  QString node() const
  {
    return m_node;
  }
  qml_node_base* parentNode() const
  {
    return m_parentNode;
  }
  QObject* device() const
  {
    return m_device;
  }
};

}
}
