#pragma once
#include <QQuickItem>
#include <wobjectdefs.h>
#include <QPointer>
#include <readerwriterqueue.h>


#include <ossia-qt/score/qml_interval.hpp>
#include <ossia-qt/score/qml_util.hpp>

#include <ossia/editor/scenario/clock.hpp>
namespace ossia
{
namespace qt
{

class qml_exec : public QObject
{
    W_OBJECT(qml_exec)
  public:
    static qml_exec& instance();
    static qml_exec* get(QObject* obj);
    void submitCommand(std::function<void()>);
  public:
    void play(qml_interval* itvl); W_SLOT(play);
    void pause(qml_interval* itvl); W_SLOT(pause);
    void stop(qml_interval* itvl); W_SLOT(stop);

  private:
    qml_exec();
    ~qml_exec() override;

    void timerEvent(QTimerEvent* event) override;

    QPointer<qml_interval> m_cur{};
    moodycamel::ReaderWriterQueue<std::function<void()>> m_queue;
    optional<int> m_timer;
    std::chrono::high_resolution_clock::time_point m_cur_t;

};

}
}
