#include "Editor/Domain_impl.hpp"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Domain> Domain::create(const Value& min,
                                    const Value& max,
                                    std::vector<Value> values)
  {
    return make_shared<impl::JamomaDomain>(min, max, values);
  }

  shared_ptr<Domain> Domain::create(const Value& min,
                                    const Value& max)
  {
    return create(min, max, {});
  }

  shared_ptr<Domain> Domain::create()
  {
    return create(Impulse{}, Impulse{});
  }

  Domain::~Domain()
  {}


}

