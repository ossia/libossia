#pragma once
#include <ossia-config.hpp>

#if defined(_MSC_VER)
#define packed_struct _Pragma("pack(push, 1)") struct
#define end_packed_struct _Pragma("pack(pop)")
#else
#define packed_struct struct __attribute__((packed))
#define end_packed_struct
#endif
