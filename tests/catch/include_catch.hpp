#pragma once
#if __has_include(<catch2/catch_all.hpp>)
// Catch 2 v3
#include <catch2/catch_all.hpp>
#else
// Catch 2 v2
#include "include_catch.hpp"
#endif
#include <iostream>
#include <fstream>

#if defined(_WIN32)
#include <winsock2.h>
static const struct ossia_test_winsock_owner {
  ossia_test_winsock_owner() { WSADATA d; ::WSAStartup(MAKEWORD(2, 2), &d); }
  ~ossia_test_winsock_owner() { ::WSACleanup(); }
} ossia_test_winsock_owner_instance;
#endif
