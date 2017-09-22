#pragma once
#include <QString>
#include <vector>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
OSSIA_EXPORT void sanitize_device_name(QString& str);
OSSIA_EXPORT void sanitize_name(QString& str);
OSSIA_EXPORT QString sanitize_name(
        QString name_base,
        const std::vector<QString>& brethren);
}
}

OSSIA_EXPORT
bool latin_compare(const QString& qstr, const std::string& str);
