#include <QtTest>
#include <iostream>
#include "TestUtils.hpp"

using namespace ossia;
using namespace ossia::net;
template<typename R, typename T>
ossia::value to(T f) { return R{(decltype(R{}))f}; }

class DomainTest : public QObject
{
  Q_OBJECT

  template<typename T>
  void push_all(ossia::net::address_base& addr, T min, T max)
  {
    using val_t = decltype(T{});
    for(int i = 0; i < 6; i++)
    {
      addr.setBoundingMode((bounding_mode)i);
      addr.pushValue(T{(val_t)(min - 100)});
      addr.pushValue(min);
      addr.pushValue(T{(val_t)((min + max) / 2)});
      addr.pushValue(max);
      addr.pushValue(T{(val_t)(max + 100)});
    }
  }

  template<typename T>
  void test_clamp_numeric(ossia::net::address_base& addr, T min, T max)
  {
    auto dom = ossia::net::make_domain(min, max);
    addr.setDomain(dom);

    // min and max
    push_all(addr, min, max);

    // no min
    ossia::net::set_min(dom, {});
    push_all(addr, min, max);

    // no max
    ossia::net::set_min(dom, min);
    ossia::net::set_max(dom, {});
    push_all(addr, min, max);

    // nothing
    ossia::net::set_min(dom, {});
    ossia::net::set_max(dom, {});
    push_all(addr, min, max);
  }

  template<std::size_t N>
  void push_all_vec(ossia::net::address_base& addr, Float min, Float max)
  {
    using val_t = Vec<float, N>;
    std::vector<Vec<float, N>> test_vecs;
    for(float f : {min - 100, min, (min + max) / 2, max, max + 100})
    {
      val_t vec;
      vec.fill(f);
      test_vecs.push_back(vec);
    }

    for(int i = 0; i < 6; i++)
    {
      addr.setBoundingMode((bounding_mode)i);
      for(auto& vec : test_vecs)
        addr.pushValue(vec);
    }
  }

  template<std::size_t N>
  void push_all_vec(ossia::net::address_base& addr, Vec<float, N> min, Vec<float, N> max)
  {
    using val_t = Vec<float, N>;
    std::vector<Vec<float, N>> test_vecs;
    for(float f : {min[0] - 100, min[0], (min[0] + max[0]) / 2, max[0], max[0] + 100})
    {
      val_t vec;
      vec.fill(f);
      test_vecs.push_back(vec);
    }

    for(int i = 0; i < 6; i++)
    {
      addr.setBoundingMode((bounding_mode)i);
      for(auto& vec : test_vecs)
        addr.pushValue(vec);
    }
  }

  template<std::size_t N>
  void test_clamp_vec(ossia::net::address_base& addr, Float min, Float max)
  {
    auto dom = ossia::net::make_domain(min, max);
    addr.setDomain(dom);

    // min and max
    push_all_vec<N>(addr, min, max);

    // no min
    ossia::net::set_min(dom, {});
    push_all_vec<N>(addr, min, max);

    // no max
    ossia::net::set_min(dom, min);
    ossia::net::set_max(dom, {});
    push_all_vec<N>(addr, min, max);

    // nothing
    ossia::net::set_min(dom, {});
    ossia::net::set_max(dom, {});
    push_all_vec<N>(addr, min, max);
  }

  template<std::size_t N>
  void test_clamp_vec(ossia::net::address_base& addr, Vec<float, N> min, Vec<float, N> max)
  {
    auto dom = ossia::net::make_domain(min, max);
    addr.setDomain(dom);

    // min and max
    push_all_vec<N>(addr, min, max);

    // no min
    ossia::net::set_min(dom, {});
    push_all_vec<N>(addr, min, max);

    // no max
    ossia::net::set_min(dom, min);
    ossia::net::set_max(dom, {});
    push_all_vec<N>(addr, min, max);

    // nothing
    ossia::net::set_min(dom, {});
    ossia::net::set_max(dom, {});
    push_all_vec<N>(addr, min, max);
  }


  template<typename T>
  void push_tuple(ossia::net::address_base& addr, T min, T max)
  {
    using val_t = T;
    // TODO why couldn't domain operate on dataspaces ?
    // e.g. for a position, we could want to limit its norm ?
    // Maybe the domain could be a list of constraint :
    // "default" min max constraint,
    // constraint added by the unit type (e.g. rgb : between 0 / 1)
    // additional constraints..
    Tuple t{T{(val_t)(min - 100)},
            min,
                T{(val_t)((min + max) / 2)},
                  max,
                      T{(val_t)(max + 100)}};
    for(int i = 0; i < 6; i++)
    {
      addr.setBoundingMode((bounding_mode)i);
      addr.pushValue(t);
    }
  }

  template<typename T>
  void test_clamp_tuple(ossia::net::address_base& addr, T min, T max)
  {
    domain_base<T> dom{min, max};

    addr.setDomain(dom);

    // min and max
    push_tuple(addr, min, max);

    // no min
    dom.min.reset();
    push_tuple(addr, min, max);

    // no max
    dom.min = min;
    dom.max.reset();
    push_tuple(addr, min, max);

    // nothing
    dom.min.reset();
    dom.max.reset();
    push_tuple(addr, min, max);
  }



  template<typename R>
  void test_clamp_lowlevel()
  {
    // cref
    auto a = to<R>(-1);
    auto b = to<R>(0);
    auto c = to<R>(0.5);
    auto d = to<R>(1);
    auto e = to<R>(10);
    auto f = to<R>(-1.3);
    auto g = to<R>(1.3);


    QCOMPARE(ossia::clamp(a,  b, d), b);
    QCOMPARE(ossia::clamp(b,   b, d), b);
    QCOMPARE(ossia::clamp(c, b, d), c);
    QCOMPARE(ossia::clamp(d,   b, d), d);
    QCOMPARE(ossia::clamp(e,  b, d), d);

    QCOMPARE(ossia::clamp_min(a,  b), b);
    QCOMPARE(ossia::clamp_min(b,   b), b);
    QCOMPARE(ossia::clamp_min(c, b), c);
    QCOMPARE(ossia::clamp_min(d,   b), d);
    QCOMPARE(ossia::clamp_min(e,  b), e);

    QCOMPARE(ossia::clamp_max(a,  d), a);
    QCOMPARE(ossia::clamp_max(b,   d), b);
    QCOMPARE(ossia::clamp_max(c, d), c);
    QCOMPARE(ossia::clamp_max(d,   d), d);
    QCOMPARE(ossia::clamp_max(e,  d), d);

    /** Needs fuzzy comparison for compare. **/
    (ossia::wrap(f,  b, d), (void) to<R>(0.7));
    (ossia::wrap(b,   b, d), (void) b);
    (ossia::wrap(c, b, d), (void) c);
    (ossia::wrap(g,   b, d), (void) to<R>(0.3));
    (ossia::wrap(e,  b, d), (void) b);

    (ossia::fold(f,  b, d), (void) to<R>(0.7));
    (ossia::fold(b,   b, d), (void) b);
    (ossia::fold(c, b, d), (void) c);
    (ossia::fold(g,   b, d), (void) to<R>(0.3));
    (ossia::fold(e,  b, d), (void) b);

    // rvalue
    QCOMPARE(ossia::clamp(to<R>(-1),  to<R>(0), to<R>(1)), to<R>(0));
    QCOMPARE(ossia::clamp(to<R>(0),   to<R>(0), to<R>(1)), to<R>(0));
    QCOMPARE(ossia::clamp(to<R>(0.5), to<R>(0), to<R>(1)), to<R>(0.5));
    QCOMPARE(ossia::clamp(to<R>(1),   to<R>(0), to<R>(1)), to<R>(1));
    QCOMPARE(ossia::clamp(to<R>(10),  to<R>(0), to<R>(1)), to<R>(1));

    QCOMPARE(ossia::clamp_min(to<R>(-1),  to<R>(0)), to<R>(0));
    QCOMPARE(ossia::clamp_min(to<R>(0),   to<R>(0)), to<R>(0));
    QCOMPARE(ossia::clamp_min(to<R>(0.5), to<R>(0)), to<R>(0.5));
    QCOMPARE(ossia::clamp_min(to<R>(1),   to<R>(0)), to<R>(1));
    QCOMPARE(ossia::clamp_min(to<R>(10),  to<R>(0)), to<R>(10));

    QCOMPARE(ossia::clamp_max(to<R>(-1),  to<R>(1)), to<R>(-1));
    QCOMPARE(ossia::clamp_max(to<R>(0),   to<R>(1)), to<R>(0));
    QCOMPARE(ossia::clamp_max(to<R>(0.5), to<R>(1)), to<R>(0.5));
    QCOMPARE(ossia::clamp_max(to<R>(1),   to<R>(1)), to<R>(1));
    QCOMPARE(ossia::clamp_max(to<R>(10),  to<R>(1)), to<R>(1));

    /** Needs fuzzy comparison for compare. **/
    (ossia::wrap(to<R>(-1.3),  to<R>(0), to<R>(1)), to<R>(0.7));
    (ossia::wrap(to<R>(0),   to<R>(0), to<R>(1)), to<R>(0));
    (ossia::wrap(to<R>(0.5), to<R>(0), to<R>(1)), to<R>(0.5));
    (ossia::wrap(to<R>(1.3),   to<R>(0), to<R>(1)), to<R>(0.3));
    (ossia::wrap(to<R>(10.),  to<R>(0), to<R>(1)), to<R>(0));

    (ossia::fold(to<R>(-1.3),  to<R>(0), to<R>(1)), to<R>(0.7));
    (ossia::fold(to<R>(0),   to<R>(0), to<R>(1)), to<R>(0));
    (ossia::fold(to<R>(0.5), to<R>(0), to<R>(1)), to<R>(0.5));
    (ossia::fold(to<R>(1.3),   to<R>(0), to<R>(1)), to<R>(0.3));
    (ossia::fold(to<R>(10.),  to<R>(0), to<R>(1)), to<R>(0));

  }
private Q_SLOTS:

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    domain_base<Int> dom(1, 24);

    QVERIFY(*dom.min  == 1);
    QVERIFY(*dom.max  == 24);

    doms = {1, 10, 24};
    QVERIFY(doms.size() == 3);

    auto copy = dom;
    QCOMPARE(copy, dom);

    domain d1 = dom, d2 = copy;
    QCOMPARE(d1, d2);

    d2 = domain_base<Int>{0, 25};
    QVERIFY(d1 != d2);

    d2 = domain_base<Float>{1., 24.};
    QVERIFY(d1 != d2);

    domain d3 = make_domain(Int(1), Int(24));
    QVERIFY(d3 == domain_base<Int>(1, 24));
    QVERIFY(d3 != dom);
  }

  void test_clamp_address()
  {
    TestUtils t;
    test_clamp_numeric(*t.float_addr, Float{0.}, Float{1.});
    test_clamp_numeric(*t.vec2f_addr, Float{0.}, Float{1.});
    test_clamp_numeric(*t.int_addr, Int{0}, Int{100});
    test_clamp_numeric(*t.bool_addr, Bool{false}, Bool{true});
    test_clamp_numeric(*t.char_addr, Char{'a'}, Char{'z'});

    test_clamp_tuple(*t.tuple_addr, Float{0.}, Float{1.});
    test_clamp_tuple(*t.tuple_addr, Int{0}, Int{100});
    test_clamp_tuple(*t.tuple_addr, Bool{false}, Bool{true});
    test_clamp_tuple(*t.tuple_addr, Char{'a'}, Char{'z'});


    test_clamp_vec<2>(*t.vec2f_addr, Float{0.}, Float{1.});
    test_clamp_vec<3>(*t.vec3f_addr, Float{0.}, Float{1.});
    test_clamp_vec<4>(*t.vec4f_addr, Float{0.}, Float{1.});

    test_clamp_vec(*t.vec2f_addr, make_vec(0., 0.), make_vec(1., 1.));
    test_clamp_vec(*t.vec3f_addr, make_vec(0., 0., 0.), make_vec(1., 1., 1.));
    test_clamp_vec(*t.vec4f_addr, make_vec(0., 0., 0., 0.), make_vec(1., 1., 1., 1.));

    // TODO test tuple with Tuple domain.

    test_clamp_lowlevel<Int>();
    test_clamp_lowlevel<Float>();
    test_clamp_lowlevel<Char>();
  }

  void test_clamp_float()
  {
    ossia::Float min{0.}, max{1.};
    auto dom = ossia::net::make_domain(min, max);

    for(float i = -100; i < 100; i++)
    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::FREE, Float{i});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, Float{i});
    }

    // Clip
    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::CLIP, Float{-100});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, min);
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::CLIP, Float{0});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, min);
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::CLIP, Float{0.5});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, Float{0.5});
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::CLIP, Float{1});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, max);
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::CLIP, Float{100});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, max);
    }

    // Low
    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::LOW, Float{-100});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, min);
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::LOW, Float{0});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, min);
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::LOW, Float{0.5});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, Float{0.5});
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::LOW, Float{1});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, Float{1});
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::LOW, Float{100});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, Float{100});
    }

    // High
    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::HIGH, Float{-100});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, Float{-100});
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::HIGH, Float{0});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res,  Float{0});
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::HIGH, Float{0.5});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, Float{0.5});
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::CLIP, Float{1});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, max);
    }

    {
      auto clamped = ossia::net::apply_domain(dom, ossia::bounding_mode::CLIP, Float{100});
      auto res = clamped.target<Float>();
      QVERIFY(res);
      QCOMPARE(*res, max);
    }


  }

  void test_get_min_max()
  {
    using namespace ossia::net;
    { // No domain
      domain d;
      QVERIFY(get_min(d) == value{});
      QVERIFY(get_max(d) == value{});


      set_min(d, Impulse{});
      QVERIFY(get_min(d) == value{});

      set_max(d, Impulse{});
      QVERIFY(get_max(d) == value{});

      set_min(d, Int{});
      QVERIFY(get_min(d) == value{});

      set_max(d, Int{});
      QVERIFY(get_max(d) == value{});

      // etc...
    }

    { // Empty domain
      domain d = domain_base<Impulse>();
      QVERIFY(get_min(d) == value{});
      QVERIFY(get_max(d) == value{});


      set_min(d, Impulse{});
      QVERIFY(get_min(d) == value{});

      set_max(d, Impulse{});
      QVERIFY(get_max(d) == value{});

      set_min(d, Int{});
      QVERIFY(get_min(d) == value{});

      set_max(d, Int{});
      QVERIFY(get_max(d) == value{});

      // etc...
    }

    { // Numeric domain
      domain d = domain_base<Int>();
      QVERIFY(get_min(d) == value{});
      QVERIFY(get_max(d) == value{});

      // No value
      set_min(d, Impulse{});
      QVERIFY(get_min(d) == value{});

      set_max(d, Impulse{});
      QVERIFY(get_max(d) == value{});

      // Correct domain
      set_min(d, Int{12});
      qDebug() << get_min(d);
      QVERIFY(get_min(d) == Int{12});

      set_max(d, Int{25});
      QVERIFY(get_max(d) == Int{25});

      // No value
      set_min(d, Float{7});
      qDebug() << get_min(d);
      QVERIFY(get_min(d) == value{});

      set_max(d, Float{42});
      QVERIFY(get_max(d) == value{});
    }

    { // Generic domain
      domain d = domain_base<ossia::value>();
      QVERIFY(get_min(d) == value{});
      QVERIFY(get_max(d) == value{});

      set_min(d, Impulse{});
      QVERIFY(get_min(d) == Impulse{});

      set_max(d, Impulse{});
      QVERIFY(get_max(d) == Impulse{});

      set_min(d, Int{12});
      qDebug() << get_min(d);
      QVERIFY(get_min(d) == Int{12});

      set_max(d, Int{25});
      QVERIFY(get_max(d) == Int{25});

      set_min(d, Float{7});
      qDebug() << get_min(d);
      QVERIFY(get_min(d) == Float{7});

      set_max(d, Float{42});
      QVERIFY(get_max(d) == Float{42});

      set_min(d, String{"foo"});
      qDebug() << get_min(d);
      QVERIFY(get_min(d) == String{"foo"});

      set_max(d, String{"bar"});
      QVERIFY(get_max(d) == String{"bar"});

      set_min(d, Tuple{Float{}, Int{}});
      qDebug() << get_min(d);
      QVERIFY(get_min(d) == (Tuple{Float{}, Int{}}));

      set_max(d, Tuple{Float{2}, Int{3}});
      QVERIFY(get_max(d) == (Tuple{Float{2}, Int{3}}));
    }
  }
};

QTEST_APPLESS_MAIN(DomainTest)

#include "DomainTest.moc"
