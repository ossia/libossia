#include <ossia/ossia.h>
#include <iostream>
#include <thread>
#include <chrono>
void valueCallback(ossia_value_t val) {
    ossia_type type = ossia_value_get_type(val);
    std::cerr << (int)type << ": \n";

    switch(type)
    {
    case ossia_type::IMPULSE:
        std::cerr << "impulse\n";
        break;
    case ossia_type::INT:
        std::cerr << "int: " << ossia_value_to_int(val) << "\n";
        break;
    case ossia_type::BOOL:
        std::cerr << "bool: " << ossia_value_to_bool(val) << "\n";
        break;
    case ossia_type::FLOAT:
        std::cerr << "float: " << ossia_value_to_float(val) << "\n";
        break;
    default:
        break;
    }
}
int main()
{
    using namespace std::literals::chrono_literals;
    auto local_protocol = ossia_protocol_local_create();
    auto local_dev = ossia_device_create(local_protocol, "newDevice");

    auto minuit_protocol = ossia_protocol_minuit_create("127.0.0.1", 13579, 9998);
    auto minuit_dev = ossia_device_create(minuit_protocol, "i-score");

    auto scene = ossia_device_add_child(local_dev, "scene");
    auto sphere = ossia_node_add_child(scene, "sphere");

    auto x = ossia_node_add_child(sphere, "x");
    auto y = ossia_node_add_child(sphere, "y");
    auto z = ossia_node_add_child(sphere, "z");

    auto x_a = ossia_node_create_address(x, ossia_type::FLOAT);
    auto y_a = ossia_node_create_address(y, ossia_type::FLOAT);
    auto z_a = ossia_node_create_address(z, ossia_type::FLOAT);

    ossia_address_push_value(x_a, ossia_value_create_float(1.3));
    ossia_address_push_value(y_a, ossia_value_create_float(2));
    ossia_address_push_value(z_a, ossia_value_create_float(3.3));

    //ossia_address_add_callback(x_a, valueCallback);
    while(1)
    {
        std::this_thread::sleep_for(1s);
        std::cerr << "in loop \n";

        valueCallback(ossia_address_pull_value(x_a));
    }


    ossia_device_free(minuit_dev);
    ossia_device_free(local_dev);

    return 0;
}
