#pragma once
#include <QtQml>
namespace ossia
{
namespace qt
{
/*
class qml_impulse :
        public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString node READ node WRITE setNode NOTIFY nodeChanged)
public:
    qml_impulse(QObject* parent = nullptr);
    virtual ~qml_impulse();

    QString node() const;

public slots:
    void setNode(QString node);

signals:
    void nodeChanged(QString node);
    void impulse();

private:
    QString m_node;
    std::shared_ptr<OSSIA::Node> m_ossia_node{};
    std::shared_ptr<OSSIA::Address> m_address{};
};
*/
}
}
