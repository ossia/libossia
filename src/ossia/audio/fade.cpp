#include "fade.hpp"
#include <cmath>

namespace ossia::snd
{
void do_fade(
    bool start_discontinuous, bool end_discontinuous, audio_channel& ap,
    std::size_t start, std::size_t end)
{
  using namespace std;
  if(end < start)
    swap(start, end);
  const auto decrement = (1. / (end - start));
  double gain = 1.0;

  if(!start_discontinuous && !end_discontinuous)
    return;
  else if(start_discontinuous && !end_discontinuous)
  {
    for(std::size_t j = start; j < end; j++)
    {
      ap[j] *= (1. - gain);
      gain -= decrement;
    }
  }
  else if(!start_discontinuous && end_discontinuous)
  {
    for(std::size_t j = start; j < end; j++)
    {
      ap[j] *= std::pow(2., gain) - 1.;
      gain -= decrement;
    }
  }
  else
  {
    for(std::size_t j = start; j < end; j++)
    {
      ap[j] *= (2. * (gain * (1. - gain)));
      gain -= decrement;
    }
  }
}

}
