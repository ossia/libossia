#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_ASIO)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#include <windows.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// Diagnostics for ASIO driver discovery.
//
// Driver enumeration goes through the Steinberg SDK host helper
// (AsioDriverList in host/pc/asiolist.cpp), which walks HKLM\SOFTWARE\ASIO and
// silently drops any driver it cannot fully resolve. When that happens the user
// sees an empty device list with no explanation whatsoever.
//
// This header provides the ground truth -- a direct, self-contained read of the
// registry that does not go through the SDK at all -- so a mismatch between
// "what is installed" and "what the SDK returned" is reported instead of being
// invisible. Every registry call here is an explicit -A call operating on char
// buffers, so it stays correct regardless of how UNICODE is defined for the
// including translation unit.
namespace ossia::asio_diagnostics
{
// Verbose per-driver tracing, enabled with OSSIA_ASIO_DEBUG=1.
inline bool verbose() noexcept
{
  static const bool enabled = [] {
    const char* e = std::getenv("OSSIA_ASIO_DEBUG");
    return e && *e != '\0' && *e != '0';
  }();
  return enabled;
}

// All ASIO diagnostics go to stderr rather than through ossia::logger() or
// qDebug(): enumeration runs while the audio factories are being constructed,
// which on Windows is before the log sinks are usable, and score's Qt message
// handler redirects qDebug() to score.log on MSVC builds. stderr is the only
// channel that reliably reaches a console.
inline std::ostream& log()
{
  return std::cerr << "[asio] ";
}

struct registry_entry
{
  std::string key;   // subkey name under HKLM\SOFTWARE\ASIO
  std::string clsid; // "clsid" value; empty when the value is missing
  std::string dll;   // HKCR\CLSID\<clsid>\InprocServer32; empty when missing
  bool dll_exists{};

  // The SDK requires all three to resolve before it will list the driver.
  bool usable() const noexcept
  {
    return !clsid.empty() && !dll.empty() && dll_exists;
  }
};

namespace detail
{
inline std::string read_sz(HKEY key, const char* value)
{
  char buf[1024]{};
  DWORD type = REG_SZ;
  DWORD size = sizeof(buf) - 1;
  if(RegQueryValueExA(key, value, nullptr, &type, (LPBYTE)buf, &size) != ERROR_SUCCESS)
    return {};
  return std::string{buf};
}
}

// Enumerates HKLM\SOFTWARE\ASIO without involving the ASIO SDK.
//
// Note on bitness: a 64-bit process sees the 64-bit view, so 32-bit-only
// drivers registered under WOW6432Node are invisible here -- and equally
// invisible to the SDK, which is correct, as they cannot be loaded in-process.
inline std::vector<registry_entry> installed_drivers_from_registry()
{
  std::vector<registry_entry> out;

  HKEY root{};
  if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\ASIO", 0, KEY_READ, &root)
     != ERROR_SUCCESS)
    return out;

  for(DWORD index = 0;; index++)
  {
    char keyname[256]{};
    DWORD keylen = sizeof(keyname);
    if(RegEnumKeyExA(root, index, keyname, &keylen, nullptr, nullptr, nullptr, nullptr)
       != ERROR_SUCCESS)
      break;

    registry_entry e;
    e.key = keyname;

    HKEY sub{};
    if(RegOpenKeyExA(root, keyname, 0, KEY_READ, &sub) == ERROR_SUCCESS)
    {
      e.clsid = detail::read_sz(sub, "clsid");
      RegCloseKey(sub);
    }

    if(!e.clsid.empty())
    {
      const std::string path = "CLSID\\" + e.clsid + "\\InprocServer32";
      HKEY inproc{};
      if(RegOpenKeyExA(HKEY_CLASSES_ROOT, path.c_str(), 0, KEY_READ, &inproc)
         == ERROR_SUCCESS)
      {
        e.dll = detail::read_sz(inproc, nullptr);
        RegCloseKey(inproc);
      }
      if(!e.dll.empty())
        e.dll_exists = GetFileAttributesA(e.dll.c_str()) != INVALID_FILE_ATTRIBUTES;
    }

    out.push_back(std::move(e));
  }

  RegCloseKey(root);
  return out;
}

// Compares what the SDK returned against what is actually installed and reports
// anything unexplained. Called once per enumeration; cheap (a few registry
// reads).
inline void report(std::size_t sdk_count)
{
  const auto installed = installed_drivers_from_registry();

  std::size_t usable = 0;
  for(const auto& e : installed)
    if(e.usable())
      usable++;

  if(verbose())
  {
    log() << "HKLM\\SOFTWARE\\ASIO lists " << installed.size() << " driver(s):\n";
    for(const auto& e : installed)
    {
      log() << "  - \"" << e.key << "\"\n";
      log() << "      clsid: " << (e.clsid.empty() ? "<missing>" : e.clsid) << '\n';
      log() << "      dll  : " << (e.dll.empty() ? "<missing>" : e.dll)
            << (e.dll.empty() ? "" : (e.dll_exists ? " (present)" : " (NOT FOUND)"))
            << '\n';
    }
  }

  if(sdk_count == usable)
    return;

  // The interesting case: the registry says a driver should be loadable but the
  // SDK dropped it (or vice versa).
  log() << "WARNING: the ASIO SDK returned " << sdk_count << " driver(s) but "
        << usable << " of " << installed.size()
        << " registry entries look loadable.\n";

  for(const auto& e : installed)
  {
    if(e.usable())
      continue;
    log() << "  \"" << e.key << "\" is not loadable: ";
    if(e.clsid.empty())
      std::cerr << "no \"clsid\" value under HKLM\\SOFTWARE\\ASIO\\" << e.key;
    else if(e.dll.empty())
      std::cerr << "no HKCR\\CLSID\\" << e.clsid << "\\InprocServer32 entry";
    else
      std::cerr << "driver DLL is missing: " << e.dll;
    std::cerr << '\n';
  }

  if(sdk_count < usable)
  {
    log() << "  " << (usable - sdk_count)
          << " loadable driver(s) were dropped by the SDK host helper.\n";
    log() << "  This is what asiolist.cpp does when built with UNICODE defined: "
             "its char-buffer registry parsing reads UTF-16, so every CLSID "
             "comparison fails. Check that asio_sdk is compiled without "
             "UNICODE/_UNICODE.\n";
  }
}
}

#endif
