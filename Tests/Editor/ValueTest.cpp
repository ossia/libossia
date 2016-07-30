#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;

class ValueTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

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

        QVERIFY(p1 == Bool());
        QVERIFY(p1 == Int());
        QVERIFY(p1 == Float());
        QVERIFY(p1 == Char());
        QVERIFY(p1 == String());
        QVERIFY(p1 == Tuple());
        //! \todo == comparison with generic
        //! \todo == comparison with destination
        //! \todo == comparison with behavior

        QVERIFY(!(p1 != Bool()));
        QVERIFY(!(p1 != Int()));
        QVERIFY(!(p1 != Float()));
        QVERIFY(!(p1 != Char()));
        QVERIFY(!(p1 != String()));
        QVERIFY(!(p1 != Tuple()));
        //! \todo != comparison with generic
        //! \todo != comparison with destination
        //! \todo != comparison with behavior

        QVERIFY(!(p1 > Bool()));
        QVERIFY(!(p1 > Int()));
        QVERIFY(!(p1 > Float()));
        QVERIFY(!(p1 > Char()));
        QVERIFY(!(p1 > String()));
        QVERIFY(!(p1 > Tuple()));
        //! \todo > comparison with generic
        //! \todo > comparison with destination
        //! \todo > comparison with behavior

        QVERIFY(p1 >= Bool());
        QVERIFY(p1 >= Int());
        QVERIFY(p1 >= Float());
        QVERIFY(p1 >= Char());
        QVERIFY(p1 >= String());
        QVERIFY(p1 >= Tuple());
        //! \todo >= comparison with generic
        //! \todo >= comparison with destination
        //! \todo >= comparison with behavior

        QVERIFY(!(p1 < Bool()));
        QVERIFY(!(p1 < Int()));
        QVERIFY(!(p1 < Float()));
        QVERIFY(!(p1 < Char()));
        QVERIFY(!(p1 < String()));
        QVERIFY(!(p1 < Tuple()));
        //! \todo < comparison with generic
        //! \todo < comparison with destination
        //! \todo < comparison with behavior

        QVERIFY(p1 <= Bool());
        QVERIFY(p1 <= Int());
        QVERIFY(p1 <= Float());
        QVERIFY(p1 <= Char());
        QVERIFY(p1 <= String());
        QVERIFY(p1 <= Tuple());
        //! \todo <= comparison with generic
        //! \todo <= comparison with destination
        //! \todo <= comparison with behavior
    }

    /*! test bool */
    void test_bool()
    {
        Bool b1(true), b2(true);
        QVERIFY(b1 == b2);
        QVERIFY(!(b1 != b2));
        QVERIFY(!(b1 > b2));
        QVERIFY(b1 >= b2);
        QVERIFY(!(b1 < b2));
        QVERIFY(b1 <= b2);

        QVERIFY(b1 == Impulse());
        QVERIFY(b1 == Int(1));
        QVERIFY(b1 == Float(1));
        QVERIFY(b1 == Char(0x01));
        QVERIFY(!(b1 == String()));
        QVERIFY(b1 == Tuple(Int(1)));
        //! \todo == comparison with generic
        //! \todo == comparison with destination
        //! \todo == comparison with behavior

        QVERIFY(!(b1 != Impulse()));
        QVERIFY(b1 != Int());
        QVERIFY(b1 != Float());
        QVERIFY(b1 != Char(0x00));
        QVERIFY(b1 != String());
        QVERIFY(b1 != Tuple(Int(0)));
        //! \todo != comparison with generic
        //! \todo != comparison with destination
        //! \todo != comparison with behavior

        QVERIFY(!(b1 > Impulse()));
        QVERIFY(b1 > Int(0));
        QVERIFY(!(b1 > Float(2)));
        QVERIFY(b1 > Char(0x00));
        QVERIFY(!(b1 > String()));
        QVERIFY(!(b1 > Tuple({Int(0), Int(0)})));
        //! \todo > comparison with generic
        //! \todo > comparison with destination
        //! \todo > comparison with behavior

        QVERIFY(b1 >= Impulse());
        QVERIFY(!(b1 >= Int(2)));
        QVERIFY(b1 >= Float(1));
        QVERIFY(!(b1 >= Char(0x03)));
        QVERIFY(!(b1 >= String()));
        QVERIFY(!(b1 >= Tuple()));
        //! \todo >= comparison with generic
        //! \todo >= comparison with destination
        //! \todo >= comparison with behavior

        QVERIFY(!(b1 < Impulse()));
        QVERIFY(b1 < Int(2));
        QVERIFY(!(b1 < Float(1)));
        QVERIFY(!(b1 < Char()));
        QVERIFY(!(b1 < String()));
        QVERIFY(b1 < Tuple(Float(3)));
        //! \todo < comparison with generic
        //! \todo < comparison with destination
        //! \todo < comparison with behavior

        QVERIFY(b1 <= Impulse());
        QVERIFY(!(b1 <= Int()));
        QVERIFY(b1 <= Float(1));
        QVERIFY(b1 <= Char(0x32));
        QVERIFY(!(b1 <= String()));
        QVERIFY(!(b1 <= Tuple()));
        //! \todo <= comparison with generic
        //! \todo <= comparison with destination
        //! \todo <= comparison with behavior
    }

    /*! test int */
    void test_int()
    {
        Int i1(5), i2(5);

        QVERIFY(i1 == i2);
        QVERIFY(!(i1 != i2));
        QVERIFY(!(i1 > i2));
        QVERIFY(i1 >= i2);
        QVERIFY(!(i1 < i2));
        QVERIFY(i1 <= i2);

        QVERIFY(i1 == Impulse());
        QVERIFY(!(i1 == Bool(true)));
        QVERIFY(!(i1 == Float(1)));
        QVERIFY(i1 == Char(0x05));
        QVERIFY(!(i1 == String()));
        QVERIFY(i1 == Tuple(Int(5)));
        //! \todo == comparison with generic
        //! \todo == comparison with destination
        //! \todo == comparison with behavior

        QVERIFY(!(i1 != Impulse()));
        QVERIFY(i1 != Bool());
        QVERIFY(i1 != Float());
        QVERIFY(i1 != Char(0x00));
        QVERIFY(i1 != String());
        QVERIFY(i1 != Tuple(Int(0)));
        //! \todo != comparison with generic
        //! \todo != comparison with destination
        //! \todo != comparison with behavior

        QVERIFY(!(i1 > Impulse()));
        QVERIFY(i1 > Bool(false));
        QVERIFY(i1 > Float(2));
        QVERIFY(i1 > Char(0x00));
        QVERIFY(!(i1 > String()));
        QVERIFY(!(i1 > Tuple({Int(0), Int(0)})));
        //! \todo > comparison with generic
        //! \todo > comparison with destination
        //! \todo > comparison with behavior

        QVERIFY(i1 >= Impulse());
        QVERIFY(i1 >= Bool(true));
        QVERIFY(i1 >= Float(1));
        QVERIFY(i1 >= Char(0x03));
        QVERIFY(!(i1 >= String()));
        QVERIFY(!(i1 >= Tuple()));
        //! \todo >= comparison with generic
        //! \todo >= comparison with destination
        //! \todo >= comparison with behavior

        QVERIFY(!(i1 < Impulse()));
        QVERIFY(!(i1 < Bool(true)));
        QVERIFY(!(i1 < Float(1)));
        QVERIFY(!(i1 < Char()));
        QVERIFY(!(i1 < String()));
        QVERIFY(!(i1 < Tuple(Float(3))));
        //! \todo < comparison with generic
        //! \todo < comparison with destination
        //! \todo < comparison with behavior

        QVERIFY(i1 <= Impulse());
        QVERIFY(!(i1 <= Bool()));
        QVERIFY(!(i1 <= Float(1)));
        QVERIFY(i1 <= Char(0x32));
        QVERIFY(!(i1 <= String()));
        QVERIFY(!(i1 <= Tuple()));
        //! \todo <= comparison with generic
        //! \todo <= comparison with destination
        //! \todo <= comparison with behavior
    }

    /*! test flaot */
    void test_float()
    {
        Float f1(5);
        Float f2 = f1;
        QVERIFY(f1 == f2);
        QVERIFY(!(f1 != f2));
        QVERIFY(!(f1 > f2));
        QVERIFY(f1 >= f2);
        QVERIFY(!(f1 < f2));
        QVERIFY(f1 <= f2);

        QVERIFY(f1 == Impulse());
        QVERIFY(!(f1 == Bool(true)));
        QVERIFY(!(f1 == Int(1)));
        QVERIFY(f1 == Char(0x05));
        QVERIFY(!(f1 == String()));
        QVERIFY(f1 == Tuple(Int(5)));
        //! \todo == comparison with generic
        //! \todo == comparison with destination
        //! \todo == comparison with behavior

        QVERIFY(!(f1 != Impulse()));
        QVERIFY(f1 != Bool());
        QVERIFY(f1 != Int());
        QVERIFY(f1 != Char(0x00));
        QVERIFY(f1 != String());
        QVERIFY(f1 != Tuple(Int(0)));
        //! \todo != comparison with generic
        //! \todo != comparison with destination
        //! \todo != comparison with behavior

        QVERIFY(!(f1 > Impulse()));
        QVERIFY(f1 > Bool(false));
        QVERIFY(f1 > Int(2));
        QVERIFY(f1 > Char(0x00));
        QVERIFY(!(f1 > String()));
        QVERIFY(!(f1 > Tuple({Int(0), Int(0)})));
        //! \todo > comparison with generic
        //! \todo > comparison with destination
        //! \todo > comparison with behavior

        QVERIFY(f1 >= Impulse());
        QVERIFY(f1 >= Bool(true));
        QVERIFY(f1 >= Int(1));
        QVERIFY(f1 >= Char(0x03));
        QVERIFY(!(f1 >= String()));
        QVERIFY(!(f1 >= Tuple()));
        //! \todo >= comparison with generic
        //! \todo >= comparison with destination
        //! \todo >= comparison with behavior

        QVERIFY(!(f1 < Impulse()));
        QVERIFY(!(f1 < Bool(true)));
        QVERIFY(!(f1 < Int(1)));
        QVERIFY(!(f1 < Char()));
        QVERIFY(!(f1 < String()));
        QVERIFY(!(f1 < Tuple(Float(3))));
        //! \todo < comparison with generic
        //! \todo < comparison with destination
        //! \todo < comparison with behavior

        QVERIFY(f1 <= Impulse());
        QVERIFY(!(f1 <= Bool()));
        QVERIFY(!(f1 <= Int(1)));
        QVERIFY(f1 <= Char(0x32));
        QVERIFY(!(f1 <= String()));
        QVERIFY(!(f1 <= Tuple()));
        //! \todo <= comparison with generic
        //! \todo <= comparison with destination
        //! \todo <= comparison with behavior
    }

    /*! test char */
    void test_char()
    {
        Char c1(5);
        Char c2 = c1;
        QVERIFY(c1 == c2);
        QVERIFY(!(c1 != c2));
        QVERIFY(!(c1 > c2));
        QVERIFY(c1 >= c2);
        QVERIFY(!(c1 < c2));
        QVERIFY(c1 <= c2);

        QVERIFY(c1 == Impulse());
        QVERIFY(!(c1 == Bool(true)));
        QVERIFY(!(c1 == Int(1)));
        QVERIFY(c1 == Char(0x05));
        QVERIFY(!(c1 == String()));
        QVERIFY(c1 == Tuple(Int(5)));
        //! \todo == comparison with generic
        //! \todo == comparison with destination
        //! \todo == comparison with behavior

        QVERIFY(!(c1 != Impulse()));
        QVERIFY(c1 != Bool());
        QVERIFY(c1 != Int());
        QVERIFY(c1 != Char(0x00));
        QVERIFY(c1 != String());
        QVERIFY(c1 != Tuple(Int(0)));
        //! \todo != comparison with generic
        //! \todo != comparison with destination
        //! \todo != comparison with behavior

        QVERIFY(!(c1 > Impulse()));
        QVERIFY(c1 > Bool(false));
        QVERIFY(c1 > Int(2));
        QVERIFY(c1 > Char(0x00));
        QVERIFY(!(c1 > String()));
        QVERIFY(!(c1 > Tuple({new Int(0), new Int(0)})));
        //! \todo > comparison with generic
        //! \todo > comparison with destination
        //! \todo > comparison with behavior

        QVERIFY(c1 >= Impulse());
        QVERIFY(c1 >= Bool(true));
        QVERIFY(c1 >= Int(1));
        QVERIFY(c1 >= Char(0x03));
        QVERIFY(!(c1 >= String()));
        QVERIFY(!(c1 >= Tuple()));
        //! \todo >= comparison with generic
        //! \todo >= comparison with destination
        //! \todo >= comparison with behavior

        QVERIFY(!(c1 < Impulse()));
        QVERIFY(!(c1 < Bool(true)));
        QVERIFY(!(c1 < Int(1)));
        QVERIFY(!(c1 < Char()));
        QVERIFY(!(c1 < String()));
        QVERIFY(!(c1 < Tuple(Float(3))));
        //! \todo < comparison with generic
        //! \todo < comparison with destination
        //! \todo < comparison with behavior

        QVERIFY(c1 <= Impulse());
        QVERIFY(!(c1 <= Bool()));
        QVERIFY(!(c1 <= Int(1)));
        QVERIFY(c1 <= Char(0x32));
        QVERIFY(!(c1 <= String()));
        QVERIFY(!(c1 <= Tuple()));
        //! \todo <= comparison with generic
        //! \todo <= comparison with destination
        //! \todo <= comparison with behavior
    }

    /*! test string */
    void test_string()
    {
        String s1("qsd");
        String s2 = s1;

        QVERIFY(s1 == s2);
        QVERIFY(!(s1 != s2));
        QVERIFY(!(s1 > s2));
        QVERIFY(s1 >= s2);
        QVERIFY(!(s1 < s2));
        QVERIFY(s1 <= s2);

        QVERIFY(s1 == Impulse());
        QVERIFY(!(s1 == Bool(true)));
        QVERIFY(!(s1 == Int(1)));
        QVERIFY(!(s1 == Float(0)));
        QVERIFY(!(s1 == Char(0x05)));
        QVERIFY(!(s1 == Tuple(Int(5))));
        //! \todo == comparison with generic
        //! \todo == comparison with destination
        //! \todo == comparison with behavior

        QVERIFY(!(s1 != Impulse()));
        QVERIFY(s1 != Bool());
        QVERIFY(s1 != Int());
        QVERIFY(s1 != Float(1));
        QVERIFY(s1 != Char(0x00));
        QVERIFY(!(s1 != Tuple(String("qsd"))));
        //! \todo != comparison with generic
        //! \todo != comparison with destination
        //! \todo != comparison with behavior

        QVERIFY(!(s1 > Impulse()));
        QVERIFY(!(s1 > Bool(false)));
        QVERIFY(!(s1 > Int(2)));
        QVERIFY(!(s1 > Float(0.1)));
        QVERIFY(!(s1 > Char(0x00)));
        QVERIFY(!(s1 > Tuple({Int(0), Int(0)})));
        //! \todo > comparison with generic
        //! \todo > comparison with destination
        //! \todo > comparison with behavior

        QVERIFY(s1 >= Impulse());
        QVERIFY(!(s1 >= Bool(true)));
        QVERIFY(!(s1 >= Int(1)));
        QVERIFY(!(s1 >= Float(5)));
        QVERIFY(!(s1 >= Char(0x03)));
        QVERIFY(!(s1 >= Tuple()));
        //! \todo >= comparison with generic
        //! \todo >= comparison with destination
        //! \todo >= comparison with behavior

        QVERIFY(!(s1 < Impulse()));
        QVERIFY(!(s1 < Bool(true)));
        QVERIFY(!(s1 < Int(1)));
        QVERIFY(!(s1 < Float(3)));
        QVERIFY(!(s1 < Char()));
        QVERIFY(s1 < Tuple(String("wxc")));
        //! \todo < comparison with generic
        //! \todo < comparison with destination
        //! \todo < comparison with behavior

        QVERIFY(s1 <= Impulse());
        QVERIFY(!(s1 <= Bool()));
        QVERIFY(!(s1 <= Int(1)));
        QVERIFY(!(s1 <= Float()));
        QVERIFY(!(s1 <= Char(0x32)));
        QVERIFY(!(s1 <= Tuple()));
        //! \todo <= comparison with generic
        //! \todo <= comparison with destination
        //! \todo <= comparison with behavior
    }

    /*! test tuple */
    void test_tuple()
    {
        Tuple t1{Int(5), Float(0.2), String("abc")};
        QVERIFY(t1.value[0].getType() == Type::INT);
        QVERIFY(t1.value[1].getType() == Type::FLOAT);
        QVERIFY(t1.value[2].getType() == Type::STRING);

        Tuple t2 = t1;
        QVERIFY(t2.value[0].getType() == Type::INT);
        QVERIFY(t2.value[1].getType() == Type::FLOAT);
        QVERIFY(t2.value[2].getType() == Type::STRING);

        QVERIFY(t1 == t2);
        QVERIFY(!(t1 != t2));
        QVERIFY(!(t1 > t2));
        QVERIFY(t1 >= t2);
        QVERIFY(!(t1 < t2));
        QVERIFY(t1 <= t2);

        QVERIFY(t1 == Impulse());
        QVERIFY(!(t1 == Bool(true)));
        QVERIFY(!(t1 == Int(1)));
        QVERIFY(!(t1 == Float(5)));
        QVERIFY(!(t1 == Char(0x05)));
        QVERIFY(!(t1 == String()));
        //! \todo == comparison with generic
        //! \todo == comparison with destination
        //! \todo == comparison with behavior

        QVERIFY(!(t1 != Impulse()));
        QVERIFY(t1 != Bool());
        QVERIFY(t1 != Int());
        QVERIFY(t1 != Float(5));
        QVERIFY(t1 != Char(0x00));
        QVERIFY(t1 != String());
        //! \todo != comparison with generic
        //! \todo != comparison with destination
        //! \todo != comparison with behavior

        QVERIFY(!(t1 > Impulse()));
        QVERIFY(!(t1 > Bool(false)));
        QVERIFY(!(t1 > Int(2)));
        QVERIFY(!(t1 > Float(15)));
        QVERIFY(!(t1 > Char(0x00)));
        QVERIFY(!(t1 > String("abc")));
        //! \todo > comparison with generic
        //! \todo > comparison with destination
        //! \todo > comparison with behavior

        QVERIFY(t1 >= Impulse());
        QVERIFY(!(t1 >= Bool(true)));
        QVERIFY(!(t1 >= Int(1)));
        QVERIFY(!(t1 >= Float(-23)));
        QVERIFY(!(t1 >= Char(0x03)));
        QVERIFY(!(t1 >= String("wxc")));
        //! \todo >= comparison with generic
        //! \todo >= comparison with destination
        //! \todo >= comparison with behavior

        QVERIFY(!(t1 < Impulse()));
        QVERIFY(!(t1 < Bool(true)));
        QVERIFY(!(t1 < Int(1)));
        QVERIFY(!(t1 < Float(0.098)));
        QVERIFY(!(t1 < Char()));
        QVERIFY(!(t1 < String("wxc")));
        //! \todo < comparison with generic
        //! \todo < comparison with destination
        //! \todo < comparison with behavior

        QVERIFY(t1 <= Impulse());
        QVERIFY(!(t1 <= Bool()));
        QVERIFY(!(t1 <= Int(1)));
        QVERIFY(!(t1 <= Float()));
        QVERIFY(!(t1 <= Char(0x32)));
        QVERIFY(!(t1 <= String()));
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
        /*
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        auto localTupleNode = *(device->emplace(device->children().cend(), "my_tuple"));
        auto localTupleAddress = localTupleNode->createAddress(Type::TUPLE);

        Tuple t = {new Float(-1.), new Float(0.), new Float(1.)};
        localTupleAddress->setValue(&t);

        Destination d1(localTupleNode);
        QVERIFY(d1.getType() == Type::DESTINATION);
        QVERIFY(d1.index.size() == 0);

        Destination d2(localTupleNode, {1});
        QVERIFY(d2.index.size() == 1);
        QVERIFY(d2.index[0] == 1);
        */
    }

    /*! test behavior */
    void test_behavior()
    {
        //! \todo test clone()
    }
};

QTEST_APPLESS_MAIN(ValueTest)

#include "ValueTest.moc"
