#include "hid_protocol.hpp"

#include <ossia/detail/fmt.hpp>

#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/value/value.hpp>

#include <algorithm>

namespace ossia::net
{
namespace
{
constexpr uint32_t max_selection_usages = 512;
constexpr uint32_t list_threshold = 16;

bool is_list_field(const hid::report_field& f)
{
  return f.is_variable() && f.report_count > list_threshold && f.usages.size() <= 1;
}

bool is_reserved_selection(uint32_t usage)
{
  const uint16_t page = uint16_t(usage >> 16);
  const uint16_t id = uint16_t(usage & 0xFFFF);
  if(id == 0)
    return true;
  return page == 0x07 && id <= 0x03;
}

std::string field_name(uint32_t usage)
{
  if(usage == 0)
    return "data";
  return hid::usage_name(usage);
}

std::vector<ossia::value> to_list(std::span<const uint8_t> bytes)
{
  std::vector<ossia::value> res;
  res.reserve(bytes.size());
  for(auto b : bytes)
    res.emplace_back(int32_t(b));
  return res;
}

std::vector<uint8_t> to_bytes(const ossia::value& v)
{
  std::vector<uint8_t> res;
  if(auto l = v.target<std::vector<ossia::value>>())
  {
    res.reserve(l->size());
    for(const auto& e : *l)
      res.push_back(uint8_t(ossia::convert<int>(e)));
  }
  else if(auto s = v.target<std::string>())
  {
    res.assign(s->begin(), s->end());
  }
  return res;
}
}

hid_protocol::hid_protocol(
    ossia::net::network_context_ptr ctx, const hid_device_configuration& conf)
    : protocol_base{flags{}}
    , m_ctx{std::move(ctx)}
{
  m_hid.open(conf);
  const auto raw = hid_read_report_descriptor(m_hid.info().path);
  m_descriptor = hid::parse_report_descriptor(raw);
  m_connected = true;
}

hid_protocol::~hid_protocol()
{
  stop();
}

void hid_protocol::stop()
{
  m_hid.close();
  m_connected = false;
}

bool hid_protocol::connected() const noexcept
{
  return m_connected;
}

void hid_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;
  auto& root = dev.get_root_node();

  {
    const auto& info = m_hid.info();
    auto make_info = [&](std::string_view name, const std::string& v) {
      auto p = create_node(root, fmt::format("info/{}", name)).create_parameter(val_type::STRING);
      p->set_value(v);
      p->set_access(access_mode::GET);
    };
    make_info("product", info.product);
    make_info("manufacturer", info.manufacturer);
    make_info("serial", info.serial);
    make_info("path", info.path);
  }

  build_inputs(root);
  build_outputs(root);

  m_raw_input = create_node(root, "raw/input").create_parameter(val_type::LIST);
  m_raw_input->set_access(access_mode::GET);
  m_raw_output = create_node(root, "raw/output").create_parameter(val_type::LIST);
  m_raw_output->set_access(access_mode::SET);
  m_raw_feature_send
      = create_node(root, "raw/feature/send").create_parameter(val_type::LIST);
  m_raw_feature_send->set_access(access_mode::SET);
  m_raw_feature_get = create_node(root, "raw/feature/get").create_parameter(val_type::INT);
  m_raw_feature_get->set_access(access_mode::SET);
  m_raw_feature_get->set_domain(make_domain(0, 255));
  m_raw_feature_value
      = create_node(root, "raw/feature/value").create_parameter(val_type::LIST);
  m_raw_feature_value->set_access(access_mode::GET);

  m_hid.start(
      [this](std::span<const uint8_t> report) { on_input(report); },
      [this](std::string_view) {
    m_connected = false;
    on_connection_closed();
  });
}

void hid_protocol::build_inputs(ossia::net::node_base& root)
{
  std::vector<std::pair<uint8_t, uint32_t>> variable_usages;
  for(const auto& f : m_descriptor.fields)
    if(f.type == hid::report_type::input && !f.is_constant() && f.is_variable())
      for(uint32_t i = 0; i < f.report_count; i++)
        variable_usages.emplace_back(f.report_id, f.usage_for(i));

  for(const auto& f : m_descriptor.fields)
  {
    if(f.type != hid::report_type::input || f.is_constant())
      continue;

    if(is_list_field(f))
    {
      auto p = create_node(root, "input/" + field_name(f.usage_for(0)))
                   .create_parameter(val_type::LIST);
      p->set_access(access_mode::GET);
      m_inputs.push_back({.field = &f, .parameter = p, .kind = input_binding::list});
    }
    else if(f.is_variable())
    {
      for(uint32_t i = 0; i < f.report_count; i++)
      {
        const uint32_t usage = f.usage_for(i);
        auto& node = create_node(root, "input/" + field_name(usage));
        const bool flag = f.report_size == 1;
        auto p = node.create_parameter(flag ? val_type::BOOL : val_type::INT);
        p->set_access(access_mode::GET);
        if(!flag)
          p->set_domain(make_domain(f.logical_minimum, f.logical_maximum));
        m_inputs.push_back(
            {.field = &f,
             .index = i,
             .usage = usage,
             .parameter = p,
             .kind = flag ? input_binding::flag : input_binding::value});
      }
    }
    else if(f.usages.size() <= max_selection_usages)
    {
      for(uint32_t i = 0; i < f.usages.size(); i++)
      {
        if(is_reserved_selection(f.usages[i]))
          continue;
        if(std::find(
               variable_usages.begin(), variable_usages.end(),
               std::pair{f.report_id, f.usages[i]})
           != variable_usages.end())
          continue;
        auto p = create_node(root, "input/" + field_name(f.usages[i]))
                     .create_parameter(val_type::BOOL);
        p->set_access(access_mode::GET);
        m_inputs.push_back(
            {.field = &f,
             .index = i,
             .usage = f.usages[i],
             .parameter = p,
             .kind = input_binding::selection});
      }
    }
    else
    {
      const auto page = uint16_t(f.usage_for(0) >> 16);
      auto p = create_node(root, "input/" + hid::usage_page_name(page) + "/active")
                   .create_parameter(val_type::LIST);
      p->set_access(access_mode::GET);
      m_inputs.push_back({.field = &f, .parameter = p, .kind = input_binding::list});
    }
  }
}

void hid_protocol::build_outputs(ossia::net::node_base& root)
{
  for(auto id : m_descriptor.report_ids(hid::report_type::output))
  {
    std::vector<uint8_t> report(1 + m_descriptor.report_size(hid::report_type::output, id));
    report[0] = id;
    m_output_reports[id] = std::move(report);
  }

  for(const auto& f : m_descriptor.fields)
  {
    if(f.type != hid::report_type::output || f.is_constant() || !f.is_variable())
      continue;

    if(is_list_field(f))
    {
      auto p = create_node(root, "output/" + field_name(f.usage_for(0)))
                   .create_parameter(val_type::LIST);
      p->set_access(access_mode::SET);
      m_outputs[p] = {.field = &f, .list = true};
      continue;
    }

    for(uint32_t i = 0; i < f.report_count; i++)
    {
      auto& node = create_node(root, "output/" + field_name(f.usage_for(i)));
      const bool flag = f.report_size == 1;
      auto p = node.create_parameter(flag ? val_type::BOOL : val_type::INT);
      p->set_access(access_mode::SET);
      if(!flag)
        p->set_domain(make_domain(f.logical_minimum, f.logical_maximum));
      m_outputs[p] = {.field = &f, .index = i};
    }
  }
}

void hid_protocol::on_input(std::span<const uint8_t> report)
{
  if(report.empty() || !m_device)
    return;

  if(m_raw_input)
  {
    m_raw_input->set_value(to_list(report));
    m_device->on_message(*m_raw_input);
  }

  uint8_t id = 0;
  auto data = report;
  if(m_descriptor.uses_report_ids())
  {
    id = report[0];
    data = report.subspan(1);
  }

  for(auto& b : m_inputs)
  {
    const auto& f = *b.field;
    if(f.report_id != id)
      continue;

    switch(b.kind)
    {
      case input_binding::list: {
        std::vector<ossia::value> values;
        values.reserve(f.report_count);
        if(f.is_variable())
        {
          for(uint32_t i = 0; i < f.report_count; i++)
            values.emplace_back(hid::extract_field(f, i, data));
        }
        else
        {
          for(uint32_t i = 0; i < f.report_count; i++)
          {
            const int32_t v = hid::extract_field(f, i, data);
            const int32_t idx = v - f.logical_minimum;
            if(v >= f.logical_minimum && v <= f.logical_maximum
               && idx < int32_t(f.usages.size()) && (f.usages[idx] & 0xFFFF) != 0)
              values.emplace_back(int32_t(f.usages[idx] & 0xFFFF));
          }
        }
        b.parameter->set_value(std::move(values));
        m_device->on_message(*b.parameter);
        break;
      }

      case input_binding::selection: {
        int32_t pressed = 0;
        for(uint32_t i = 0; i < f.report_count; i++)
        {
          if(hid::extract_field(f, i, data) - f.logical_minimum == int32_t(b.index))
          {
            pressed = 1;
            break;
          }
        }
        if(b.has_last && b.last == pressed)
          break;
        b.last = pressed;
        b.has_last = true;
        b.parameter->set_value(bool(pressed));
        m_device->on_message(*b.parameter);
        break;
      }

      case input_binding::flag:
      case input_binding::value: {
        const int32_t v = hid::extract_field(f, b.index, data);
        if(b.has_last && b.last == v)
          break;
        b.last = v;
        b.has_last = true;
        if(b.kind == input_binding::flag)
          b.parameter->set_value(bool(v));
        else
          b.parameter->set_value(v);
        m_device->on_message(*b.parameter);
        break;
      }
    }
  }
}

void hid_protocol::write_output_report(uint8_t id)
{
  auto it = m_output_reports.find(id);
  if(it != m_output_reports.end())
    m_hid.write(it->second);
}

void hid_protocol::write_raw(const ossia::value& v, bool feature)
{
  auto bytes = to_bytes(v);
  if(bytes.empty())
    return;
  if(feature)
    m_hid.send_feature_report(bytes);
  else
    m_hid.write(bytes);
}

void hid_protocol::read_feature(int id)
{
  if(id < 0 || id > 255 || !m_raw_feature_value)
    return;

  std::size_t size = m_descriptor.report_size(hid::report_type::feature, uint8_t(id));
  if(size == 0)
    size = 255;
  std::vector<uint8_t> buf(size + 1);
  buf[0] = uint8_t(id);
  const int r = m_hid.get_feature_report(buf);
  if(r <= 0)
    return;
  buf.resize(r);
  m_raw_feature_value->set_value(to_list(buf));
  if(m_device)
    m_device->on_message(*m_raw_feature_value);
}

bool hid_protocol::push(const ossia::net::parameter_base& p, const ossia::value& v)
{
  if(&p == m_raw_output)
  {
    write_raw(v, false);
    return true;
  }
  if(&p == m_raw_feature_send)
  {
    write_raw(v, true);
    return true;
  }
  if(&p == m_raw_feature_get)
  {
    read_feature(ossia::convert<int>(v));
    return true;
  }

  auto it = m_outputs.find(&p);
  if(it == m_outputs.end())
    return false;

  const auto& b = it->second;
  const auto& f = *b.field;
  std::lock_guard lock{m_output_mutex};
  auto& report = m_output_reports[f.report_id];
  auto data = std::span<uint8_t>{report}.subspan(1);
  if(b.list)
  {
    const auto bytes = to_bytes(v);
    for(uint32_t i = 0; i < f.report_count && i < bytes.size(); i++)
      hid::insert_field(f, i, bytes[i], data);
  }
  else
  {
    const int32_t value = std::clamp(
        ossia::convert<int>(v), f.logical_minimum, f.logical_maximum);
    hid::insert_field(f, b.index, value, data);
  }
  write_output_report(f.report_id);
  return true;
}

bool hid_protocol::pull(ossia::net::parameter_base&)
{
  return false;
}

bool hid_protocol::push_raw(const ossia::net::full_parameter_data&)
{
  return false;
}

bool hid_protocol::observe(ossia::net::parameter_base&, bool)
{
  return false;
}

bool hid_protocol::update(ossia::net::node_base&)
{
  return false;
}

}
