#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_node.hpp>
namespace ossia
{
namespace net
{
address::~address() = default;

static void getAddressFromNode_rec(
        const node& node,
        std::vector<std::string>& str)
{
    if(auto p = node.getParent())
        getAddressFromNode_rec(*p, str);

    str.push_back(node.getName());
}

std::string getAddressFromNode(const ossia::net::node& node)
{
    std::vector<std::string> vec;
    getAddressFromNode_rec(node, vec);

    // vec cannot be empty.

    int i = 0;

    std::string str;
    str.reserve(vec.size() * 5);
    str.append(vec.at(i++));
    str.append(":/");

    int n = vec.size();
    for(; i < n - 1; i++)
    {
        str.append(vec.at(i));
        str.append("/");
    }
    if((n - 1) > 0)
        str.append(vec.at(n-1));

    return str;
}

}
}
