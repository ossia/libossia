#include <ossia/detail/instantiations.hpp>
#include <boost/container/static_vector.hpp>

template class std::vector<int>;
template class std::vector<std::string>;
template class std::array<float, 2>;
template class std::array<float, 3>;
template class std::array<float, 4>;
template class std::array<double, 2>;
template class std::array<double, 3>;
template class std::array<double, 4>;
template class boost::basic_string_ref<char, std::char_traits<char>>;
template class boost::optional<int>;
template class boost::optional<float>;
template class boost::optional<char>;
template class boost::optional<bool>;
template class std::vector<boost::string_ref>;
template class std::set<std::string, std::less<>>;
template class boost::container::static_vector<char, 8>;
