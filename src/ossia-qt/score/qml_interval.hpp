#pragma once
#include <QQuickItem>
#include <wobjectdefs.h>
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
    W_OBJECT(qml_interval)
    
    
    
    
    
    
    
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
public:
    void setNominalDuration(qint32 nominalDuration); W_SLOT(setNominalDuration);
    void setMinDuration(qint32 minDuration); W_SLOT(setMinDuration);
    void setMaxDuration(qint32 maxDuration); W_SLOT(setMaxDuration);
    void setPlayDuration(qint32 playDuration); W_SLOT(setPlayDuration);
    void setFollows(qml_cond* follows); W_SLOT(setFollows);
    void setPrecedes(qml_cond* precedes); W_SLOT(setPrecedes);
    void setFollows(qml_sync* follows); W_SLOT(setFollows);
    void setPrecedes(qml_sync* precedes); W_SLOT(setPrecedes);
    void setFollows(QObject* follows); W_SLOT(setFollows);
    void setPrecedes(QObject* precedes); W_SLOT(setPrecedes);
    void setSpeed(double speed); W_SLOT(setSpeed);

    void play(); W_SLOT(play);
    void pause(); W_SLOT(pause);
    void stop(); W_SLOT(stop);

public:
    void nominalDurationChanged(qint32 nominalDuration) E_SIGNAL(OSSIA_EXPORT, nominalDurationChanged, nominalDuration);
    void minDurationChanged(qint32 minDuration) E_SIGNAL(OSSIA_EXPORT, minDurationChanged, minDuration);
    void maxDurationChanged(qint32 maxDuration) E_SIGNAL(OSSIA_EXPORT, maxDurationChanged, maxDuration);
    void playDurationChanged(qint32 playDuration) E_SIGNAL(OSSIA_EXPORT, playDurationChanged, playDuration);
    void followsChanged(qml_cond* follows) E_SIGNAL(OSSIA_EXPORT, followsChanged, follows);
    void precedesChanged(qml_cond* precedes) E_SIGNAL(OSSIA_EXPORT, precedesChanged, precedes);
    void speedChanged(double speed) E_SIGNAL(OSSIA_EXPORT, speedChanged, speed);

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

W_PROPERTY(QObject*, precedes READ precedes WRITE setPrecedes NOTIFY precedesChanged)

W_PROPERTY(QObject*, follows READ follows WRITE setFollows NOTIFY followsChanged)

W_PROPERTY(double, speed READ speed WRITE setSpeed NOTIFY speedChanged)

W_PROPERTY(qint32, playDuration READ playDuration WRITE setPlayDuration NOTIFY playDurationChanged)

W_PROPERTY(qint32, maxDuration READ maxDuration WRITE setMaxDuration NOTIFY maxDurationChanged)

W_PROPERTY(qint32, minDuration READ minDuration WRITE setMinDuration NOTIFY minDurationChanged)

W_PROPERTY(qint32, nominalDuration READ nominalDuration WRITE setNominalDuration NOTIFY nominalDurationChanged)
};
}
}
