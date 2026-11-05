#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/protocols/hid/hid_device.hpp>
#include <ossia/protocols/hid/hid_report_descriptor.hpp>

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace ossia::net
{
class device_base;
class parameter_base;

/**
 * Generic HID device.
 *
 * Where the report descriptor can be read, each input and output usage gets
 * a parameter under /input and /output. Raw reports are always available
 * under /raw:
 * - /raw/input: every input report, as a list of bytes (report id first when
 *   the device numbers its reports)
 * - /raw/output: a list of bytes, report id first, written as an output report
 * - /raw/feature/send: a list of bytes, report id first, sent as a feature report
 * - /raw/feature/get: a report id; the report read back lands in /raw/feature/value
 */
class OSSIA_EXPORT hid_protocol final : public ossia::net::protocol_base
{
public:
  hid_protocol(ossia::net::network_context_ptr ctx, const hid_device_configuration& conf);
  hid_protocol(const hid_protocol&) = delete;
  hid_protocol& operator=(const hid_protocol&) = delete;
  ~hid_protocol() override;

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base&, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base&) override;
  bool connected() const noexcept override;
  void stop() override;

  const hid_device_info& info() const noexcept { return m_hid.info(); }
  const hid::report_descriptor& descriptor() const noexcept { return m_descriptor; }

private:
  struct input_binding
  {
    const hid::report_field* field{};
    uint32_t index{};
    uint32_t usage{};
    ossia::net::parameter_base* parameter{};
    enum kind_t
    {
      value,
      flag,
      list,
      selection
    } kind{value};
    int32_t last{};
    bool has_last{};
  };

  struct output_binding
  {
    const hid::report_field* field{};
    uint32_t index{};
    bool list{};
  };

  void build_inputs(ossia::net::node_base& root);
  void build_outputs(ossia::net::node_base& root);
  void on_input(std::span<const uint8_t> report);
  void write_output_report(uint8_t id);
  void write_raw(const ossia::value& v, bool feature);
  void read_feature(int id);

  ossia::net::network_context_ptr m_ctx;
  hid_device m_hid;
  hid::report_descriptor m_descriptor;
  ossia::net::device_base* m_device{};

  std::vector<input_binding> m_inputs;
  std::map<const ossia::net::parameter_base*, output_binding> m_outputs;
  std::map<uint8_t, std::vector<uint8_t>> m_output_reports;
  std::mutex m_output_mutex;

  ossia::net::parameter_base* m_raw_input{};
  ossia::net::parameter_base* m_raw_output{};
  ossia::net::parameter_base* m_raw_feature_send{};
  ossia::net::parameter_base* m_raw_feature_get{};
  ossia::net::parameter_base* m_raw_feature_value{};

  std::atomic_bool m_connected{false};
};

}
