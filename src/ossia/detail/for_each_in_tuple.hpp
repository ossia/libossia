#pragma once
#include <utility>
#include <tuple>
#include <array>

namespace ossia
{

template <class F, class T1, std::size_t... I1s, class T2>
void for_each_in_tuples_impl(
    T1&& t1,
    T2&& t2,
    F&& func,
    std::index_sequence<I1s...>
    )
{
  using namespace std;
  (forward<F>(func)(
       get<I1s>(forward<T1>(t1)),
       get<I1s>(forward<T2>(t2))
       ),
   ...);
}

template <class F, template<class...> class T1, class... T1s, template<class...> class T2, class... T2s>
void for_each_in_tuples(T1<T1s...>&& t1, T2<T2s...>&& t2, F&& func)
{
  static_assert(sizeof...(T1s) == sizeof...(T2s));

  using namespace std;
  for_each_in_tuples_impl(
      move<T1<T1s...>>(t1),
      move<T2<T2s...>>(t2),
      forward<F>(func),
      make_index_sequence<sizeof...(T1s)>()
      );
}

template <class F>
void for_each_in_tuples(const std::tuple<>& , const std::tuple<>& , const F& )
{
}

template <class F, template<class...> class T1, class... T1s, std::size_t... I1s, template<class...> class T2, class... T2s>
void for_each_in_tuples_ref_impl(
    T1<T1s...>& t1,
    T2<T2s...>& t2,
    F&& func,
    std::index_sequence<I1s...>
    )
{
  using namespace std;
  (forward<F>(func)(
       get<I1s>(t1),
       get<I1s>(t2)
       ),
   ...);
}

template <class F, template<class...> class T1, class... T1s, template<class...> class T2, class... T2s>
void for_each_in_tuples_ref(T1<T1s...>& t1, T2<T2s...>& t2, F&& func)
{
  static_assert(sizeof...(T1s) == sizeof...(T2s));

  for_each_in_tuples_ref_impl(
      t1, t2,
      std::forward<F>(func),
      std::make_index_sequence<sizeof...(T1s)>()
      );
}


template < template<class...> class T1, class... T1s, std::size_t... I1s, template<class...> class T2, class... T2s>
auto concat_tuples_impl(
    T1<T1s...>& t1,
    T2<T2s...>& t2,
    std::index_sequence<I1s...>
    )
{
  using namespace std;
  return make_tuple(make_pair(get<I1s>(t1), std::get<I1s>(t2)) ...);
}

template <template<class...> class T1, class... T1s, template<class...> class T2, class... T2s>
void concat_tuples(T1<T1s...>& t1, T2<T2s...>& t2)
{
  static_assert(sizeof...(T1s) == sizeof...(T2s));

  concat_tuples_impl(
      t1, t2, std::make_index_sequence<sizeof...(T1s)>()
  );
}


template <class F, template<class...> class T1, class... T1s, std::size_t... I1s, typename U, std::size_t N>
void tuple_array_func_impl(
    T1<T1s...>& t1,
    std::array<U, N>& t2,
    F&& func,
    std::index_sequence<I1s...>
    )
{
  using namespace std;
  (forward<F>(func)(
       get<I1s>(t1),
       t2[I1s]
       ),
   ...);
}

template <class F, template<class...> class T1, class... T1s, typename U, std::size_t N>
void tuple_array_func(T1<T1s...>&& t1, std::array<U, N>& t2, F&& func)
{
  static_assert(sizeof...(T1s) == N);

  tuple_array_func_impl(
      t1, t2, std::forward<F>(func), std::make_index_sequence<N>{});
}

}
