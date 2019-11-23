#pragma once

namespace ossia
{

// https://stackoverflow.com/a/53367052/1495627
template<typename, template<typename...> class>
struct apply_type {};

template<
        template<typename...> class T,
        template<typename...> class List,
        typename ... Ts>
struct apply_type<List<Ts...>, T> {
    using type = List<T<Ts>...>;
};

}
