#pragma once
#include <QQuickItem>
#include <QQmlListProperty>
#include <ossia/editor/scenario/time_interval.hpp>
namespace ossia
{
namespace qt
{
class qml_cond;
class qml_scenario;
class qml_process;

template<typename T>
T findParent(QObject* obj)
{
  while((obj = obj->parent()))
  {
    if(auto ok = qobject_cast<T>(obj))
      return ok;
  }
  return nullptr;
}

class qml_interval : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qint64 nominalDuration READ nominalDuration WRITE setNominalDuration NOTIFY nominalDurationChanged)
    Q_PROPERTY(qint64 minDuration READ minDuration WRITE setMinDuration NOTIFY minDurationChanged)
    Q_PROPERTY(qint64 maxDuration READ maxDuration WRITE setMaxDuration NOTIFY maxDurationChanged)
    Q_PROPERTY(qint64 playDuration READ playDuration WRITE setPlayDuration NOTIFY playDurationChanged)
    Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(qml_cond* follows READ follows WRITE setFollows NOTIFY followsChanged)
    Q_PROPERTY(qml_cond* precedes READ precedes WRITE setPrecedes NOTIFY precedesChanged)
  public:
    qml_interval(QQuickItem* parent = nullptr);
    ~qml_interval() override;

    qint64 nominalDuration() const;
    qint64 minDuration() const;
    qint64 maxDuration() const;
    qint64 playDuration() const;
    qml_cond* follows() const;
    qml_cond* precedes() const;

    std::shared_ptr<ossia::time_interval> interval() const;
    double speed() const;

    void setup();
    void registerProcess(qml_process*);
    void unregisterProcess(qml_process*);
    std::shared_ptr<ossia::time_interval> m_interval;
public slots:
    void setNominalDuration(qint64 nominalDuration);
    void setMinDuration(qint64 minDuration);
    void setMaxDuration(qint64 maxDuration);
    void setPlayDuration(qint64 playDuration);
    void setFollows(qml_cond* follows);
    void setPrecedes(qml_cond* precedes);
    void setSpeed(double speed);

    void play();
    void pause();
    void stop();

signals:
    void nominalDurationChanged(qint64 nominalDuration);
    void minDurationChanged(qint64 minDuration);
    void maxDurationChanged(qint64 maxDuration);
    void playDurationChanged(qint64 playDuration);
    void followsChanged(qml_cond* follows);
    void precedesChanged(qml_cond* precedes);
    void speedChanged(double speed);

private:
    void reset();
    qint64 m_nominalDuration{};
    qint64 m_minDuration{};
    qint64 m_maxDuration{};
    qint64 m_playDuration{};
    qml_cond* m_follows{};
    qml_cond* m_precedes{};
    qml_process* m_parent{};

    double m_speed;
};
}
}
