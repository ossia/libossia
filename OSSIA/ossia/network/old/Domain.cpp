#include <ossia/network/v1/detail/Domain_impl.hpp>
# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  std::shared_ptr<Domain> Domain::create(const Value& min,
                                    const Value& max,
                                    std::vector<Value> values)
  {
    return std::make_shared<impl::JamomaDomain>(min, max, values);
  }

  std::shared_ptr<Domain> Domain::create(const Value& min,
                                    const Value& max)
  {
    return create(min, max, {});
  }

  std::shared_ptr<Domain> Domain::create()
  {
    return create(Impulse{}, Impulse{});
  }

  Domain::~Domain()
  {}


}

