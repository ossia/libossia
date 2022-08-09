#pragma once
#include <ossia_export.h>

#include <string>

namespace ossia
{
OSSIA_EXPORT std::string get_commit_sha();
OSSIA_EXPORT std::string get_commit_date();
}
