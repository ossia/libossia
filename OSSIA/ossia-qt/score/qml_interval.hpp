#pragma once
#include <QQuickItem>
#include <QQmlListProperty>
#include <ossia-qt/score/qml_sync.hpp>
#include <ossia-qt/score/qml_cond.hpp>
#include <ossia-qt/score/qml_util.hpp>
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
    Q_PROPERTY(qint32 nominalDuration READ nominalDuration WRITE setNominalDuration NOTIFY nominalDurationChanged)
    Q_PROPERTY(qint32 minDuration READ minDuration WRITE setMinDuration NOTIFY minDurationChanged)
    Q_PROPERTY(qint32 maxDuration READ maxDuration WRITE setMaxDuration NOTIFY maxDurationChanged)
    Q_PROPERTY(qint32 playDuration READ playDuration WRITE setPlayDuration NOTIFY playDurationChanged)
    Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(QObject* follows READ follows WRITE setFollows NOTIFY followsChanged)
    Q_PROPERTY(QObject* precedes READ precedes WRITE setPrecedes NOTIFY precedesChanged)
  public:
    qml_interval(QQuickItem* parent = nullptr);
    ~qml_interval() override;

    qint32 nominalDuration() const;
    qint32 minDuration() const;
    qint32 maxDuration() const;
    qint32 playDuration() const;
    qml_cond* follows() const;
    qml_cond* precedes() const;

    std::shared_ptr<ossia::time_interval> interval() const;
    double speed() const;

    void setup();
    void registerProcess(qml_process*);
    void unregisterProcess(qml_process*);
    std::shared_ptr<ossia::time_interval> m_interval;
public Q_SLOTS:
    void setNominalDuration(qint32 nominalDuration);
    void setMinDuration(qint32 minDuration);
    void setMaxDuration(qint32 maxDuration);
    void setPlayDuration(qint32 playDuration);
    void setFollows(qml_cond* follows);
    void setPrecedes(qml_cond* precedes);
    void setFollows(qml_sync* follows);
    void setPrecedes(qml_sync* precedes);
    void setFollows(QObject* follows);
    void setPrecedes(QObject* precedes);
    void setSpeed(double speed);

    void play();
    void pause();
    void stop();

signals:
    void nominalDurationChanged(qint32 nominalDuration);
    void minDurationChanged(qint32 minDuration);
    void maxDurationChanged(qint32 maxDuration);
    void playDurationChanged(qint32 playDuration);
    void followsChanged(qml_cond* follows);
    void precedesChanged(qml_cond* precedes);
    void speedChanged(double speed);

private:
    void reset();
    qint32 m_nominalDuration{};
    qint32 m_minDuration{0};
    qint32 m_maxDuration{ossia::qt::infinite()};
    qint32 m_playDuration{};
    qml_cond* m_follows{};
    qml_cond* m_precedes{};
    qml_process* m_parent{};

    double m_speed;
};
}
}
