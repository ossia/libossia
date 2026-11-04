#pragma once
#include <ossia/detail/config.hpp>

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace ossia::net
{

struct OSSIA_EXPORT hid_device_info
{
  std::string path;
  std::string serial;
  std::string manufacturer;
  std::string product;
  uint16_t vendor_id{};
  uint16_t product_id{};
  uint16_t release{};
  uint16_t usage_page{};
  uint16_t usage{};
  int interface_number{-1};

  //! Human-readable, unique among the devices currently connected.
  std::string display_name() const;
};

OSSIA_EXPORT
std::vector<hid_device_info> hid_enumerate(uint16_t vendor_id = 0, uint16_t product_id = 0);

//! Increases each time a HID device is plugged or unplugged, where the platform
//! can tell; 0 if it cannot.
OSSIA_EXPORT
uint32_t hid_device_change_count();

//! The raw report descriptor of a device. Only available on Linux (through
//! sysfs); empty elsewhere or if it cannot be read.
OSSIA_EXPORT
std::vector<uint8_t> hid_read_report_descriptor(std::string_view path);

struct hid_device_configuration
{
  //! If set, opened as is; otherwise the first device matching the other fields.
  std::string path;
  std::string serial;
  uint16_t vendor_id{};
  uint16_t product_id{};
  int interface_number{-1};
  //! 0 matches any.
  uint16_t usage_page{};
};

//! Resolves a configuration to a currently connected device.
OSSIA_EXPORT
std::optional<hid_device_info> hid_find_device(const hid_device_configuration&);

/**
 * A HID device opened for reading and writing reports.
 *
 * Input reports are delivered on a dedicated thread. write(), send_feature_report()
 * and get_feature_report() are synchronous and may be called from any thread.
 *
 * Output and feature report buffers start with the report id, 0 for devices
 * without numbered reports. Input reports start with the report id only when
 * the device uses numbered reports.
 */
class OSSIA_EXPORT hid_device
{
public:
  using input_callback = std::function<void(std::span<const uint8_t>)>;
  using error_callback = std::function<void(std::string_view)>;

  hid_device();
  hid_device(const hid_device&) = delete;
  hid_device& operator=(const hid_device&) = delete;
  ~hid_device();

  //! Throws std::runtime_error on failure.
  void open(const hid_device_configuration& conf);

  //! Starts delivering input reports. on_error is called once, from the
  //! reader thread, if the device goes away.
  void start(input_callback on_input, error_callback on_error, std::size_t max_report_size = 4096);

  void close();

  bool is_open() const noexcept { return m_handle != nullptr; }
  const hid_device_info& info() const noexcept { return m_info; }

  //! Number of bytes written, or -1.
  int write(std::span<const uint8_t> report);
  int send_feature_report(std::span<const uint8_t> report);

  //! report[0] must hold the report id; returns the number of bytes read, or -1.
  int get_feature_report(std::span<uint8_t> report);

  std::string last_error() const;

private:
  void reader_loop(std::size_t max_report_size);

  void* m_handle{};
  hid_device_info m_info;
  input_callback m_on_input;
  error_callback m_on_error;
  std::thread m_reader;
  std::atomic_bool m_running{false};
  mutable std::mutex m_io_mutex;
};

}
