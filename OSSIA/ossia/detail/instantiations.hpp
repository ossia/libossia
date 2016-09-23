#pragma once
#include <vector>
#include <array>
#include <string>
#include <set>
#include <boost/optional.hpp>
#include <boost/utility/string_ref.hpp>
#include <ossia_export.h>
extern template class OSSIA_EXPORT std::vector<int>;
extern template class OSSIA_EXPORT std::vector<std::string>;
extern template class OSSIA_EXPORT std::array<float, 2>;
extern template class OSSIA_EXPORT std::array<float, 3>;
extern template class OSSIA_EXPORT std::array<float, 4>;
extern template class OSSIA_EXPORT std::array<double, 2>;
extern template class OSSIA_EXPORT std::array<double, 3>;
extern template class OSSIA_EXPORT std::array<double, 4>;
extern template class OSSIA_EXPORT boost::basic_string_ref<char, std::char_traits<char>>;
extern template class OSSIA_EXPORT boost::optional<int>;
extern template class OSSIA_EXPORT boost::optional<float>;
extern template class OSSIA_EXPORT boost::optional<char>;
extern template class OSSIA_EXPORT boost::optional<bool>;
extern template class OSSIA_EXPORT std::vector<boost::string_ref>;
extern template class OSSIA_EXPORT std::set<std::string, std::less<>>;
