// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <iostream>
#include "TestUtils.hpp"

template<typename R, typename T>
ossia::value to(T f) { return R{(decltype(R{}))f}; }

class DomainTest : public QObject
{
  Q_OBJECT

  template<typename T>
  void push_all(ossia::net::parameter_base& addr, T min, T max)
  {
    using val_t = decltype(T{});
    for(int i = 0; i < 6; i++)
    {
      addr.set_bounding((ossia::bounding_mode)i);
      addr.push_value(T{(val_t)(min - 100)});
      addr.push_value(min);
      addr.push_value(T{(val_t)((min + max) / 2)});
      addr.push_value(max);
      addr.push_value(T{(val_t)(max + 100)});
    }
  }

  template<typename T>
  void test_clamp_numeric(ossia::net::parameter_base& addr, T min, T max)
  {
    auto dom = ossia::make_domain(min, max);
    addr.set_domain(dom);

    // min and max
    push_all(addr, min, max);

    // no min
    ossia::set_min(dom, {});
    push_all(addr, min, max);

    // no max
    ossia::set_min(dom, min);
    ossia::set_max(dom, {});
    push_all(addr, min, max);

    // nothing
    ossia::set_min(dom, {});
    ossia::set_max(dom, {});
    push_all(addr, min, max);
  }

  template<std::size_t N>
  void push_all_vec(ossia::net::parameter_base& addr, float min, float max)
  {
    using val_t = std::array<float, N>;
    std::vector<std::array<float, N>> test_vecs;
    for(float f : {min - 100, min, (min + max) / 2, max, max + 100})
    {
      val_t vec;
      vec.fill(f);
      test_vecs.push_back(vec);
    }

    for(int i = 0; i < 6; i++)
    {
      addr.set_bounding((ossia::bounding_mode)i);
      for(auto& vec : test_vecs)
        addr.push_value(vec);
    }
  }

  template<std::size_t N>
  void push_all_vec(ossia::net::parameter_base& addr, std::array<float, N> min, std::array<float, N> max)
  {
    using val_t = std::array<float, N>;
    std::vector<std::array<float, N>> test_vecs;
    for(float f : {min[0] - 100, min[0], (min[0] + max[0]) / 2, max[0], max[0] + 100})
    {
      val_t vec;
      vec.fill(f);
      test_vecs.push_back(vec);
    }

    for(int i = 0; i < 6; i++)
    {
      addr.set_bounding((ossia::bounding_mode)i);
      for(auto& vec : test_vecs)
        addr.push_value(vec);
    }
  }

  template<std::size_t N>
  void test_clamp_vec(ossia::net::parameter_base& addr, float min, float max)
  {
    auto dom = ossia::make_domain(min, max);
    addr.set_domain(dom);

    // min and max
    push_all_vec<N>(addr, min, max);

    // no min
    ossia::set_min(dom, {});
    push_all_vec<N>(addr, min, max);

    // no max
    ossia::set_min(dom, min);
    ossia::set_max(dom, {});
    push_all_vec<N>(addr, min, max);

    // nothing
    ossia::set_min(dom, {});
    ossia::set_max(dom, {});
    push_all_vec<N>(addr, min, max);
  }

  template<std::size_t N>
  void test_clamp_vec(ossia::net::parameter_base& addr, std::array<float, N> min, std::array<float, N> max)
  {
    auto dom = ossia::make_domain(min, max);
    addr.set_domain(dom);

    // min and max
    push_all_vec<N>(addr, min, max);

    // no min
    ossia::set_min(dom, {});
    push_all_vec<N>(addr, min, max);

    // no max
    ossia::set_min(dom, min);
    ossia::set_max(dom, {});
    push_all_vec<N>(addr, min, max);

    // nothing
    ossia::set_min(dom, {});
    ossia::set_max(dom, {});
    push_all_vec<N>(addr, min, max);
  }


  template<typename T>
  void push_tuple(ossia::net::parameter_base& addr, T min, T max)
  {
    using val_t = T;
    // TODO why couldn't domain operate on dataspaces ?
    // e.g. for a position, we could want to limit its norm ?
    // Maybe the domain could be a list of interval :
    // "default" min max interval,
    // interval added by the unit type (e.g. rgb : between 0 / 1)
    // additional intervals..
    std::vector<ossia::value> t{T{(val_t)(min - 100)},
            min,
                T{(val_t)((min + max) / 2)},
                  max,
                      T{(val_t)(max + 100)}};
    for(int i = 0; i < 6; i++)
    {
      addr.set_bounding((ossia::bounding_mode)i);
      addr.push_value(t);
    }
  }

  template<typename T>
  void test_clamp_tuple(ossia::net::parameter_base& addr, T min, T max)
  {
    ossia::domain_base<T> dom{min, max};

    addr.set_domain(dom);

    // min and max
    push_tuple(addr, min, max);

    // no min
    dom.min = ossia::none;
    push_tuple(addr, min, max);

    // no max
    dom.min = min;
    dom.max = ossia::none;
    push_tuple(addr, min, max);

    // nothing
    dom.min = ossia::none;
    dom.max = ossia::none;
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
private:

  void test_visit()
  {
    auto range = ossia::make_domain(-1, 1);
    struct my_vis {
      void operator()(ossia::domain_base<int>& d)
      {
        if(d.min) { }
        if(d.max) { }
        if(!d.values.empty()) { }
      }
      void operator()(ossia::domain_base<float>& d)
      {
        if(d.min) { }
        if(d.max) { }
        if(!d.values.empty()) { }
      }
      void operator()(ossia::domain_base<char>& d)
      {
        if(d.min) { }
        if(d.max) { }
        if(!d.values.empty()) { }
      }
      void operator()(ossia::domain_base<bool>& d)
      {
        if(d.min) { }
        if(d.max) { }
        if(!d.values.empty()) { }
      }
      void operator()(ossia::domain_base<ossia::impulse>& d)
      {
        // nothing to do
      }
      void operator()(ossia::domain_base<std::string> d)
      {
        if(!d.values.empty()) { }
      }
      void operator()(ossia::domain_base<ossia::value> d)
      {
        if(d.min) { }
        if(d.max) { }
        if(!d.values.empty()) { }
      }
      void operator()(ossia::vecf_domain<2>& d)
      {
        for(auto min : d.min) if(min) { }
        for(auto max : d.max) if(max) { }
        for(auto values : d.values) if(!values.empty()) { }
      }
      void operator()(ossia::vecf_domain<3>& d)
      {
        for(auto min : d.min) if(min) { }
        for(auto max : d.max) if(max) { }
        for(auto values : d.values) if(!values.empty()) { }
      }
      void operator()(ossia::vecf_domain<4>& d)
      {
        for(auto min : d.min) if(min) { }
        for(auto max : d.max) if(max) { }
        for(auto values : d.values) if(!values.empty()) { }
      }
      void operator()(ossia::vector_domain& d)
      {
        if(!d.min.empty()) { }
        if(!d.max.empty()) { }
        for(auto values : d.values) if(!values.empty()) { }
      }
      void operator()()
      {

      }
    } my_visitor;
    ossia::apply(my_visitor, range.v);
  }

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    using namespace ossia;
    domain_base<int> dom(1, 24);

    QVERIFY(*dom.min  == 1);
    QVERIFY(*dom.max  == 24);

    dom.values = {1, 10, 24};
    QVERIFY(dom.values.size() == 3);

    auto copy = dom;
    QCOMPARE(copy, dom);

    domain d1 = dom, d2 = copy;
    QCOMPARE(d1, d2);

    d2 = domain_base<int>{0, 25};
    QVERIFY(d1 != d2);

    d2 = domain_base<float>{1., 24.};
    QVERIFY(d1 != d2);

    domain d3 = make_domain(1, 24);
    QVERIFY(d3 == domain_base<int>(1, 24));
    QVERIFY(d3 != dom);
  }

  void test_tuple()
  {
    std::vector<ossia::value> t1{0, 0, 0};
    std::vector<ossia::value> t2{10, 10, 10};
    auto d4 = ossia::make_domain(t1, t2);
    auto res = d4.apply(ossia::bounding_mode::CLIP, std::vector<ossia::value>{-10, 5, 2000});
    QVERIFY((res == std::vector<ossia::value>{0, 5, 10}));
  }

  void test_clamp_address()
  {
    using namespace ossia;
    ossia::TestDevice t;
    test_clamp_numeric(*t.float_addr, float{0.}, float{1.});
    test_clamp_numeric(*t.vec2f_addr, float{0.}, float{1.});
    test_clamp_numeric(*t.int_addr, int{0}, int{100});
    test_clamp_numeric(*t.bool_addr, bool{false}, bool{true});
    test_clamp_numeric(*t.char_addr, char{'a'}, char{'z'});

    test_clamp_tuple(*t.tuple_addr, float{0.}, float{1.});
    test_clamp_tuple(*t.tuple_addr, int{0}, int{100});
    test_clamp_tuple(*t.tuple_addr, bool{false}, bool{true});
    test_clamp_tuple(*t.tuple_addr, char{'a'}, char{'z'});


    test_clamp_vec<2>(*t.vec2f_addr, float{0.}, float{1.});
    test_clamp_vec<3>(*t.vec3f_addr, float{0.}, float{1.});
    test_clamp_vec<4>(*t.vec4f_addr, float{0.}, float{1.});

    test_clamp_vec(*t.vec2f_addr, make_vec(0., 0.), make_vec(1., 1.));
    test_clamp_vec(*t.vec3f_addr, make_vec(0., 0., 0.), make_vec(1., 1., 1.));
    test_clamp_vec(*t.vec4f_addr, make_vec(0., 0., 0., 0.), make_vec(1., 1., 1., 1.));

    // TODO test tuple with Tuple domain.

    test_clamp_lowlevel<int>();
    test_clamp_lowlevel<float>();
    test_clamp_lowlevel<char>();
  }

  void test_clamp_float()
  {
    float min{0.}, max{1.};
    auto dom = ossia::make_domain(min, max);

    for(float i = -100; i < 100; i++)
    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::FREE, float{i});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, float{i});
    }

    // Clip
    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::CLIP, float{-100});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, min);
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::CLIP, float{0});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, min);
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::CLIP, float{0.5});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, float{0.5});
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::CLIP, float{1});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, max);
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::CLIP, float{100});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, max);
    }

    // Low
    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::LOW, float{-100});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, min);
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::LOW, float{0});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, min);
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::LOW, float{0.5});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, float{0.5});
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::LOW, float{1});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, float{1});
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::LOW, float{100});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, float{100});
    }

    // High
    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::HIGH, float{-100});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, float{-100});
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::HIGH, float{0});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res,  float{0});
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::HIGH, float{0.5});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, float{0.5});
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::CLIP, float{1});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, max);
    }

    {
      auto clamped = ossia::apply_domain(dom, ossia::bounding_mode::CLIP, float{100});
      auto res = clamped.target<float>();
      QVERIFY(res);
      QCOMPARE(*res, max);
    }


  }

  void test_get_min_max()
  {
    using namespace ossia;
    { // No domain
      domain d;
      QVERIFY(get_min(d) == value{});
      QVERIFY(get_max(d) == value{});


      set_min(d, impulse{});
      QVERIFY(get_min(d) == value{});

      set_max(d, impulse{});
      QVERIFY(get_max(d) == value{});

      set_min(d, int{});
      QVERIFY(get_min(d) == value{});

      set_max(d, int{});
      QVERIFY(get_max(d) == value{});

      // etc...
    }

    { // Empty domain
      domain d = domain_base<impulse>();
      QVERIFY(get_min(d) == impulse{});
      QVERIFY(get_max(d) == impulse{});


      set_min(d, impulse{});
      QVERIFY(get_min(d) == impulse{});

      set_max(d, impulse{});
      QVERIFY(get_max(d) == impulse{});

      set_min(d, int{});
      QVERIFY(get_min(d) == impulse{});

      set_max(d, int{});
      QVERIFY(get_max(d) == impulse{});

      // etc...
    }

    { // Numeric domain
      domain d = domain_base<int>();
      QVERIFY(get_min(d) == value{});
      QVERIFY(get_max(d) == value{});

      // No value
      set_min(d, impulse{});
      QVERIFY(get_min(d) == value{});

      set_max(d, impulse{});
      QVERIFY(get_max(d) == value{});

      // Correct domain
      set_min(d, int{12});
      std::cerr << get_min(d);
      QVERIFY(get_min(d) == int{12});

      set_max(d, int{25});
      QVERIFY(get_max(d) == int{25});

      // No value
      set_min(d, float{7});
      std::cerr << get_min(d);
      QVERIFY(get_min(d) == value{});

      set_max(d, float{42});
      QVERIFY(get_max(d) == value{});
    }

    { // Generic domain
      domain d = domain_base<ossia::value>();
      QVERIFY(get_min(d) == value{});
      QVERIFY(get_max(d) == value{});

      set_min(d, impulse{});
      QVERIFY(get_min(d) == impulse{});

      set_max(d, impulse{});
      QVERIFY(get_max(d) == impulse{});

      set_min(d, int{12});
      std::cerr << get_min(d);
      QVERIFY(get_min(d) == int{12});

      set_max(d, int{25});
      QVERIFY(get_max(d) == int{25});

      set_min(d, float{7});
      std::cerr << get_min(d);
      QVERIFY(get_min(d) == float{7});

      set_max(d, float{42});
      QVERIFY(get_max(d) == float{42});

      set_min(d, std::string{"foo"});
      std::cerr << get_min(d);
      QVERIFY(get_min(d) == std::string{"foo"});

      set_max(d, std::string{"bar"});
      QVERIFY(get_max(d) == std::string{"bar"});

      set_min(d, std::vector<ossia::value>{float{}, int{}});
      std::cerr << get_min(d);
      QVERIFY(get_min(d) == (std::vector<ossia::value>{float{}, int{}}));

      set_max(d, std::vector<ossia::value>{float{2}, int{3}});
      QVERIFY(get_max(d) == (std::vector<ossia::value>{float{2}, int{3}}));
    }
  }

  void test_string()
  {
    using namespace ossia;
    {
      domain d = domain_base<std::string>();
      ossia::set_values(d, {"foo", "bar", "baz"});

      QVERIFY(ossia::apply_domain(d, ossia::bounding_mode::CLIP, "foo") == std::string("foo"));
      QVERIFY(ossia::apply_domain(d, ossia::bounding_mode::CLIP, "blah") == ossia::value{});
    }

    {
      domain d = make_domain(std::vector<std::string>{"foo", "bar", "baz"});

      QVERIFY(ossia::apply_domain(d, ossia::bounding_mode::CLIP, "foo") == std::string("foo"));
      QVERIFY(ossia::apply_domain(d, ossia::bounding_mode::CLIP, "blah") == ossia::value{});
    }

    {
      domain d = make_domain(std::vector<std::string>{"foo", "bar", "baz"});

      QVERIFY(ossia::apply_domain(d, ossia::bounding_mode::CLIP, "foo") == std::string("foo"));
      QVERIFY(ossia::apply_domain(d, ossia::bounding_mode::CLIP, "blah") == ossia::value{});
    }


  }
};

QTEST_APPLESS_MAIN(DomainTest)

#include "DomainTest.moc"
