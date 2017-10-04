#pragma once
#include <ossia-qt/score/qml_interval.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/automation/automation.hpp>
#include <ossia-qt/device/qml_node_base.hpp>
#include <QQuickItem>
#include <QPointer>
#include <QEasingCurve>

namespace ossia
{
namespace qt
{
class qml_interval;
class qml_process : public QQuickItem
{
    Q_OBJECT
  public:
    qml_process(QQuickItem* parent = nullptr);
    ~qml_process() override;

    qml_interval* interval() const;
    virtual void setup();

    virtual std::shared_ptr<ossia::time_process> process() const = 0;

  protected:
    void reset();
    virtual void reset_impl();

    QPointer<qml_interval> m_cur_parent{};
};

class qml_breakpoint : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QEasingCurve::Type type READ type WRITE setType NOTIFY typeChanged)
  public: using QQuickItem::QQuickItem;
    std::function<float(double,float,float)> segment();

    QEasingCurve::Type type() const { return m_type; }

  signals:
    void typeChanged(QEasingCurve::Type);

  public slots:
    void setType(QEasingCurve::Type t) { if(m_type != t) { m_type = t; emit typeChanged(t); } }

  private:
    QEasingCurve::Type m_type{QEasingCurve::Linear};
};

class qml_autom : public qml_process
{
    Q_OBJECT

    Q_PROPERTY(QVariant target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(double xMin READ xMin WRITE setXMin NOTIFY xMinChanged)
    Q_PROPERTY(double xMax READ xMax WRITE setXMax NOTIFY xMaxChanged)
    Q_PROPERTY(double yMin READ yMin WRITE setYMin NOTIFY yMinChanged)
    Q_PROPERTY(double yMax READ yMax WRITE setYMax NOTIFY yMaxChanged)
  public:
    qml_autom(QQuickItem* parent = nullptr);
    ~qml_autom() override;
    void setup() override;

    QVariant target() const;
    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;
    std::shared_ptr<ossia::time_process> process() const override;
  public slots:
    void setTarget(QVariant target);
    void setXMin(double xMin);
    void setXMax(double xMax);
    void setYMin(double yMin);
    void setYMax(double yMax);

  signals:
    void targetChanged(QVariant target);
    void xMinChanged(double xMin);
    void xMaxChanged(double xMax);
    void yMinChanged(double yMin);
    void yMaxChanged(double yMax);

  private:
    void reset_impl() override;
    std::shared_ptr<ossia::automation> m_impl;
    QVariant m_target;
    double m_xMin{0.};
    double m_xMax{1.};
    double m_yMin{0.};
    double m_yMax{1.};
    QMetaObject::Connection m_death;
};
}
}
