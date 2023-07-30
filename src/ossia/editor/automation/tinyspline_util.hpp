#pragma once
// clang-format off
#include <ossia/detail/config.hpp>
#define TINYSPLINE_API OSSIA_EXPORT

#include "tinyspline.h"
// clang-format on

#include <array>
#include <cstdint>

extern "C" {
TINYSPLINE_API
void ts_int_deboornet_init(tsDeBoorNet* _deBoorNet_);

TINYSPLINE_API
tsError
ts_int_deboornet_new(const tsBSpline* spline, tsDeBoorNet* net, tsStatus* status);

TINYSPLINE_API
void ts_int_bspline_init(tsBSpline* spline);

TINYSPLINE_API
tsError ts_int_bspline_eval_woa(
    const tsBSpline* spline, tsReal u, tsDeBoorNet* net, tsStatus* status);

TINYSPLINE_API
tsReal* ts_int_deboornet_access_result(const tsDeBoorNet* net);
}

namespace ts
{
template <std::size_t N>
struct spline
{
  tsBSpline m_spline;
  mutable tsDeBoorNet m_net;

  spline()
  {
    ts_int_bspline_init(&m_spline);
    ts_int_deboornet_init(&m_net);
  }

  ~spline()
  {
    if(m_net.pImpl)
      ts_deboornet_free(&m_net);
    if(m_spline.pImpl)
      ts_bspline_free(&m_spline);
  }

  operator bool() const noexcept { return m_net.pImpl; }

  void set_points(const tsReal* points, std::size_t numPoints) noexcept
  {
    tsStatus status;
    if(m_net.pImpl)
      ts_deboornet_free(&m_net);

    if(m_spline.pImpl)
      ts_bspline_free(&m_spline);

    ts_int_bspline_init(&m_spline);
    ts_bspline_new(numPoints, N, 3, TS_CLAMPED, &m_spline, &status);

    ts_bspline_set_control_points(&m_spline, points, &status);

    ts_int_deboornet_init(&m_net);
    ts_int_deboornet_new(&m_spline, &m_net, &status);
  }

  std::array<tsReal, N> evaluate(double pos) const noexcept
  {
    std::array<tsReal, N> res = {};

    tsStatus status;
    if(ts_int_bspline_eval_woa(&m_spline, pos, &m_net, &status) != 0)
    {
      // error during evaluation
      return res;
    }

    {
      const auto bytes = std::min(
          (std::size_t)N * sizeof(tsReal), (std::size_t)ts_deboornet_sof_result(&m_net));
      memcpy(res.data(), ts_int_deboornet_access_result(&m_net), bytes);
    }

    return res;
  }
};
}
