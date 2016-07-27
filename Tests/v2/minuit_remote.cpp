#include <ossia/OSSIA.hpp>

int main()
{
  impl::BasicDevice root(
        std::make_unique<impl::Minuit2>("127.0.0.1", 9996, 9997),
        "myDevice"
        );

  root.getProtocol().updateChildren(root);

}
