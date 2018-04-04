#pragma once
#include <ossia/detail/config.hpp>

#include <vector>
#include <tuple>
#include <list>
#include <iostream>
#include <utility>
#include <brigand/algorithms/transform.hpp>
#include <brigand/algorithms/for_each.hpp>
#include <brigand/sequences/make_sequence.hpp>

namespace ossia
{
template<
    template<typename...> typename Sequence,
    template<typename...> typename Tuple,
    typename... Args
>
auto list_of_tuple_to_tuple_of_list(const Sequence<Tuple<Args...>>& lst)
{
    namespace bg = brigand;

    // res_type = Tuple<Sequence<Arg1>, Sequence<Arg2>, ...>
    using res_type =
        bg::transform<
            Tuple<Args...>,
            bg::bind<Sequence, bg::_1>
        >;

    // list of integers over which we iterate
    using index_list =
        bg::make_sequence<
            bg::size_t<0>,
            sizeof...(Args)
        >;

    res_type res;
    // TODO reserve sizeof...(Args)

    // copy our data
    for(const auto& tuple : lst)
    {
        bg::for_each<index_list>(
            [&] (auto t) {
              constexpr auto N = decltype(t)::type::value;
              std::get<N>(res).push_back(std::get<N>(tuple));
        });
    }
    return res;
}

}
