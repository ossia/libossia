#include <ossia/detail/config.hpp>

// The one and only translation unit that compiles the miniaudio single-header
// implementation. Everywhere else miniaudio_protocol.hpp only brings in the
// declarations. Config macros (backends, MA_MAX_CHANNELS, MA_API) come from
// miniaudio_protocol.hpp so they stay identical between here and every user.
#if __has_include(<miniaudio.h>)
#define MINIAUDIO_IMPLEMENTATION
#include <ossia/audio/miniaudio_protocol.hpp>
#endif
