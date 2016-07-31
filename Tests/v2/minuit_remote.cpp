#include <ossia/OSSIA.hpp>

int main()
{
  ossia::net::generic_device root(
        std::make_unique<ossia::net::minuit_protocol>("i-score", "127.0.0.1", 9996, 9997),
        "myDevice"
        );

  root.getProtocol().update(root);

}
