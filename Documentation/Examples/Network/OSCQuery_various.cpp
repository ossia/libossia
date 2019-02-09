#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/common/complex_type.hpp>

int main()
{
    ossia::oscquery_device dev;
    auto& root = dev.device.get_root_node();
    auto i = ossia::create_parameter(root, "int", "int");
    auto f = ossia::create_parameter(root, "float", "float");
    auto b = ossia::create_parameter(root, "bool", "bool");
    auto c = ossia::create_parameter(root, "char", "char");
    auto s = ossia::create_parameter(root, "string", "string");
    auto v2 = ossia::create_parameter(root, "vec2", "vec2f");
    auto v3 = ossia::create_parameter(root, "vec3", "vec3f");
    auto v4 = ossia::create_parameter(root, "vec4", "vec4f");

    ossia::create_parameter(root, "pos", "cartesian.xy");
    ossia::create_parameter(root, "polarpos", "polar");

    ossia::create_parameter(root, "argb", "argb");
    ossia::create_parameter(root, "rgba", "rgba");
    ossia::create_parameter(root, "argb8", "argb8");
    ossia::create_parameter(root, "rgba8", "rgba8");
    ossia::create_parameter(root, "hsv", "hsv");

    ossia::create_parameter(root, "db", "db");

    ossia::create_parameter(root, "meter", "meter");
    while(1)
        std::this_thread::sleep_for(std::chrono::seconds(1));
}
