#pragma once
#include <ossia/audio/audio_protocol.hpp>
namespace ossia
{
class dummy_engine final
    : public audio_engine
{
  public:
    void reload(audio_protocol* cur) override
    {

    }
    void stop() override
    {

    }
};

}
