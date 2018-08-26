// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/context.hpp>
#include <ossia-qt/device/qt_device.hpp>
#include <iostream>
#include <QVector>
#include <QtGui/QVector3D>
#include <wobjectdefs.h>
#include <wobjectimpl.h>
#include <ossia/network/oscquery/oscquery_server.hpp>

class SomeObject : public QObject
{
  W_OBJECT(SomeObject)
  QVector3D m_vec3;

  QString m_str;

  int m_tutu;

public:
  using QObject::QObject;
  QVector3D vec3() const
  {
    return m_vec3;
  }
  QString str() const
  {
    return m_str;
  }

  int tutu() const
  {
    return m_tutu;
  }

public:
  void setVec3(QVector3D vec3)
  {
    if (m_vec3 == vec3)
      return;

    m_vec3 = vec3;
    vec3Changed(vec3);
  }
  void setStr(QString str)
  {
    if (m_str == str)
      return;

    m_str = str;
    strChanged(str);
  }

  void setTutu(int tutu)
  {
    if (m_tutu == tutu)
      return;

    m_tutu = tutu;
    tutuChanged(tutu);
  }

public:
  void vec3Changed(QVector3D vec3) W_SIGNAL(vec3Changed, vec3);
  void strChanged(QString str) W_SIGNAL(strChanged, str);
  void tutuChanged(int tutu) W_SIGNAL(tutuChanged, tutu);
  W_PROPERTY(int, tutu READ tutu WRITE setTutu NOTIFY tutuChanged)
  W_PROPERTY(QString, str READ str WRITE setStr NOTIFY strChanged)
  W_PROPERTY(QVector3D, vec3 READ vec3 WRITE setVec3 NOTIFY vec3Changed)

};
using namespace ossia;

class QtDeviceTest : public QObject
{
  W_OBJECT(QtDeviceTest)

private Q_SLOTS:
  void test_device()
  {
    int argc{}; char** argv{};
    QCoreApplication app(argc, argv);

    QObject obj1{&app};
    Q_SET_OBJECT_NAME(obj1);
    SomeObject obj2{&obj1};
    Q_SET_OBJECT_NAME(obj2);

    ossia::context context;

    ossia::qt::qt_device dev{
      app, std::make_unique<ossia::oscquery::oscquery_server_protocol>(), "newDevice" };

    obj2.setTutu(555);
    obj2.setVec3({1, 2, 3});

    QTimer::singleShot(3000, [&] () { app.exit(); });

    app.exec();
  } W_SLOT(test_device);
};
W_OBJECT_IMPL(SomeObject)
W_OBJECT_IMPL(QtDeviceTest)


QTEST_APPLESS_MAIN(QtDeviceTest)
