#pragma once

#include <ossia/detail/config.hpp>

#include <ossia/detail/dylib_loader.hpp>

#include <alsa/asoundlib.h>

#include <exception>
#include <vector>
namespace ossia
{
#define snd_alloca(ptr, lib, type)                       \
  do                                                     \
  {                                                      \
    *ptr = (snd_##type##_t*)alloca(lib.type##_sizeof()); \
    memset(*ptr, 0, lib.type##_sizeof());                \
  } while(0)
class libasound
{
public:
  // clang-format off
  decltype(&::snd_pcm_open) pcm_open{};
  decltype(&::snd_pcm_avail_update) pcm_avail_update{};
  decltype(&::snd_pcm_drop) pcm_drop{};
  decltype(&::snd_pcm_link) pcm_link{};
  decltype(&::snd_pcm_poll_descriptors) pcm_poll_descriptors{};
  decltype(&::snd_pcm_poll_descriptors_revents) pcm_poll_descriptors_revents{};
  decltype(&::snd_pcm_poll_descriptors_count) pcm_poll_descriptors_count{};
  decltype(&::snd_pcm_mmap_begin) pcm_mmap_begin{};
  decltype(&::snd_pcm_mmap_commit) pcm_mmap_commit{};
  decltype(&::snd_pcm_format_name) pcm_format_name{};
  decltype(&::snd_pcm_status) pcm_status{};
  decltype(&::snd_pcm_status_sizeof) pcm_status_sizeof{};
  decltype(&::snd_pcm_status_get_state) pcm_status_get_state{};
  decltype(&::snd_pcm_status_get_tstamp) pcm_status_get_tstamp{};
  decltype(&::snd_pcm_status_get_trigger_tstamp) pcm_status_get_trigger_tstamp{};
  decltype(&::snd_pcm_hw_params_malloc) pcm_hw_params_malloc{};
  decltype(&::snd_pcm_hw_params_get_access) pcm_hw_params_get_access{};
  decltype(&::snd_pcm_hw_params_set_periods_min) pcm_hw_params_set_periods_min{};
  decltype(&::snd_pcm_hw_params_set_rate) pcm_hw_params_set_rate{};
  decltype(&::snd_pcm_hw_params_set_period_size_near) pcm_hw_params_set_period_size_near{};
  decltype(&::snd_ctl_open) ctl_open{};
  decltype(&::snd_ctl_close) ctl_close{};
  decltype(&::snd_ctl_card_info) ctl_card_info{};
  decltype(&::snd_ctl_card_info_sizeof) ctl_card_info_sizeof{};
  decltype(&::snd_pcm_hw_params_any) pcm_hw_params_any{};
  decltype(&::snd_pcm_hw_params_sizeof) pcm_hw_params_sizeof{};
  decltype(&::snd_pcm_hw_params_set_access) pcm_hw_params_set_access{};
  decltype(&::snd_pcm_hw_params_get_format) pcm_hw_params_get_format{};
  decltype(&::snd_pcm_hw_params_set_format) pcm_hw_params_set_format{};
  decltype(&::snd_pcm_hw_params_set_channels) pcm_hw_params_set_channels{};
  decltype(&::snd_pcm_hw_params_set_period_size) pcm_hw_params_set_period_size{};
  decltype(&::snd_pcm_hw_params_set_buffer_size) pcm_hw_params_set_buffer_size{};
  decltype(&::snd_pcm_hw_params_set_periods) pcm_hw_params_set_periods{};
  decltype(&::snd_pcm_hw_params_set_periods_near) pcm_hw_params_set_periods_near{};
  decltype(&::snd_pcm_hw_params_set_periods_integer) pcm_hw_params_set_periods_integer{};
  decltype(&::snd_pcm_hw_params_set_rate_near) pcm_hw_params_set_rate_near{};
  decltype(&::snd_pcm_hw_params_get_channels) pcm_hw_params_get_channels{};
  decltype(&::snd_pcm_hw_params_get_channels_min) pcm_hw_params_get_channels_min{};
  decltype(&::snd_pcm_hw_params_get_channels_max) pcm_hw_params_get_channels_max{};
  decltype(&::snd_pcm_hw_params_get_rate) pcm_hw_params_get_rate{};
  decltype(&::snd_pcm_hw_params_get_rate_min) pcm_hw_params_get_rate_min{};
  decltype(&::snd_pcm_hw_params_get_rate_max) pcm_hw_params_get_rate_max{};
  decltype(&::snd_pcm_hw_params_get_buffer_size) pcm_hw_params_get_buffer_size{};
  decltype(&::snd_pcm_hw_params_get_buffer_size_min) pcm_hw_params_get_buffer_size_min{};
  decltype(&::snd_pcm_hw_params_get_buffer_size_max) pcm_hw_params_get_buffer_size_max{};
  decltype(&::snd_pcm_hw_params_get_period_size) pcm_hw_params_get_period_size{};
  decltype(&::snd_pcm_hw_params_get_period_size_min) pcm_hw_params_get_period_size_min{};
  decltype(&::snd_pcm_hw_params_get_period_size_max) pcm_hw_params_get_period_size_max{};
  decltype(&::snd_pcm_hw_params_get_periods) pcm_hw_params_get_periods{};
  decltype(&::snd_pcm_hw_params) pcm_hw_params{};
  decltype(&::snd_pcm_hw_params_free) pcm_hw_params_free{};
  decltype(&::snd_pcm_sw_params_malloc) pcm_sw_params_malloc{};
  decltype(&::snd_pcm_sw_params_current) pcm_sw_params_current{};
  decltype(&::snd_pcm_sw_params_sizeof) pcm_sw_params_sizeof{};
  decltype(&::snd_pcm_sw_params_set_start_threshold) pcm_sw_params_set_start_threshold{};
  decltype(&::snd_pcm_sw_params_set_avail_min) pcm_sw_params_set_avail_min{};
  decltype(&::snd_pcm_sw_params_set_tstamp_mode) pcm_sw_params_set_tstamp_mode{};
  decltype(&::snd_pcm_sw_params) pcm_sw_params{};
  decltype(&::snd_pcm_sw_params_free) pcm_sw_params_free{};
  decltype(&::snd_pcm_name) pcm_name{};
  decltype(&::snd_pcm_state) pcm_state{};
  decltype(&::snd_pcm_state_name) pcm_state_name{};
  decltype(&::snd_pcm_writei) pcm_writei{};
  decltype(&::snd_pcm_writen) pcm_writen{};
  decltype(&::snd_pcm_prepare) pcm_prepare{};
  decltype(&::snd_pcm_start) pcm_start{};
  decltype(&::snd_pcm_recover) pcm_recover{};
  decltype(&::snd_pcm_drain) pcm_drain{};
  decltype(&::snd_pcm_close) pcm_close{};
  decltype(&::snd_pcm_wait) pcm_wait{};
  decltype(&::snd_strerror) strerror{};
  decltype(&::snd_device_name_hint) device_name_hint{};
  decltype(&::snd_device_name_get_hint) device_name_get_hint{};
  decltype(&::snd_device_name_free_hint) device_name_free_hint{};

  // clang-format on
  static const libasound& instance()
  {
    static const libasound self;
    return self;
  }

private:
  dylib_loader library;

  libasound()
      : library("libasound.so.2")
  {
    // in terms of regex:
    // decltype\(&::([a-z_]+)\) ([a-z_]+){};
    // \2 = library.symbol<decltype(&::\1)>("\1");
    pcm_open = library.symbol<decltype(&::snd_pcm_open)>("snd_pcm_open");
    pcm_avail_update
        = library.symbol<decltype(&::snd_pcm_avail_update)>("snd_pcm_avail_update");
    pcm_drop = library.symbol<decltype(&::snd_pcm_drop)>("snd_pcm_drop");
    pcm_link = library.symbol<decltype(&::snd_pcm_link)>("snd_pcm_link");
    pcm_poll_descriptors = library.symbol<decltype(&::snd_pcm_poll_descriptors)>(
        "snd_pcm_poll_descriptors");
    pcm_poll_descriptors_revents
        = library.symbol<decltype(&::snd_pcm_poll_descriptors_revents)>(
            "snd_pcm_poll_descriptors_revents");
    pcm_poll_descriptors_count
        = library.symbol<decltype(&::snd_pcm_poll_descriptors_count)>(
            "snd_pcm_poll_descriptors_count");
    pcm_mmap_begin
        = library.symbol<decltype(&::snd_pcm_mmap_begin)>("snd_pcm_mmap_begin");
    pcm_mmap_commit
        = library.symbol<decltype(&::snd_pcm_mmap_commit)>("snd_pcm_mmap_commit");
    pcm_format_name
        = library.symbol<decltype(&::snd_pcm_format_name)>("snd_pcm_format_name");
    pcm_status = library.symbol<decltype(&::snd_pcm_status)>("snd_pcm_status");
    pcm_status_sizeof
        = library.symbol<decltype(&::snd_pcm_status_sizeof)>("snd_pcm_status_sizeof");
    pcm_status_get_state = library.symbol<decltype(&::snd_pcm_status_get_state)>(
        "snd_pcm_status_get_state");
    pcm_status_get_tstamp = library.symbol<decltype(&::snd_pcm_status_get_tstamp)>(
        "snd_pcm_status_get_tstamp");
    pcm_status_get_trigger_tstamp
        = library.symbol<decltype(&::snd_pcm_status_get_trigger_tstamp)>(
            "snd_pcm_status_get_trigger_tstamp");
    pcm_hw_params_malloc = library.symbol<decltype(&::snd_pcm_hw_params_malloc)>(
        "snd_pcm_hw_params_malloc");
    pcm_hw_params_get_access = library.symbol<decltype(&::snd_pcm_hw_params_get_access)>(
        "snd_pcm_hw_params_get_access");
    pcm_hw_params_set_periods_min
        = library.symbol<decltype(&::snd_pcm_hw_params_set_periods_min)>(
            "snd_pcm_hw_params_set_periods_min");
    pcm_hw_params_set_rate = library.symbol<decltype(&::snd_pcm_hw_params_set_rate)>(
        "snd_pcm_hw_params_set_rate");
    pcm_hw_params_set_period_size_near
        = library.symbol<decltype(&::snd_pcm_hw_params_set_period_size_near)>(
            "snd_pcm_hw_params_set_period_size_near");
    ctl_open = library.symbol<decltype(&::snd_ctl_open)>("snd_ctl_open");
    ctl_close = library.symbol<decltype(&::snd_ctl_close)>("snd_ctl_close");
    ctl_card_info = library.symbol<decltype(&::snd_ctl_card_info)>("snd_ctl_card_info");
    ctl_card_info_sizeof = library.symbol<decltype(&::snd_ctl_card_info_sizeof)>(
        "snd_ctl_card_info_sizeof");
    pcm_hw_params_any
        = library.symbol<decltype(&::snd_pcm_hw_params_any)>("snd_pcm_hw_params_any");
    pcm_hw_params_sizeof = library.symbol<decltype(&::snd_pcm_hw_params_sizeof)>(
        "snd_pcm_hw_params_sizeof");
    pcm_hw_params_set_access = library.symbol<decltype(&::snd_pcm_hw_params_set_access)>(
        "snd_pcm_hw_params_set_access");
    pcm_hw_params_get_format = library.symbol<decltype(&::snd_pcm_hw_params_get_format)>(
        "snd_pcm_hw_params_get_format");
    pcm_hw_params_set_format = library.symbol<decltype(&::snd_pcm_hw_params_set_format)>(
        "snd_pcm_hw_params_set_format");
    pcm_hw_params_set_channels
        = library.symbol<decltype(&::snd_pcm_hw_params_set_channels)>(
            "snd_pcm_hw_params_set_channels");
    pcm_hw_params_set_period_size
        = library.symbol<decltype(&::snd_pcm_hw_params_set_period_size)>(
            "snd_pcm_hw_params_set_period_size");
    pcm_hw_params_set_buffer_size
        = library.symbol<decltype(&::snd_pcm_hw_params_set_buffer_size)>(
            "snd_pcm_hw_params_set_buffer_size");
    pcm_hw_params_set_periods
        = library.symbol<decltype(&::snd_pcm_hw_params_set_periods)>(
            "snd_pcm_hw_params_set_periods");
    pcm_hw_params_set_periods_near
        = library.symbol<decltype(&::snd_pcm_hw_params_set_periods_near)>(
            "snd_pcm_hw_params_set_periods_near");
    pcm_hw_params_set_periods_integer
        = library.symbol<decltype(&::snd_pcm_hw_params_set_periods_integer)>(
            "snd_pcm_hw_params_set_periods_integer");
    pcm_hw_params_set_rate_near
        = library.symbol<decltype(&::snd_pcm_hw_params_set_rate_near)>(
            "snd_pcm_hw_params_set_rate_near");
    pcm_hw_params_get_channels
        = library.symbol<decltype(&::snd_pcm_hw_params_get_channels)>(
            "snd_pcm_hw_params_get_channels");
    pcm_hw_params_get_channels_min
        = library.symbol<decltype(&::snd_pcm_hw_params_get_channels_min)>(
            "snd_pcm_hw_params_get_channels_min");
    pcm_hw_params_get_channels_max
        = library.symbol<decltype(&::snd_pcm_hw_params_get_channels_max)>(
            "snd_pcm_hw_params_get_channels_max");
    pcm_hw_params_get_rate = library.symbol<decltype(&::snd_pcm_hw_params_get_rate)>(
        "snd_pcm_hw_params_get_rate");
    pcm_hw_params_get_rate_min
        = library.symbol<decltype(&::snd_pcm_hw_params_get_rate_min)>(
            "snd_pcm_hw_params_get_rate_min");
    pcm_hw_params_get_rate_max
        = library.symbol<decltype(&::snd_pcm_hw_params_get_rate_max)>(
            "snd_pcm_hw_params_get_rate_max");
    pcm_hw_params_get_buffer_size
        = library.symbol<decltype(&::snd_pcm_hw_params_get_buffer_size)>(
            "snd_pcm_hw_params_get_buffer_size");
    pcm_hw_params_get_buffer_size_min
        = library.symbol<decltype(&::snd_pcm_hw_params_get_buffer_size_min)>(
            "snd_pcm_hw_params_get_buffer_size_min");
    pcm_hw_params_get_buffer_size_max
        = library.symbol<decltype(&::snd_pcm_hw_params_get_buffer_size_max)>(
            "snd_pcm_hw_params_get_buffer_size_max");
    pcm_hw_params_get_period_size
        = library.symbol<decltype(&::snd_pcm_hw_params_get_period_size)>(
            "snd_pcm_hw_params_get_period_size");
    pcm_hw_params_get_period_size_min
        = library.symbol<decltype(&::snd_pcm_hw_params_get_period_size_min)>(
            "snd_pcm_hw_params_get_period_size_min");
    pcm_hw_params_get_period_size_max
        = library.symbol<decltype(&::snd_pcm_hw_params_get_period_size_max)>(
            "snd_pcm_hw_params_get_period_size_max");
    pcm_hw_params_get_periods
        = library.symbol<decltype(&::snd_pcm_hw_params_get_periods)>(
            "snd_pcm_hw_params_get_periods");
    pcm_hw_params = library.symbol<decltype(&::snd_pcm_hw_params)>("snd_pcm_hw_params");
    pcm_hw_params_free
        = library.symbol<decltype(&::snd_pcm_hw_params_free)>("snd_pcm_hw_params_free");
    pcm_sw_params_malloc = library.symbol<decltype(&::snd_pcm_sw_params_malloc)>(
        "snd_pcm_sw_params_malloc");
    pcm_sw_params_current = library.symbol<decltype(&::snd_pcm_sw_params_current)>(
        "snd_pcm_sw_params_current");
    pcm_sw_params_sizeof = library.symbol<decltype(&::snd_pcm_sw_params_sizeof)>(
        "snd_pcm_sw_params_sizeof");
    pcm_sw_params_set_start_threshold
        = library.symbol<decltype(&::snd_pcm_sw_params_set_start_threshold)>(
            "snd_pcm_sw_params_set_start_threshold");
    pcm_sw_params_set_avail_min
        = library.symbol<decltype(&::snd_pcm_sw_params_set_avail_min)>(
            "snd_pcm_sw_params_set_avail_min");
    pcm_sw_params_set_tstamp_mode
        = library.symbol<decltype(&::snd_pcm_sw_params_set_tstamp_mode)>(
            "snd_pcm_sw_params_set_tstamp_mode");
    pcm_sw_params = library.symbol<decltype(&::snd_pcm_sw_params)>("snd_pcm_sw_params");
    pcm_sw_params_free
        = library.symbol<decltype(&::snd_pcm_sw_params_free)>("snd_pcm_sw_params_free");
    pcm_name = library.symbol<decltype(&::snd_pcm_name)>("snd_pcm_name");
    pcm_state = library.symbol<decltype(&::snd_pcm_state)>("snd_pcm_state");
    pcm_state_name
        = library.symbol<decltype(&::snd_pcm_state_name)>("snd_pcm_state_name");
    pcm_writei = library.symbol<decltype(&::snd_pcm_writei)>("snd_pcm_writei");
    pcm_writen = library.symbol<decltype(&::snd_pcm_writen)>("snd_pcm_writen");
    pcm_prepare = library.symbol<decltype(&::snd_pcm_prepare)>("snd_pcm_prepare");
    pcm_start = library.symbol<decltype(&::snd_pcm_start)>("snd_pcm_start");
    pcm_recover = library.symbol<decltype(&::snd_pcm_recover)>("snd_pcm_recover");
    pcm_drain = library.symbol<decltype(&::snd_pcm_drain)>("snd_pcm_drain");
    pcm_close = library.symbol<decltype(&::snd_pcm_close)>("snd_pcm_close");
    pcm_wait = library.symbol<decltype(&::snd_pcm_wait)>("snd_pcm_wait");
    strerror = library.symbol<decltype(&::snd_strerror)>("snd_strerror");
    device_name_hint
        = library.symbol<decltype(&::snd_device_name_hint)>("snd_device_name_hint");
    device_name_get_hint = library.symbol<decltype(&::snd_device_name_get_hint)>(
        "snd_device_name_get_hint");
    device_name_free_hint = library.symbol<decltype(&::snd_device_name_free_hint)>(
        "snd_device_name_free_hint");

    // in terms of regex:
    // decltype\(&::([a-z_]+)\) [a-z_]+{};
    // assert(\1);
    assert(snd_pcm_open);
    assert(snd_pcm_avail_update);
    assert(snd_pcm_drop);
    assert(snd_pcm_link);
    assert(snd_pcm_poll_descriptors);
    assert(snd_pcm_poll_descriptors_revents);
    assert(snd_pcm_poll_descriptors_count);
    assert(snd_pcm_mmap_begin);
    assert(snd_pcm_mmap_commit);
    assert(snd_pcm_format_name);
    assert(snd_pcm_status);
    assert(snd_pcm_status_sizeof);
    assert(snd_pcm_status_get_state);
    assert(snd_pcm_status_get_tstamp);
    assert(snd_pcm_status_get_trigger_tstamp);
    assert(snd_pcm_hw_params_malloc);
    assert(snd_pcm_hw_params_get_access);
    assert(snd_pcm_hw_params_set_periods_min);
    assert(snd_pcm_hw_params_set_rate);
    assert(snd_pcm_hw_params_set_period_size_near);
    assert(snd_ctl_open);
    assert(snd_ctl_close);
    assert(snd_ctl_card_info);
    assert(snd_ctl_card_info_sizeof);
    assert(snd_pcm_hw_params_any);
    assert(snd_pcm_hw_params_sizeof);
    assert(snd_pcm_hw_params_set_access);
    assert(snd_pcm_hw_params_get_format);
    assert(snd_pcm_hw_params_set_format);
    assert(snd_pcm_hw_params_set_channels);
    assert(snd_pcm_hw_params_set_period_size);
    assert(snd_pcm_hw_params_set_buffer_size);
    assert(snd_pcm_hw_params_set_periods);
    assert(snd_pcm_hw_params_set_periods_near);
    assert(snd_pcm_hw_params_set_periods_integer);
    assert(snd_pcm_hw_params_set_rate_near);
    assert(snd_pcm_hw_params_get_channels);
    assert(snd_pcm_hw_params_get_channels_min);
    assert(snd_pcm_hw_params_get_channels_max);
    assert(snd_pcm_hw_params_get_rate);
    assert(snd_pcm_hw_params_get_rate_min);
    assert(snd_pcm_hw_params_get_rate_max);
    assert(snd_pcm_hw_params_get_buffer_size);
    assert(snd_pcm_hw_params_get_buffer_size_min);
    assert(snd_pcm_hw_params_get_buffer_size_max);
    assert(snd_pcm_hw_params_get_period_size);
    assert(snd_pcm_hw_params_get_period_size_min);
    assert(snd_pcm_hw_params_get_period_size_max);
    assert(snd_pcm_hw_params_get_periods);
    assert(snd_pcm_hw_params);
    assert(snd_pcm_hw_params_free);
    assert(snd_pcm_sw_params_malloc);
    assert(snd_pcm_sw_params_current);
    assert(snd_pcm_sw_params_sizeof);
    assert(snd_pcm_sw_params_set_start_threshold);
    assert(snd_pcm_sw_params_set_avail_min);
    assert(snd_pcm_sw_params_set_tstamp_mode);
    assert(snd_pcm_sw_params);
    assert(snd_pcm_sw_params_free);
    assert(snd_pcm_name);
    assert(snd_pcm_state);
    assert(snd_pcm_state_name);
    assert(snd_pcm_writei);
    assert(snd_pcm_writen);
    assert(snd_pcm_prepare);
    assert(snd_pcm_start);
    assert(snd_pcm_recover);
    assert(snd_pcm_drain);
    assert(snd_pcm_close);
    assert(snd_pcm_wait);
    assert(snd_strerror);
    assert(snd_device_name_hint);
    assert(snd_device_name_get_hint);
    assert(snd_device_name_free_hint);
  }
};
}
