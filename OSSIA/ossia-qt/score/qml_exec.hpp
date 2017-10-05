#pragma once
#include <QQuickItem>
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
    Q_OBJECT
  public:
    static qml_exec& instance();
    static qml_exec* get(QObject* obj);
    void submitCommand(std::function<void()>);
  public slots:
    void play(qml_interval* itvl);
    void pause(qml_interval* itvl);
    void stop(qml_interval* itvl);

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
