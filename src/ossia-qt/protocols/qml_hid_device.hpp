#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_PROTOCOL_HID)
#include <ossia/protocols/hid/hid_device.hpp>

#include <ossia-qt/invoke.hpp>

#include <QByteArray>
#include <QJSValue>
#include <QObject>
#include <QPointer>
#include <QQmlEngine>

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <verdigris>

namespace ossia::qt
{

inline QByteArray bytes_from_js(const QJSValue& v)
{
  if(v.isArray())
  {
    QByteArray res;
    const int n = v.property("length").toInt();
    res.reserve(n);
    for(int i = 0; i < n; i++)
      res.append(char(v.property(i).toInt()));
    return res;
  }
  if(v.isString())
    return v.toString().toUtf8();
  return v.toVariant().toByteArray();
}

class qml_hid_device : public QObject
{
  W_OBJECT(qml_hid_device)
public:
  //! Shared with the reader and writer threads, which may outlive the QObject
  //! by the time it takes them to stop.
  struct state
  {
    ossia::net::hid_device device;

    std::mutex qt_mutex;
    qml_hid_device* self{};

    struct pending
    {
      QByteArray data;
      bool feature{};
    };
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::deque<pending> queue;
    bool stopping{};
    std::thread writer;
    std::weak_ptr<state> weak_self;

    ~state() { stop(); }

    template <typename F>
    void post_to_qt(F&& f)
    {
      std::lock_guard g{qt_mutex};
      if(self)
        ossia::qt::run_async(self, std::forward<F>(f), Qt::QueuedConnection);
    }

    void writer_loop()
    {
      for(;;)
      {
        pending p;
        {
          std::unique_lock lock{queue_mutex};
          queue_cv.wait(lock, [this] { return stopping || !queue.empty(); });
          if(stopping)
          {
            queue.clear();
            return;
          }
          p = std::move(queue.front());
          queue.pop_front();
        }

        const std::span<const uint8_t> bytes{
            reinterpret_cast<const uint8_t*>(p.data.constData()),
            std::size_t(p.data.size())};
        const int r = p.feature ? device.send_feature_report(bytes) : device.write(bytes);
        if(r < 0)
        {
          auto err = QString::fromStdString(device.last_error());
          post_to_qt([w = weak_self, err] {
            if(auto s = w.lock(); s && s->self)
              s->self->report_error(err);
          });
        }
      }
    }

    void stop()
    {
      {
        std::lock_guard lock{queue_mutex};
        stopping = true;
      }
      queue_cv.notify_all();
      if(writer.joinable())
        writer.join();
      device.close();
    }
  };

  qml_hid_device() = default;

  ~qml_hid_device()
  {
    if(m_state)
    {
      {
        std::lock_guard g{m_state->qt_mutex};
        m_state->self = nullptr;
      }
      m_state->stop();
    }
  }

  void open(const ossia::net::hid_device_configuration& conf)
  {
    auto st = std::make_shared<state>();
    st->device.open(conf);
    st->self = this;
    st->weak_self = st;
    m_state = st;

    st->writer = std::thread{[s = st.get()] { s->writer_loop(); }};
    st->device.start(
        [s = st.get()](std::span<const uint8_t> report) {
      QByteArray buf{reinterpret_cast<const char*>(report.data()), qsizetype(report.size())};
      s->post_to_qt([w = s->weak_self, buf = std::move(buf)] {
        auto s = w.lock();
        if(!s)
          return;
        if(auto self = s->self; self && self->onReport.isCallable())
          if(auto engine = qjsEngine(self))
            self->onReport.call({engine->toScriptValue(buf)});
      });
    },
        [s = st.get()](std::string_view err) {
      auto msg = QString::fromUtf8(err.data(), err.size());
      s->post_to_qt([w = s->weak_self, msg] {
        auto s = w.lock();
        if(!s)
          return;
        if(auto self = s->self)
        {
          self->report_error(msg);
          if(self->onClose.isCallable())
            self->onClose.call();
        }
      });
    });

    if(onOpen.isCallable())
      onOpen.call({qjsEngine(this)->newQObject(this)});
  }

  void report_error(const QString& err)
  {
    if(onError.isCallable())
      onError.call({err});
  }

  bool isOpen() const noexcept { return m_state && m_state->device.is_open(); }
  W_SLOT(isOpen)

  void write(QJSValue data) { enqueue(bytes_from_js(data), false); }
  W_SLOT(write)

  void sendFeatureReport(QJSValue data) { enqueue(bytes_from_js(data), true); }
  W_SLOT(sendFeatureReport)

  QJSValue getFeatureReport(int id, int length)
  {
    auto engine = qjsEngine(this);
    if(!m_state || !engine || id < 0 || id > 255 || length <= 0)
      return QJSValue{};

    QByteArray buf(length + 1, 0);
    buf[0] = char(id);
    const int r = m_state->device.get_feature_report(
        {reinterpret_cast<uint8_t*>(buf.data()), std::size_t(buf.size())});
    if(r < 0)
      return QJSValue{};
    buf.resize(r);
    return engine->toScriptValue(buf);
  }
  W_SLOT(getFeatureReport)

  QJSValue reportDescriptor()
  {
    auto engine = qjsEngine(this);
    if(!m_state || !engine)
      return QJSValue{};
    auto desc = ossia::net::hid_read_report_descriptor(m_state->device.info().path);
    return engine->toScriptValue(
        QByteArray{reinterpret_cast<const char*>(desc.data()), qsizetype(desc.size())});
  }
  W_SLOT(reportDescriptor)

  QString path() const { return m_state ? QString::fromStdString(m_state->device.info().path) : QString{}; }
  W_SLOT(path)

  QString serial() const { return m_state ? QString::fromStdString(m_state->device.info().serial) : QString{}; }
  W_SLOT(serial)

  QString product() const { return m_state ? QString::fromStdString(m_state->device.info().product) : QString{}; }
  W_SLOT(product)

  void close()
  {
    if(!m_state)
      return;
    {
      std::lock_guard g{m_state->qt_mutex};
      m_state->self = nullptr;
    }
    m_state->stop();
    m_state.reset();
    if(onClose.isCallable())
      onClose.call();
  }
  W_SLOT(close)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onReport;

private:
  void enqueue(QByteArray data, bool feature)
  {
    if(!m_state || data.isEmpty())
      return;
    {
      std::lock_guard lock{m_state->queue_mutex};
      m_state->queue.push_back({std::move(data), feature});
    }
    m_state->queue_cv.notify_one();
  }

  std::shared_ptr<state> m_state;
};

}
#endif
