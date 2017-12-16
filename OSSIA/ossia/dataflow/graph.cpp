// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/audio_parameter.hpp>
namespace ossia
{

audio_parameter::~audio_parameter()
{
}

#if defined(OSSIA_PROTOCOL_MIDI)
midi_generic_parameter::~midi_generic_parameter()
{
}
#endif

texture_generic_parameter::~texture_generic_parameter()
{
}
}
