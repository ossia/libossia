#pragma once

#if defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ <= 9
#define concept concept bool
#endif
#endif
