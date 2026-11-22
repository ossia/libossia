// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>

#pragma once
// A path of its own under /tmp for each Unix socket of a test run, removed
// when the run ends: two runs at once (ctest -j, two tests on the same
// name), or the socket another user left there, do not meet on a fixed path.
// Under 104 bytes, the limit of sun_path on macOS.
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <mutex>
#include <string>
#include <vector>

namespace ossia_test
{
inline std::string unix_socket_path(const std::string& name)
{
  struct registry
  {
    std::mutex mutex;
    std::vector<std::string> paths;
    ~registry()
    {
      for(auto& p : paths)
        ::unlink(p.c_str());
    }
  };
  static registry r;
  static const std::string run
      = std::to_string(::getpid()) + "."
        + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count() % 1000000);
  std::string path = "/tmp/ossia_" + name + "." + run + ".socket";
  std::lock_guard lock{r.mutex};
  if(std::find(r.paths.begin(), r.paths.end(), path) == r.paths.end())
    r.paths.push_back(path);
  return path;
}
}
