#pragma once

#if defined(_MSC_VER)
#define OSSIA_DECL_RELAXED_CONSTEXPR
#else
#define OSSIA_DECL_RELAXED_CONSTEXPR constexpr
#endif
