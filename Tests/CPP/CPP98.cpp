// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia-cpp/ossia-cpp98.hpp>
class CPP98Test : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void test_basic()
    {
      using namespace std::literals;
      opp::oscquery_server server{"banana"};
      auto root = server.get_root_node();
      auto argb = root
          .create_child("foo")
          .create_argb("bar/baz")
          .set_access(opp::Get)
          ;

      // creates /foo/bar/baz of type argb in get
      QCOMPARE(argb.get_parameter(), "/foo/bar/baz"s);
      QCOMPARE(argb.get_unit(), "color.argb"s);

      auto blob = root.create_buffer("/foo/my/buffer");
      QVERIFY(blob.get_value().is_string());
    }
};

QTEST_APPLESS_MAIN(CPP98Test)

#include "CPP98.moc"
