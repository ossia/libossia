#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/common/complex_type.hpp>

int main()
{
    ossia::oscquery_device dev;
    auto& root = dev.device.get_root_node();
    ossia::create_parameter(root, "int", "int");
    ossia::create_parameter(root, "float", "float");
    ossia::create_parameter(root, "bool", "bool");
    ossia::create_parameter(root, "char", "char");
    ossia::create_parameter(root, "string", "string");
    ossia::create_parameter(root, "vec2", "vec2f");
    ossia::create_parameter(root, "vec3", "vec3f");
    ossia::create_parameter(root, "vec4", "vec4f");

    ossia::create_parameter(root, "pos", "cartesian.xy");
    ossia::create_parameter(root, "polarpos", "polar");

    ossia::create_parameter(root, "argb", "argb");
    ossia::create_parameter(root, "rgba", "rgba");
    ossia::create_parameter(root, "argb8", "argb8");
    ossia::create_parameter(root, "rgba8", "rgba8");
    ossia::create_parameter(root, "hsv", "hsv");

    ossia::create_parameter(root, "db", "db");

    ossia::create_parameter(root, "meter", "meter");


    {
        auto f = ossia::create_parameter(root, "domain/minmax/float", "float");
        f->set_domain(ossia::make_domain(-100., 100.));
    }

    {
        auto f = ossia::create_parameter(root, "domain/minmax/int", "int");
        f->set_domain(ossia::make_domain(-100, 100));
    }
    while(1)
        std::this_thread::sleep_for(std::chrono::seconds(1));
}
