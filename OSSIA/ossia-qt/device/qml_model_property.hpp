
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

public:
  QModelIndex index(int row, int column, const QModelIndex& parent) const override;
  QModelIndex parent(const QModelIndex& child) const override;
  int rowCount(const QModelIndex& parent) const override;
  int columnCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  int count() const;
  QString node() const;
  qml_node_base* parentNode() const;
  QObject* device() const;

signals:
  void setValue_sig(const value&);
  void countChanged(int count);
  void nodeChanged(QString node);
  void parentNodeChanged(qml_node_base* parentNode);
  void deviceChanged(QObject* device);

public slots:
  void setCount(int count);
  void setNode(QString node);
  void setParentNode(qml_node_base* parentNode);
  void setDevice(QObject* device);

private:
  void on_node_deleted(const ossia::net::node_base&);

  QString m_node;

  qml_node_base* m_parentNode{};
  qml_device* m_device{};
  int m_count{};

};

}
}
