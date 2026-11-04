#pragma once
#include <ossia/detail/config.hpp>

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace ossia::net::hid
{

enum class report_type : uint8_t
{
  input,
  output,
  feature
};

//! One main item of a report descriptor: report_count values of report_size bits.
struct OSSIA_EXPORT report_field
{
  report_type type{};
  uint8_t report_id{};

  //! Offset in bits from the start of the report data, report id excluded.
  uint32_t bit_offset{};
  uint32_t report_size{};
  uint32_t report_count{};

  //! Main item flags: bit 0 constant, bit 1 variable, bit 2 relative.
  uint32_t flags{};

  int32_t logical_minimum{};
  int32_t logical_maximum{};

  //! Usages as (page << 16) | id. For a variable field, usage i applies to
  //! value i (the last one repeating); for an array field, they are the
  //! usages the values select from.
  std::vector<uint32_t> usages;

  //! The usage of each enclosing collection, outermost first.
  std::vector<uint32_t> collections;

  bool is_constant() const noexcept { return flags & 0x1; }
  bool is_variable() const noexcept { return flags & 0x2; }
  bool is_relative() const noexcept { return flags & 0x4; }
  bool is_signed() const noexcept { return logical_minimum < 0; }

  uint32_t usage_for(uint32_t index) const noexcept
  {
    if(usages.empty())
      return 0;
    return index < usages.size() ? usages[index] : usages.back();
  }
};

struct OSSIA_EXPORT report_descriptor
{
  std::vector<report_field> fields;

  bool uses_report_ids() const noexcept;

  //! Size in bytes of a report, report id excluded.
  std::size_t report_size(report_type t, uint8_t id) const noexcept;

  std::vector<uint8_t> report_ids(report_type t) const;
};

OSSIA_EXPORT
report_descriptor parse_report_descriptor(std::span<const uint8_t> data);

//! Reads value `index` of a field from report data (report id excluded).
OSSIA_EXPORT
int32_t extract_field(
    const report_field& f, uint32_t index, std::span<const uint8_t> data) noexcept;

//! Writes value `index` of a field into report data (report id excluded).
OSSIA_EXPORT
void insert_field(
    const report_field& f, uint32_t index, int32_t value, std::span<uint8_t> data) noexcept;

//! A short lowercase name for a usage, e.g. "x", "button.3", "volume_up",
//! or "page_ff00.usage_0001" when it is not known.
OSSIA_EXPORT
std::string usage_name(uint32_t usage);

OSSIA_EXPORT
std::string usage_page_name(uint16_t page);
}
