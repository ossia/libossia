#pragma once

#include "tinyspline.h"
#include <cstdint>
#include <array>

namespace ts
{
template<std::size_t N>
struct spline
{
  mutable tsBSpline m_spline;
  mutable tsDeBoorNet m_net;

  spline()
    : m_spline{ts_bspline_init()}
  {
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

    m_spline = ts_bspline_init();
    ts_bspline_new(numPoints, N, 3, TS_CLAMPED, &m_spline, &status);

    ts_bspline_set_control_points(
        &m_spline,
        points,
        &status);

    ts_int_deboornet_init(&m_net);
    ts_int_deboornet_new(&m_spline, &m_net, &status);
  }

  std::array<tsReal, N> evaluate(double pos) const noexcept
  {
    std::array<tsReal, N> res;

    tsStatus status;
    if (ts_int_bspline_eval_woa(&m_spline, pos, &m_net, &status) != 0)
    {
      // error during evaluation
      return res;
    }

    {
      const auto bytes = std::min((std::size_t)N * sizeof(tsReal), (std::size_t)ts_deboornet_sof_result(&m_net));
      memcpy(res.data(), ts_int_deboornet_access_result(&m_net), bytes);
    }

    return res;
  }
};
}
