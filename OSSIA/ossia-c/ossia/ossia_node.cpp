#include "ossia_utils.hpp"

extern "C"
{

void ossia_node_remove_child(
        ossia_node_t node,
        ossia_node_t child)
{
    return safe_function(__func__, [=] {
        if(!node)
            return;
        if(!child)
            return;

        auto& cld = node->node->children();
        std::string node_name = child->node->getName();
        auto it = std::find_if(cld.begin(), cld.end(),
                               [&] (const auto& node) {
            return node->getName() == node_name;
        });

        if(it != cld.end())
        {
            node->node->erase(it);
        }
        delete child;
    });
}

const char* ossia_node_get_name(ossia_node_t node)
{
    return safe_function(__func__, [=] () -> const char* {
        if(!node)
            return nullptr;

        if(!node->node)
            return nullptr;
        return strdup(node->node->getName().c_str());
    });
}

ossia_node_t ossia_node_add_child(
        ossia_node_t node,
        const char* name)
{
    return safe_function(__func__, [=] () -> ossia_node_t {
        if(!node)
            return nullptr;

        auto it = node->node->emplace(node->node->children().end(), name);
        auto child_node = *it;

        return new ossia_node{child_node};
    });
}

void ossia_node_free(ossia_node_t node)
{
    return safe_function(__func__, [=] {
        delete node;
    });
}

int ossia_node_child_size(ossia_node_t node)
{
    return safe_function(__func__, [=] {
        if(!node)
            return std::size_t(0);

        return node->node->children().size();
    });
}

ossia_node_t ossia_node_get_child(ossia_node_t node, int child_n)
{
    return safe_function(__func__, [=] () -> ossia_node_t {
        if(!node)
            return {};

        if(node->node->children().size() < child_n)
            return nullptr;

        return new ossia_node{node->node->children()[child_n]};
    });
}

ossia_address_t ossia_node_create_address(
        ossia_node_t node,
        ossia_type type)
{
    return safe_function(__func__, [=] () -> ossia_address_t {
        if(!node)
            return nullptr;

        return new ossia_address{node->node->createAddress(convert(type))};
    });
}

void ossia_node_remove_address(
        ossia_node_t node,
        ossia_address_t address)
{
    return safe_function(__func__, [=] {
        if(!node)
            return;

        node->node->removeAddress();
        delete address;
    });
}

}
