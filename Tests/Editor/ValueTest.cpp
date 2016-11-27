#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

class ValueTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void test_wrapped()
  {
    QVERIFY(Impulse() == true);
    QVERIFY(ossia::value(Impulse()) == ossia::value(true));
  }

  /*! test impulse */
  void test_impulse()
  {
    Impulse p1;
    Impulse p2;
    QVERIFY(p1 == p2);
    QVERIFY(!(p1 != p2));
    QVERIFY(!(p1 > p2));
    QVERIFY(p1 >= p2);
    QVERIFY(!(p1 < p2));
    QVERIFY(p1 <= p2);

    QVERIFY(p1 == bool{});
    QVERIFY(p1 == int32_t{});
    QVERIFY(p1 == float{});
    QVERIFY(p1 == char{});
    QVERIFY(p1 == value::make<std::string>());
    QVERIFY(p1 == std::vector<ossia::value>{});
    //! \todo == comparison with generic
    //! \todo == comparison with destination
    //! \todo == comparison with behavior

    QVERIFY(!(p1 != bool{}));
    QVERIFY(!(p1 != int32_t{}));
    QVERIFY(!(p1 != float{}));
    QVERIFY(!(p1 != char{}));
    QVERIFY(!(p1 != value::make<std::string>()));
    QVERIFY(!(p1 != std::vector<ossia::value>{}));
    //! \todo != comparison with generic
    //! \todo != comparison with destination
    //! \todo != comparison with behavior

    QVERIFY(!(p1 > bool{}));
    QVERIFY(!(p1 > int32_t{}));
    QVERIFY(!(p1 > float{}));
    QVERIFY(!(p1 > char{}));
    QVERIFY(!(p1 > value::make<std::string>()));
    QVERIFY(!(p1 > std::vector<ossia::value>{}));
    //! \todo > comparison with generic
    //! \todo > comparison with destination
    //! \todo > comparison with behavior

    QVERIFY(p1 >= bool{});
    QVERIFY(p1 >= int32_t{});
    QVERIFY(p1 >= float{});
    QVERIFY(p1 >= char{});
    QVERIFY(p1 >= value::make<std::string>());
    QVERIFY(p1 >= std::vector<ossia::value>{});
    //! \todo >= comparison with generic
    //! \todo >= comparison with destination
    //! \todo >= comparison with behavior

    QVERIFY(!(p1 < bool{}));
    QVERIFY(!(p1 < int32_t{}));
    QVERIFY(!(p1 < float{}));
    QVERIFY(!(p1 < char{}));
    QVERIFY(!(p1 < value::make<std::string>()));
    QVERIFY(!(p1 < std::vector<ossia::value>{}));
    //! \todo < comparison with generic
    //! \todo < comparison with destination
    //! \todo < comparison with behavior

    QVERIFY(p1 <= bool{});
    QVERIFY(p1 <= int32_t{});
    QVERIFY(p1 <= float{});
    QVERIFY(p1 <= char{});
    QVERIFY(p1 <= value::make<std::string>());
    QVERIFY(p1 <= std::vector<ossia::value>{});
    //! \todo <= comparison with generic
    //! \todo <= comparison with destination
    //! \todo <= comparison with behavior
  }

  /*! test bool */
  void test_bool()
  {
    bool b1(true), b2(true);
    QVERIFY(b1 == b2);
    QVERIFY(!(b1 != b2));
    QVERIFY(!(b1 > b2));
    QVERIFY(b1 >= b2);
    QVERIFY(!(b1 < b2));
    QVERIFY(b1 <= b2);

    QVERIFY(b1 == Impulse());
    QVERIFY(b1 == int32_t(1));
    QVERIFY(b1 == 1.);
    QVERIFY(b1 == char{0x01});
    QVERIFY(!(b1 == value::make<std::string>()));
    QVERIFY(b1 == std::vector<ossia::value>{int32_t(1)});
    //! \todo == comparison with generic
    //! \todo == comparison with destination
    //! \todo == comparison with behavior

    QVERIFY(!(b1 != Impulse()));
    QVERIFY(b1 != int32_t{});
    QVERIFY(b1 != float{});
    QVERIFY(b1 != char{0x00});
    QVERIFY(b1 != value::make<std::string>());
    QVERIFY(b1 != std::vector<ossia::value>{0});
    //! \todo != comparison with generic
    //! \todo != comparison with destination
    //! \todo != comparison with behavior

    QVERIFY(!(b1 > Impulse()));
    QVERIFY(b1 > int32_t(0));
    QVERIFY(!(b1 > float(2)));
    QVERIFY(b1 > char{0x00});
    QVERIFY(!(b1 > value::make<std::string>()));
    QVERIFY(!(b1 > std::vector<ossia::value>{0, 0}));
    //! \todo > comparison with generic
    //! \todo > comparison with destination
    //! \todo > comparison with behavior

    QVERIFY(b1 >= Impulse());
    QVERIFY(b1 >= 1.);
    QVERIFY(!(b1 >= value::make<std::string>()));
    QVERIFY(!(b1 >= std::vector<ossia::value>{}));
    //! \todo >= comparison with generic
    //! \todo >= comparison with destination
    //! \todo >= comparison with behavior

    QVERIFY(!(b1 < Impulse()));
    QVERIFY(!(b1 < 1.));
    QVERIFY(!(b1 < value::make<std::string>()));
    QVERIFY(b1 < std::vector<ossia::value>{float(3)});
    //! \todo < comparison with generic
    //! \todo < comparison with destination
    //! \todo < comparison with behavior

    QVERIFY(b1 <= Impulse());
    QVERIFY(!(b1 <= int32_t{}));
    QVERIFY(b1 <= 1.);
    QVERIFY(!(b1 <= value::make<std::string>()));
    QVERIFY(!(b1 <= std::vector<ossia::value>{}));
    //! \todo <= comparison with generic
    //! \todo <= comparison with destination
    //! \todo <= comparison with behavior
  }

  /*! test int */
  void test_int32_t()
  {
    int32_t i1(5), i2(5);

    QVERIFY(i1 == i2);
    QVERIFY(!(i1 != i2));
    QVERIFY(!(i1 > i2));
    QVERIFY(i1 >= i2);
    QVERIFY(!(i1 < i2));
    QVERIFY(i1 <= i2);

    QVERIFY(i1 == Impulse());
    QVERIFY(!(i1 == true));
    QVERIFY(!(i1 == 1.));
    QVERIFY(i1 == char(0x05));
    QVERIFY(!(i1 == value::make<std::string>()));
    QVERIFY(i1 == std::vector<ossia::value>{int32_t(5)});
    //! \todo == comparison with generic
    //! \todo == comparison with destination
    //! \todo == comparison with behavior

    QVERIFY(!(i1 != Impulse()));
    QVERIFY(i1 != bool{});
    QVERIFY(i1 != float{});
    QVERIFY(i1 != char{0x00});
    QVERIFY(i1 != value::make<std::string>());
    QVERIFY(i1 != std::vector<ossia::value>{0});
    //! \todo != comparison with generic
    //! \todo != comparison with destination
    //! \todo != comparison with behavior

    QVERIFY(!(i1 > Impulse()));
    QVERIFY(i1 > bool(false));
    QVERIFY(i1 > float(2));
    QVERIFY(i1 > char{0x00});
    QVERIFY(!(i1 > value::make<std::string>()));
    QVERIFY(!(i1 > std::vector<ossia::value>{0, 0}));
    //! \todo > comparison with generic
    //! \todo > comparison with destination
    //! \todo > comparison with behavior

    QVERIFY(i1 >= Impulse());
    QVERIFY(i1 >= true);
    QVERIFY(i1 >= 1.);
    QVERIFY(i1 >= char(0x03));
    QVERIFY(!(i1 >= value::make<std::string>()));
    QVERIFY(!(i1 >= std::vector<ossia::value>{}));
    //! \todo >= comparison with generic
    //! \todo >= comparison with destination
    //! \todo >= comparison with behavior

    QVERIFY(!(i1 < Impulse()));
    QVERIFY(!(i1 < true));
    QVERIFY(!(i1 < 1.));
    QVERIFY(!(i1 < char{}));
    QVERIFY(!(i1 < value::make<std::string>()));
    QVERIFY(!(i1 < std::vector<ossia::value>{float(3)}));
    //! \todo < comparison with generic
    //! \todo < comparison with destination
    //! \todo < comparison with behavior

    QVERIFY(i1 <= Impulse());
    QVERIFY(!(i1 <= bool{}));
    QVERIFY(!(i1 <= 1.));
    QVERIFY(i1 <= char(0x32));
    QVERIFY(!(i1 <= value::make<std::string>()));
    QVERIFY(!(i1 <= std::vector<ossia::value>{}));
    //! \todo <= comparison with generic
    //! \todo <= comparison with destination
    //! \todo <= comparison with behavior
  }

  /*! test flaot */
  void test_float()
  {
    float f1(5);
    float f2 = f1;
    QVERIFY(f1 == f2);
    QVERIFY(!(f1 != f2));
    QVERIFY(!(f1 > f2));
    QVERIFY(f1 >= f2);
    QVERIFY(!(f1 < f2));
    QVERIFY(f1 <= f2);

    QVERIFY(f1 == Impulse());
    QVERIFY(!(f1 == true));
    QVERIFY(!(f1 == int32_t(1)));
    QVERIFY(f1 == char(0x05));
    QVERIFY(!(f1 == value::make<std::string>()));
    QVERIFY(f1 == std::vector<ossia::value>{int32_t(5)});
    //! \todo == comparison with generic
    //! \todo == comparison with destination
    //! \todo == comparison with behavior

    QVERIFY(!(f1 != Impulse()));
    QVERIFY(f1 != bool{});
    QVERIFY(f1 != int32_t{});
    QVERIFY(f1 != char{0x00});
    QVERIFY(f1 != value::make<std::string>());
    QVERIFY(f1 != std::vector<ossia::value>{0});
    //! \todo != comparison with generic
    //! \todo != comparison with destination
    //! \todo != comparison with behavior

    QVERIFY(!(f1 > Impulse()));
    QVERIFY(f1 > bool(false));
    QVERIFY(f1 > 2);
    QVERIFY(f1 > char{0x00});
    QVERIFY(!(f1 > value::make<std::string>()));
    QVERIFY(!(f1 > std::vector<ossia::value>{0, 0}));
    //! \todo > comparison with generic
    //! \todo > comparison with destination
    //! \todo > comparison with behavior

    QVERIFY(f1 >= Impulse());
    QVERIFY(f1 >= true);
    QVERIFY(f1 >= int32_t(1));
    QVERIFY(f1 >= char(0x03));
    QVERIFY(!(f1 >= value::make<std::string>()));
    QVERIFY(!(f1 >= std::vector<ossia::value>{}));
    //! \todo >= comparison with generic
    //! \todo >= comparison with destination
    //! \todo >= comparison with behavior

    QVERIFY(!(f1 < Impulse()));
    QVERIFY(!(f1 < true));
    QVERIFY(!(f1 < int32_t(1)));
    QVERIFY(!(f1 < char{}));
    QVERIFY(!(f1 < value::make<std::string>()));
    QVERIFY(!(f1 < std::vector<ossia::value>{3.}));
    //! \todo < comparison with generic
    //! \todo < comparison with destination
    //! \todo < comparison with behavior

    QVERIFY(f1 <= Impulse());
    QVERIFY(!(f1 <= bool{}));
    QVERIFY(!(f1 <= int32_t(1)));
    QVERIFY(f1 <= char(0x32));
    QVERIFY(!(f1 <= value::make<std::string>()));
    QVERIFY(!(f1 <= std::vector<ossia::value>{}));
    //! \todo <= comparison with generic
    //! \todo <= comparison with destination
    //! \todo <= comparison with behavior
  }

  /*! test char */
  void test_char()
  {
    char c1(5);
    char c2 = c1;
    QVERIFY(c1 == c2);
    QVERIFY(!(c1 != c2));
    QVERIFY(!(c1 > c2));
    QVERIFY(c1 >= c2);
    QVERIFY(!(c1 < c2));
    QVERIFY(c1 <= c2);

    QVERIFY(c1 == Impulse());
    QVERIFY(!(c1 == true));
    QVERIFY(!(c1 == int32_t(1)));
    QVERIFY(c1 == char(0x05));
    QVERIFY(!(c1 == value::make<std::string>()));
    QVERIFY(c1 == std::vector<ossia::value>{5});
    //! \todo == comparison with generic
    //! \todo == comparison with destination
    //! \todo == comparison with behavior

    QVERIFY(!(c1 != Impulse()));
    QVERIFY(c1 != bool{});
    QVERIFY(c1 != int32_t{});
    QVERIFY(c1 != char{0x00});
    QVERIFY(c1 != value::make<std::string>());
    QVERIFY(c1 != std::vector<ossia::value>{0});
    //! \todo != comparison with generic
    //! \todo != comparison with destination
    //! \todo != comparison with behavior

    QVERIFY(!(c1 > Impulse()));
    QVERIFY(c1 > bool(false));
    QVERIFY(c1 > 2);
    QVERIFY(c1 > char{0x00});
    QVERIFY(!(c1 > value::make<std::string>()));
    QVERIFY(!(c1 > std::vector<ossia::value>({int32_t(0), int32_t(0)})));
    //! \todo > comparison with generic
    //! \todo > comparison with destination
    //! \todo > comparison with behavior

    QVERIFY(c1 >= Impulse());
    QVERIFY(c1 >= true);
    QVERIFY(c1 >= int32_t(1));
    QVERIFY(c1 >= char(0x03));
    QVERIFY(!(c1 >= value::make<std::string>()));
    QVERIFY(!(c1 >= std::vector<ossia::value>{}));
    //! \todo >= comparison with generic
    //! \todo >= comparison with destination
    //! \todo >= comparison with behavior

    QVERIFY(!(c1 < Impulse()));
    QVERIFY(!(c1 < true));
    QVERIFY(!(c1 < int32_t(1)));
    QVERIFY(!(c1 < char{}));
    QVERIFY(!(c1 < value::make<std::string>()));
    QVERIFY(!(c1 < std::vector<ossia::value>{3.}));
    //! \todo < comparison with generic
    //! \todo < comparison with destination
    //! \todo < comparison with behavior

    QVERIFY(c1 <= Impulse());
    QVERIFY(!(c1 <= bool{}));
    QVERIFY(!(c1 <= int32_t(1)));
    QVERIFY(c1 <= char(0x32));
    QVERIFY(!(c1 <= value::make<std::string>()));
    QVERIFY(!(c1 <= std::vector<ossia::value>{}));
    //! \todo <= comparison with generic
    //! \todo <= comparison with destination
    //! \todo <= comparison with behavior
  }

  /*! test string */
  void test_value_string()
  {
    auto s1 = value::make<std::string>("qsd");
    auto s2 = s1;

    QVERIFY(s1 == s2);
    QVERIFY(!(s1 != s2));
    QVERIFY(!(s1 > s2));
    QVERIFY(s1 >= s2);
    QVERIFY(!(s1 < s2));
    QVERIFY(s1 <= s2);

    QVERIFY(s1 == Impulse());
    QVERIFY(!(s1 == true));
    QVERIFY(!(s1 == int32_t(1)));
    QVERIFY(!(s1 == float(0)));
    QVERIFY(!(s1 == char(0x05)));
    QVERIFY(!(s1 == std::vector<ossia::value>{5}));
    //! \todo == comparison with generic
    //! \todo == comparison with destination
    //! \todo == comparison with behavior

    QVERIFY(!(s1 != Impulse()));
    QVERIFY(s1 != bool{});
    QVERIFY(s1 != int32_t{});
    QVERIFY(s1 != 1.);
    QVERIFY(s1 != char{0x00});
    QVERIFY(!(s1 != std::vector<ossia::value>{std::string("qsd")}));
    //! \todo != comparison with generic
    //! \todo != comparison with destination
    //! \todo != comparison with behavior

    QVERIFY(!(s1 > Impulse()));
    QVERIFY(!(s1 > bool(false)));
    QVERIFY(!(s1 > 2));
    QVERIFY(!(s1 > float(0.1)));
    QVERIFY(!(s1 > char{0x00}));
    QVERIFY(!(s1 > std::vector<ossia::value>{0, 0}));
    //! \todo > comparison with generic
    //! \todo > comparison with destination
    //! \todo > comparison with behavior

    QVERIFY(s1 >= Impulse());
    QVERIFY(!(s1 >= true));
    QVERIFY(!(s1 >= int32_t(1)));
    QVERIFY(!(s1 >= float(5)));
    QVERIFY(!(s1 >= char(0x03)));
    QVERIFY(!(s1 >= std::vector<ossia::value>{}));
    //! \todo >= comparison with generic
    //! \todo >= comparison with destination
    //! \todo >= comparison with behavior

    QVERIFY(!(s1 < Impulse()));
    QVERIFY(!(s1 < true));
    QVERIFY(!(s1 < int32_t(1)));
    QVERIFY(!(s1 < float(3)));
    QVERIFY(!(s1 < char{}));

    QVERIFY(s1 < std::vector<ossia::value>{std::string("wxc")});
    //! \todo < comparison with generic
    //! \todo < comparison with destination
    //! \todo < comparison with behavior

    QVERIFY(s1 <= Impulse());
    QVERIFY(!(s1 <= bool{}));
    QVERIFY(!(s1 <= int32_t(1)));
    QVERIFY(!(s1 <= float{}));
    QVERIFY(!(s1 <= char(0x32)));
    QVERIFY(!(s1 <= std::vector<ossia::value>{}));
    //! \todo <= comparison with generic
    //! \todo <= comparison with destination
    //! \todo <= comparison with behavior
  }

  /*! test std::vector<ossia::value> */
  void test_tuple()
  {
    std::vector<ossia::value> t1{int32_t(5), float(0.2), std::string("abc")};
    QVERIFY(t1[0].getType() == val_type::INT);
    QVERIFY(t1[1].getType() == val_type::FLOAT);
    QVERIFY(t1[2].getType() == val_type::STRING);

    std::vector<ossia::value> t2 = t1;
    QVERIFY(t2[0].getType() == val_type::INT);
    QVERIFY(t2[1].getType() == val_type::FLOAT);
    QVERIFY(t2[2].getType() == val_type::STRING);

    QVERIFY(t1 == t2);
    QVERIFY(!(t1 != t2));
    QVERIFY(!(t1 > t2));
    QVERIFY(t1 >= t2);
    QVERIFY(!(t1 < t2));
    QVERIFY(t1 <= t2);

    QVERIFY(t1 == Impulse());
    QVERIFY(!(t1 == true));
    QVERIFY(!(t1 == int32_t(1)));
    QVERIFY(!(t1 == float(5)));
    QVERIFY(!(t1 == char(0x05)));
    QVERIFY(!(t1 == value::make<std::string>()));
    //! \todo == comparison with generic
    //! \todo == comparison with destination
    //! \todo == comparison with behavior

    QVERIFY(!(t1 != Impulse()));
    QVERIFY(t1 != bool{});
    QVERIFY(t1 != int32_t{});
    QVERIFY(t1 != float(5));
    QVERIFY(t1 != char{0x00});
    QVERIFY(t1 != value::make<std::string>());
    //! \todo != comparison with generic
    //! \todo != comparison with destination
    //! \todo != comparison with behavior

    QVERIFY(!(t1 > Impulse()));
    QVERIFY(!(t1 > bool(false)));
    QVERIFY(!(t1 > 2));
    QVERIFY(!(t1 > float(15)));
    QVERIFY(!(t1 > char{0x00}));
    QVERIFY(!(t1 > std::string("abc")));
    //! \todo > comparison with generic
    //! \todo > comparison with destination
    //! \todo > comparison with behavior

    QVERIFY(t1 >= Impulse());
    QVERIFY(!(t1 >= true));
    QVERIFY(!(t1 >= int32_t(1)));
    QVERIFY(!(t1 >= float(-23)));
    QVERIFY(!(t1 >= char(0x03)));
    QVERIFY(!(t1 >= std::string("wxc")));
    //! \todo >= comparison with generic
    //! \todo >= comparison with destination
    //! \todo >= comparison with behavior

    QVERIFY(!(t1 < Impulse()));
    QVERIFY(!(t1 < true));
    QVERIFY(!(t1 < int32_t(1)));
    QVERIFY(!(t1 < float(0.098)));
    QVERIFY(!(t1 < char{}));
    QVERIFY(!(t1 < std::string("wxc")));
    //! \todo < comparison with generic
    //! \todo < comparison with destination
    //! \todo < comparison with behavior

    QVERIFY(t1 <= Impulse());
    QVERIFY(!(t1 <= bool{}));
    QVERIFY(!(t1 <= int32_t(1)));
    QVERIFY(!(t1 <= float{}));
    QVERIFY(!(t1 <= char(0x32)));
    QVERIFY(!(t1 <= value::make<std::string>()));
    //! \todo <= comparison with generic
    //! \todo <= comparison with destination
    //! \todo <= comparison with behavior
  }

  /*! test generic */
  void test_generic()
  {
    //! \todo test clone()
  }

  /*! test destination */
  void test_destination()
  {
    //! \todo test clone()

    // Local device
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};

    auto localTupleNode = device.createChild("my_tuple");
    auto localTupleAddress = localTupleNode->createAddress(val_type::TUPLE);

    std::vector<ossia::value> t{float(-1.), float(0.), float(1.)};
    localTupleAddress->setValue(t);

    Destination d1(*localTupleAddress);
    QVERIFY(d1.index.size() == 0);

    Destination d2(*localTupleAddress, ossia::destination_index{1});
    QVERIFY(d2.index.size() == 1);
    QVERIFY(d2.index[0] == 1);

    Destination d3 = d1;
    QVERIFY(&d3.value.get() == &d1.value.get());
    Destination d4 = std::move(d1);
    QVERIFY(&d4.value.get() == &d3.value.get());
  }


  /*! test behavior */
  void test_behavior()
  {
    //! \todo test clone()
  }
};

QTEST_APPLESS_MAIN(ValueTest)

#include "ValueTest.moc"
