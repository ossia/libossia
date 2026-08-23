#pragma once
#include <ossia/detail/config.hpp>

#include <cmath>

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

/**
 * \file tracking.hpp
 *
 * Building blocks for multi-object trackers: a dt-correct scalar
 * position-velocity Kalman filter, its N-dimensional composition, and the
 * greedy bipartite assignment step shared by the trackers in the tree.
 *
 * A constant-velocity tracker with independent axes, a diagonal measurement
 * noise and a (block-)diagonal process noise decouples exactly into one
 * two-state filter per axis: the full 2Nx2N filter (ByteTrack's 8-dim xyah
 * included) is mathematically identical to N of these, so no matrix library is
 * needed and everything below is closed-form scalar algebra.
 *
 * Everything takes an explicit dt in seconds: these run off cameras, OSC and
 * TUIO sources whose frame spacing is neither fixed nor known in advance.
 */

namespace ossia
{

/**
 * @brief Scalar constant-velocity Kalman filter, one axis of a tracked point.
 *
 * State is [position, velocity]; the measurement is the position alone.
 * Velocity is in units per second.
 */
struct kalman_pv_filter
{
  //! Discrete process noise covariance for one predict step.
  struct process_noise
  {
    float q00, q01, q11;
  };

  /**
   * @brief Discretised continuous-white-noise-acceleration process noise.
   *
   * Q = q * [[dt^3/3, dt^2/2], [dt^2/2, dt]], with q = sigma_a^2.
   *
   * @param sigma_a Acceleration magnitude of the tracked motion. Human motion
   *        peaks around 2 m/s^2; fitting that as 3 sigma gives ~0.67 as a
   *        default in metric spaces. (Strictly q is a power spectral density in
   *        units^2/s^3; treating sigma_a^2 as its value is the usual shortcut.)
   * @param dt Time step in seconds.
   */
  [[nodiscard]] static OSSIA_INLINE process_noise cwna(float sigma_a, float dt) noexcept
  {
    const float q = sigma_a * sigma_a;
    const float dt2 = dt * dt;
    return {q * dt2 * dt / 3.f, q * dt2 / 2.f, q * dt};
  }

  /**
   * @brief Ad-hoc diagonal process noise, dt-scaled.
   *
   * For filters tuned with per-frame standard deviations (ByteTrack's
   * std_weight_position / std_weight_velocity): pass the per-second equivalents
   * and the reference-relative time ratio; reproduces the historical diagonal
   * exactly at the reference rate while still growing with elapsed time.
   *
   * @param std_p Position process std for one reference-length step.
   * @param std_v Velocity process std for one reference-length step.
   * @param time_ratio dt / reference_dt.
   */
  [[nodiscard]] static OSSIA_INLINE process_noise
  diagonal(float std_p, float std_v, float time_ratio) noexcept
  {
    return {std_p * std_p * time_ratio, 0.f, std_v * std_v * time_ratio};
  }

  //! State: position and velocity (units, units/s).
  float p{}, v{};

  //! Covariance, symmetric 2x2: [[P00, P01], [P01, P11]].
  float P00{}, P01{}, P11{};

  //! Start tracking at @p p0 with the given initial variances.
  OSSIA_INLINE void initiate(float p0, float var_p, float var_v) noexcept
  {
    p = p0;
    v = 0.f;
    P00 = var_p;
    P01 = 0.f;
    P11 = var_v;
  }

  //! Propagate the state @p dt seconds forward.
  OSSIA_INLINE void predict(float dt, const process_noise& q) noexcept
  {
    p += v * dt;
    P00 += dt * (2.f * P01 + dt * P11) + q.q00;
    P01 += dt * P11 + q.q01;
    P11 += q.q11;
  }

  /**
   * @brief Fold in a position measurement.
   * @param z Measured position.
   * @param r Measurement variance (std^2).
   */
  OSSIA_INLINE void update(float z, float r) noexcept
  {
    const float S = P00 + r;
    if(!(S > 0.f)) [[unlikely]]
      return; // degenerate (or NaN) innovation covariance: skip the update
    const float K0 = P00 / S;
    const float K1 = P01 / S;
    const float y = z - p;
    p += K0 * y;
    v += K1 * y;
    P01 -= K0 * P01;
    P11 -= K1 * P01; // uses the already-updated P01 = (1-K0)P01: exact (I-KH)P
    P00 -= K0 * P00;
  }

  //! Squared Mahalanobis distance of measurement @p z, in measurement space.
  //! Chi-square distributed with 1 DOF; sum over axes for N DOF.
  [[nodiscard]] OSSIA_INLINE float gating_distance2(float z, float r) const noexcept
  {
    const float S = P00 + r;
    const float y = z - p;
    return S > 0.f ? (y * y) / S : 0.f;
  }
};

/**
 * @brief N independent position-velocity Kalman filters: a tracked N-D point.
 *
 * [pos, vel] state per axis (so 4 states in 2D, 6 in 3D), isotropic noise.
 */
template <std::size_t N>
struct kalman_point_filter
{
  std::array<kalman_pv_filter, N> axes{};

  OSSIA_INLINE void
  initiate(const std::array<float, N>& p0, float var_p, float var_v) noexcept
  {
    for(std::size_t i = 0; i < N; i++)
      axes[i].initiate(p0[i], var_p, var_v);
  }

  //! Propagate @p dt seconds forward with acceleration noise @p sigma_a.
  OSSIA_INLINE void predict(float dt, float sigma_a) noexcept
  {
    const auto q = kalman_pv_filter::cwna(sigma_a, dt);
    for(auto& a : axes)
      a.predict(dt, q);
  }

  //! Fold in a position measurement with isotropic variance @p r.
  OSSIA_INLINE void update(const std::array<float, N>& z, float r) noexcept
  {
    for(std::size_t i = 0; i < N; i++)
      axes[i].update(z[i], r);
  }

  //! Squared Mahalanobis distance, chi-square with N DOF.
  [[nodiscard]] OSSIA_INLINE float
  gating_distance2(const std::array<float, N>& z, float r) const noexcept
  {
    float d = 0.f;
    for(std::size_t i = 0; i < N; i++)
      d += axes[i].gating_distance2(z[i], r);
    return d;
  }

  [[nodiscard]] OSSIA_INLINE std::array<float, N> position() const noexcept
  {
    std::array<float, N> r;
    for(std::size_t i = 0; i < N; i++)
      r[i] = axes[i].p;
    return r;
  }

  [[nodiscard]] OSSIA_INLINE std::array<float, N> velocity() const noexcept
  {
    std::array<float, N> r;
    for(std::size_t i = 0; i < N; i++)
      r[i] = axes[i].v;
    return r;
  }

  OSSIA_INLINE void set_velocity(const std::array<float, N>& v) noexcept
  {
    for(std::size_t i = 0; i < N; i++)
      axes[i].v = v[i];
  }
};

/**
 * @brief One candidate pairing for greedy bipartite assignment.
 */
struct match_candidate
{
  float cost;
  std::int32_t track, det;
};

/**
 * @brief Greedy bipartite matching: repeatedly take the cheapest remaining
 * candidate whose track and detection are both still free.
 *
 * The candidate list is sorted in place (stable, so ties keep insertion order
 * and results are deterministic). Not optimal like Hungarian, but within a
 * fraction of it for tracking workloads (ByteTrack itself ships greedy variants)
 * and O(K log K) with no allocation beyond the caller's buffers.
 *
 * @param candidates Gated candidate pairs; entries with an already-matched
 *        track/detection are skipped.
 * @param track_match Per-track matched detection index, -1 if unmatched.
 *        Pre-filled entries (>= 0) are treated as taken.
 * @param n_tracks Size of @p track_match.
 * @param det_used Per-detection taken flag.
 * @param n_dets Size of @p det_used.
 */
inline void greedy_assignment(
    std::vector<match_candidate>& candidates, std::int32_t* track_match,
    std::size_t n_tracks, char* det_used, std::size_t n_dets) noexcept
{
  std::stable_sort(
      candidates.begin(), candidates.end(),
      [](const match_candidate& a, const match_candidate& b) {
    return a.cost < b.cost;
  });
  for(const auto& c : candidates)
  {
    if(c.track < 0 || std::size_t(c.track) >= n_tracks)
      continue;
    if(c.det < 0 || std::size_t(c.det) >= n_dets)
      continue;
    if(track_match[c.track] >= 0 || det_used[c.det])
      continue;
    track_match[c.track] = c.det;
    det_used[c.det] = 1;
  }
}

}
