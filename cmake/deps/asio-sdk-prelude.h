// Force-included in every translation unit of the vendored Steinberg ASIO SDK
// (see asio-sdk.cmake).
//
// common/iasiodrv.h declares IASIO with the `interface` keyword and IUnknown
// as its base, but includes neither <windows.h> nor <unknwn.h>: it relies on
// the unabridged <windows.h> the SDK's own sources include, which reaches
// <ole2.h>. An embedder defining WIN32_LEAN_AND_MEAN globally, as score does,
// cuts <ole2.h> out and every file of the SDK stops compiling with "unknown
// type name 'interface'". <unknwn.h> brings <ole2.h> back on its own, whatever
// the define, so this does not depend on the order of the compiler's -D/-U
// flags, which differs between generators.
#pragma once
#include <windows.h>
#include <unknwn.h>
