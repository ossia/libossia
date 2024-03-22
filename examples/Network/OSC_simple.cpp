
/**
 * @brief OSC simple example using libossia
 * 
 * Edu Meneses (2024) - https://www.edumeneses.com 
 *                      http://github.com/edumeneses
 * 
 * This example creates a libossia OSC device and sends "/foo/bar i" every second
 * to port 9000. At each loop, it will add 1 to i, and as it pushes the new value, 
 * it will trigger the callback. The callback prints the new value and also watches 
 * for incoming OSC messages to manually set the value of i as it listens to port 9001 
 * (9000 + 1).
 */


// libossia headers
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/base/parameter_data.hpp>

// Dependencies
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <thread>

// OSC device parameters
std::string client_ip = "127.0.0.1";
int client_port = 9000;
int local_port = client_port + 1;
std::string device_name = "Puara_Gestures";

// This lambda function will print unhandled incoming messages
auto cb = [](ossia::string_view v, const ossia::value& val) {
    std::cout << "Unhandled message: " << v << " => " << ossia::value_to_pretty_string(val) << std::endl;
};

// Main code starts here
int main(int argc, char* argv[]) {

    // An argument (integer) can be passed to this program to set custom ports
    if (argc > 1) {
        try {
            client_port = boost::lexical_cast<int>(argv[1]);
            local_port = client_port + 1;
        }
        catch(...) {
        };
    }

    // Here we create or value...
    int i = 0;

    // ...and here we create our ossia device 
    ossia::net::generic_device device {
        std::make_unique<ossia::net::osc_protocol>(
            client_ip, client_port, local_port), device_name
    };

    // Now we can connect our lambda function to the proper device's callback
    device.on_unhandled_message.connect(&cb);
    
    // We are ready to create our node and attach a parameter to it
    auto& node = find_or_create_node(device, "/foo/bar");
    auto param = node.create_parameter(ossia::val_type::INT);

    // Finaly, we can create one last callback to receive messages sent to the node we
    // set before
    param->add_callback([&](const ossia::value& v) {
        // Let's print the new value
        std::cout << "New value received: " << ossia::value_to_pretty_string(v) << std::endl;
        // This is a safer method to ensure we will assign an int to i. Floats will be truncated
        // and strings will be ignored
        i = ossia::convert<int>(v); 
    });

    while(true) {

        // Let's push i to our parameter at each loop. this will also trigger the callback,
        // printing the new value
        param->push_value(i); 
        
        // Let's add 1 at each loop
        i += 1;

        // Let's wait 1 second to start the while loop again
        std::this_thread::sleep_for(std::chrono::seconds(1));
  };
}
