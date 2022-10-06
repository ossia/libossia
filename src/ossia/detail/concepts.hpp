#pragma once

#if defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ <= 9
#define concept concept bool
#endif
#endif

#if __cpp_concepts >= 201907L
#define OSSIA_HAS_CONCEPTS 1
#define OSSIA_WRAP_CONCEPT(...) __VA_ARGS__
#else
#define OSSIA_WRAP_CONCEPT(...)
#endif
