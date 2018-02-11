// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/context.hpp>
#include <ossia-qt/device/qt_device.hpp>
#include <iostream>
#include <QVector>
#include <QtGui/QVector3D>


class SomeObject : public QObject
{
  Q_OBJECT
  Q_PROPERTY(int tutu READ tutu WRITE setTutu NOTIFY tutuChanged)
  Q_PROPERTY(QString str READ str WRITE setStr NOTIFY strChanged)
  Q_PROPERTY(QVector3D vec3 READ vec3 WRITE setVec3 NOTIFY vec3Changed)

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

public Q_SLOTS:
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

Q_SIGNALS:
void vec3Changed(QVector3D vec3);
void strChanged(QString str);
void tutuChanged(int tutu);
};
using namespace ossia;
class QtDeviceTest : public QObject
{
    Q_OBJECT

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
          app,
          std::make_unique<ossia::net::multiplex_protocol>(),
          "newDevice" };

        obj2.setTutu(555);
        obj2.setVec3({1, 2, 3});

        auto& proto = static_cast<ossia::net::multiplex_protocol&>(dev.get_protocol());
        proto.expose_to(
              std::make_unique<ossia::net::minuit_protocol>("score", "127.0.0.1", 13579, 9998));

       QTimer::singleShot(3000, [&] () { app.exit(); });

        app.exec();
    }
};


QTEST_APPLESS_MAIN(QtDeviceTest)

#include "QtDeviceTest.moc"

