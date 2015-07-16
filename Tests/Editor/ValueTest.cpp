#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class ValueTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test impulse */
    void test_impulse()
    {
        Impulse p1;
        QVERIFY(p1.getType() == Value::Type::IMPULSE);

        Value* p2 = p1.clone();
        QVERIFY(p2->getType() == Value::Type::IMPULSE);

        QVERIFY(p1 == *p2);
        QVERIFY(!(p1 != *p2));
        QVERIFY(!(p1 > *p2));
        QVERIFY(p1 >= *p2);
        QVERIFY(!(p1 < *p2));
        QVERIFY(p1 >= *p2);

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
        Bool b1(true);
        QVERIFY(b1.getType() == Value::Type::BOOL);

        Value* b2 = b1.clone();
        QVERIFY(b2->getType() == Value::Type::BOOL);

        QVERIFY(b1 == *b2);
        QVERIFY(!(b1 != *b2));
        QVERIFY(!(b1 > *b2));
        QVERIFY(b1 >= *b2);
        QVERIFY(!(b1 < *b2));
        QVERIFY(b1 >= *b2);

        QVERIFY(b1 == Impulse());
        QVERIFY(b1 == Int(1));
        QVERIFY(b1 == Float(1));
        QVERIFY(b1 == Char(0x01));
        QVERIFY(!(b1 == String()));
        QVERIFY(b1 == Tuple(new Int(1)));
        //! \todo == comparison with generic
        //! \todo == comparison with destination
        //! \todo == comparison with behavior

        QVERIFY(!(b1 != Impulse()));
        QVERIFY(b1 != Int());
        QVERIFY(b1 != Float());
        QVERIFY(b1 != Char(0x00));
        QVERIFY(b1 != String());
        QVERIFY(b1 != Tuple(new Int(0)));
        //! \todo != comparison with generic
        //! \todo != comparison with destination
        //! \todo != comparison with behavior

        QVERIFY(!(b1 > Impulse()));
        QVERIFY(b1 > Int(0));
        QVERIFY(!(b1 > Float(2)));
        QVERIFY(b1 > Char(0x00));
        QVERIFY(!(b1 > String()));
        QVERIFY(!(b1 > Tuple(new Int(0), new Int(0))));
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
        QVERIFY(b1 < Tuple(new Float(3)));
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
        //! \todo test clone()
    }

    /*! test flaot */
    void test_float()
    {
        //! \todo test clone()
    }

    /*! test char */
    void test_char()
    {
        //! \todo test clone()
    }

    /*! test string */
    void test_string()
    {
        //! \todo test clone()
    }

    /*! test tuple */
    void test_tuple()
    {
        //! \todo test clone()
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
    }

    /*! test behavior */
    void test_behavior()
    {
        //! \todo test clone()
    }
};

QTEST_APPLESS_MAIN(ValueTest)

#include "ValueTest.moc"
