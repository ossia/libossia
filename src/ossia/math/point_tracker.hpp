#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/small_vector.hpp>
#include <ossia/math/filters.hpp>
#include <ossia/math/tracking.hpp>

#include <cmath>

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <vector>

/**
 * \file point_tracker.hpp
 *
 * Generic multi-object point tracker: turns per-frame sets of bare detections
 * (positions with an optional confidence) into stable identified tracks over
 * time. This is the association + filtering + lifecycle layer between a
 * detector (blobs, keypoints, TUIO/OSC points...) and whatever maps object
 * identity to sound or visuals.
 *
 * Ingredients, and where they come from:
 *  - per-track constant-velocity Kalman over real elapsed time (tracking.hpp),
 *  - two-stage association by detection confidence (ByteTrack: low-confidence
 *    detections may sustain an existing track but never start one),
 *  - OC-SORT touches: velocity-direction consistency in the cost, velocity
 *    re-seeding after a gap, and observation-centric re-update along a virtual
 *    trajectory when a lost track is revived (kills the post-occlusion lurch),
 *  - M-of-N + minimum-time confirmation, so confirmation latency is stable in
 *    milliseconds across frame rates instead of being a frame count,
 *  - two-tier lifecycle: provisional tracks exist (and can be emitted, flagged)
 *    from the very first detection - waiting for confirmation costs ~100 ms of
 *    onset latency, an order of magnitude over the ~10 ms budget of musical
 *    control - while confirmed tracks are the stable set for continuous
 *    mappings,
 *  - persistent ids (monotonic, never reused) plus dense reusable slots with a
 *    quarantine hold, for mapping to a fixed bank of voices/parameters,
 *  - per-track One-Euro output smoothing (filters.hpp), owned per identity so
 *    two crossing objects never bleed into each other's history.
 *
 * All time is real seconds passed in by the caller; nothing assumes a fixed
 * frame rate. Coordinates are whatever space the caller uses consistently;
 * every threshold with a unit (max_speed, meas_std...) is in that same space.
 *
 * The hot path does not allocate once the scratch buffers are warm, except when
 * a new track is born.
 */

namespace ossia
{

enum class track_state : std::uint8_t
{
  provisional, //!< Seen, but not yet confirmed: usable for triggers, flagged.
  confirmed,   //!< Passed M-of-N + time confirmation: the stable set.
  coasting,    //!< Missed recently; position is the Kalman prediction.
  revived,     //!< Re-acquired after being lost (transient, one frame).
  lost,        //!< Past the coast window; kept for revival, not emitted.
  expired      //!< Terminal; the track is removed right after this is set.
};

enum class track_motion_gate : std::uint8_t
{
  off,        //!< No motion gate: nearest-neighbour within the cost ranking.
  max_speed,  //!< Analytic: displacement <= max_speed * dt * (1 + lost/coast).
  mahalanobis //!< Kalman gating distance against a chi-square threshold.
};

enum class track_slot_allocation : std::uint8_t
{
  lowest_free,    //!< Lowest free slot index (cv.jit-style, deterministic).
  round_robin,    //!< Cycle through the slots, spreading reuse over time.
  nearest_vacated //!< Prefer a recently-vacated slot near the track's position.
};

enum class track_slot_steal : std::uint8_t
{
  never,            //!< A confirmed track without a free slot stays unslotted.
  stalest,          //!< Steal from the track unseen for the longest time.
  lowest_confidence //!< Steal from the track with the lowest confidence.
};

template <std::size_t N>
struct point_tracker_config
{
  // --- Association ---------------------------------------------------------
  track_motion_gate gate = track_motion_gate::max_speed;
  //! Maximum plausible object speed, in coordinate units per second. Also sets
  //! the scale on which association costs are normalized.
  float max_speed = 2.f;
  //! Chi-square 99.9% for N degrees of freedom: 13.8155 (2), 16.2662 (3).
  //! Not the DeepSORT-conventional 95% (5.9915 / 7.8147): the gate's
  //! false-rejection rate is exactly the chi-square tail, every rejected true
  //! detection births a competing duplicate track (birth needs only
  //! new_conf), and a 5%-per-frame rejection rate measured ~200 id switches
  //! in 20 s at 60 fps on a single isolated object where 0.1% measures 0.
  //! The tail rate is per FRAME, so a tight quantile also degrades faster at
  //! higher frame rates.
  float mahalanobis_thresh = N == 3 ? 16.2662f : 13.8155f;
  //! ByteTrack: after the high-confidence pass, let low-confidence detections
  //! sustain still-unmatched recent tracks (worth 1-10 IDF1 points).
  bool two_stage = true;
  float high_conf = 0.5f; //!< >= : first-stage detections.
  float low_conf = 0.1f;  //!< [low, high[ : second-stage recovery detections.
  float new_conf = 0.6f;  //!< Birth a track only at or above this.
  //! OC-SORT velocity-direction consistency weight in the association cost.
  //! The term sums the angle between observed track direction and the
  //! track->detection direction over temporal baselines of 1, 2 and 3
  //! detection frames (OC-SORT Table 7: gains stop at 3 and decline at 5).
  float dir_weight = 0.2f;
  //! Hybrid-SORT TCM (Tracklet Confidence Modeling, arXiv:2308.00783): weight
  //! of |predicted track confidence - detection confidence| in the
  //! association cost. Detection confidence is a strong identity cue exactly
  //! when position is at its weakest (occlusions depress it smoothly), worth
  //! +4 HOTA in their ablation. The prediction is Kalman-filtered in the
  //! high-confidence stage but two-point LINEAR in the low-confidence stage:
  //! confidence changes abruptly at occlusion onset/exit and the Kalman lags
  //! those transitions (their ablated Kalman/Linear split, Table IV - linear
  //! in the first stage is worse than nothing). 0 disables the term.
  float conf_weight = 1.0f;

  // --- Motion filter -------------------------------------------------------
  //! Process noise: acceleration magnitude of the tracked motion, units/s^2.
  //! Human motion peaks around 2 m/s^2; 2/3 fits that as 3 sigma.
  float accel_sigma = 0.67f;
  //! Measurement noise std of the detector, in coordinate units.
  float meas_std = 0.005f;

  // --- Confirmation --------------------------------------------------------
  //! Minimum age before confirmation, seconds (not frames: milliseconds mean
  //! the same thing at 30 and at 144 fps).
  float confirm_time = 0.100f;
  std::uint32_t confirm_hits = 3;   //!< M of...
  std::uint32_t confirm_window = 5; //!< ...the last N detection frames.
  //! A detection at or above this confidence confirms immediately.
  float instant_confirm = 0.9f;

  // --- Lifecycle -----------------------------------------------------------
  //! How long a missed track keeps being emitted at its predicted position.
  float coast_time = 0.5f;
  bool revive = true;      //!< Keep lost tracks around for re-acquisition.
  float revive_time = 2.f; //!< Revival window after the coast window, seconds.
  //! OC-SORT ORU: on revival, re-run the filter along a virtual trajectory
  //! spanning the gap instead of trusting the drifted prediction.
  bool revive_reupdate = true;

  // --- Output smoothing (One-Euro, per track per axis) ---------------------
  bool smooth = true;
  float min_cutoff = 1.f;   //!< Hz. Lower = smoother at rest.
  float beta = 1.f;         //!< Speed coefficient. MediaPipe ships 10-80.
  float deriv_cutoff = 1.f; //!< Hz, low-pass on the derivative estimate.

  // --- Slots ---------------------------------------------------------------
  std::uint32_t slot_count = 8;
  track_slot_allocation allocation = track_slot_allocation::lowest_free;
  track_slot_steal steal = track_slot_steal::never;
  //! Quarantine: a vacated slot is not reallocated for this long, so a brief
  //! exit/re-entry does not hand an object's slot to a stranger.
  float slot_hold_time = 0.25f;
};

template <std::size_t N>
struct point_detection
{
  std::array<float, N> position{};
  float confidence = 1.f;
};

template <std::size_t N>
struct point_track
{
  //! Persistent identity: monotonic, never reused within a tracker lifetime.
  std::int32_t id = -1;
  //! Dense reusable index in [0, slot_count[, -1 if unslotted.
  std::int32_t slot = -1;
  track_state state = track_state::provisional;
  //! Seconds (tracker clock) at birth - the true identity; the index in
  //! tracks() is only a transient handle.
  double creation_time = 0.;
  float age = 0.f;             //!< Seconds since birth.
  float time_since_seen = 0.f; //!< Seconds since the last matched detection.
  float confidence = 0.f;      //!< Confidence of the last matched detection.
  bool reacquired = false;     //!< Went through at least one lost->revived.

  //! Kalman state: position/velocity per axis, velocity in units/second.
  kalman_point_filter<N> kf{};
  //! Filter state as of the last measurement, for the revival re-update.
  kalman_point_filter<N> kf_at_meas{};
  //! One-Euro smoothed output position.
  std::array<float, N> filtered{};
  //! Last raw measurement.
  std::array<float, N> last_meas{};
  //! Ring of the most recent observed (matched) positions: obs_hist[0]
  //! duplicates last_meas, obs_hist[k] is k detection-frames earlier. Feeds
  //! the multi-dt direction-consistency term.
  std::array<std::array<float, N>, 4> obs_hist{};
  std::uint8_t obs_count = 0;
  //! Kalman state over the detection confidence (Hybrid-SORT TCM):
  //! p = confidence, v = confidence rate per second.
  kalman_pv_filter conf_kf{};
  //! Confidence of the match before last (c[t-2]); -1 while there is none.
  //! With `confidence` (c[t-1]) this is the two-point linear extrapolation
  //! base for the low-confidence association stage.
  float conf_prev = -1.f;

  std::array<one_euro_filter<float>, N> smoothers{};

  //! Ring of the last detection-frame outcomes, bit 0 = most recent frame.
  std::uint32_t hit_history = 0;
  std::uint32_t hits = 0; //!< Total matched detections.
  std::uint32_t consecutive_misses = 0;

  [[nodiscard]] std::array<float, N> position() const noexcept { return kf.position(); }
  [[nodiscard]] std::array<float, N> velocity() const noexcept { return kf.velocity(); }

  //! Should this track appear in the outputs?
  [[nodiscard]] bool emitted(bool include_provisional) const noexcept
  {
    switch(state)
    {
      case track_state::confirmed:
      case track_state::coasting:
      case track_state::revived:
        return true;
      case track_state::provisional:
        return include_provisional;
      default:
        return false;
    }
  }
};

/**
 * @brief Lifecycle notifications of one step. Ids, not indices: by the time an
 * exit is reported the track is no longer in tracks().
 */
struct track_events
{
  ossia::small_vector<std::int32_t, 8> entered;
  ossia::small_vector<std::int32_t, 8> confirmed;
  ossia::small_vector<std::int32_t, 8> exited;
  ossia::small_vector<std::int32_t, 8> revived;

  void clear() noexcept
  {
    entered.clear();
    confirmed.clear();
    exited.clear();
    revived.clear();
  }
};

template <std::size_t N>
class point_tracker
{
public:
  using config = point_tracker_config<N>;
  using detection = point_detection<N>;
  using track = point_track<N>;

  void configure(const config& c)
  {
    m_cfg = c;
    m_proto.min_cutoff = c.min_cutoff;
    m_proto.beta = c.beta;
    m_proto.d_cutoff = c.deriv_cutoff;
    for(auto& t : m_tracks)
    {
      for(auto& s : t.smoothers)
        s.assign_parameters(m_proto);
      // A slot beyond a shrunk slot count is gone; no quarantine for those.
      if(t.slot >= std::int32_t(c.slot_count))
        t.slot = -1;
    }
    m_vacated.erase(
        std::remove_if(
            m_vacated.begin(), m_vacated.end(),
            [&](const vacated& v) { return v.slot >= std::int32_t(c.slot_count); }),
        m_vacated.end());
  }

  const config& get_config() const noexcept { return m_cfg; }
  const std::vector<track>& tracks() const noexcept { return m_tracks; }
  const track_events& events() const noexcept { return m_events; }
  double now() const noexcept { return m_now; }
  //! Smoothed estimate of the spacing between detection frames, seconds.
  float estimated_period() const noexcept { return m_est_period; }

  //! Forget everything, including the id counter.
  void reset()
  {
    m_tracks.clear();
    m_vacated.clear();
    m_events.clear();
    m_next_id = 1;
    m_now = 0.;
    m_last_frame_time = 0.;
    m_est_period = 1.f / 30.f;
    m_rr_cursor = 0;
  }

  /**
   * @brief Let @p dt seconds pass without a detection frame.
   *
   * Predicts every track forward, applies the time-based lifecycle transitions
   * (coasting -> lost -> expired) and fires the corresponding events. Call this
   * when time passes but the source produced nothing, so tracks keep coasting
   * and eventually expire even if the detector goes silent.
   */
  void advance(float dt)
  {
    m_events.clear();
    step_time(dt);
    refresh_states_and_expire();
    smooth_outputs(dt);
  }

  /**
   * @brief Process one detection frame, @p dt seconds after the previous call.
   *
   * @return One entry per input detection: the id of the track it was
   * associated to (freshly born included), or -1 if it was dropped. Valid until
   * the next update()/advance().
   */
  const std::vector<std::int32_t>& update(const detection* dets, std::size_t n, float dt)
  {
    m_events.clear();
    step_time(dt);

    // Real spacing between detection frames (advance() may have run inbetween).
    const float frame_dt = std::clamp(float(m_now - m_last_frame_time), 1e-4f, 10.f);
    m_last_frame_time = m_now;
    m_est_period = 0.9f * m_est_period + 0.1f * std::min(frame_dt, 1.f);

    auto& out = m_assign;
    out.assign(n, -1);

    // Split by confidence (ByteTrack)
    m_high.clear();
    m_low.clear();
    for(std::size_t i = 0; i < n; i++)
    {
      const float c = dets[i].confidence;
      if(c >= m_cfg.high_conf)
        m_high.push_back(std::int32_t(i));
      else if(m_cfg.two_stage && c >= m_cfg.low_conf)
        m_low.push_back(std::int32_t(i));
    }

    m_t_match.assign(m_tracks.size(), -1);
    m_d_used.assign(n, 0);

    // Stage 1: all tracks (lost ones included when revival is on) vs
    // high-confidence detections.
    m_cands.clear();
    for(std::size_t ti = 0; ti < m_tracks.size(); ti++)
    {
      const auto& t = m_tracks[ti];
      if(t.state == track_state::lost && !m_cfg.revive)
        continue;
      for(const auto di : m_high)
      {
        float c;
        if(admissible(t, dets[di], frame_dt, true, c))
          m_cands.push_back({c, std::int32_t(ti), di});
      }
    }
    greedy_assignment(m_cands, m_t_match.data(), m_tracks.size(), m_d_used.data(), n);

    // Stage 2: still-unmatched recent tracks vs low-confidence detections.
    // Lost tracks are excluded: a low-confidence blip must not revive.
    if(!m_low.empty())
    {
      m_cands.clear();
      for(std::size_t ti = 0; ti < m_tracks.size(); ti++)
      {
        if(m_t_match[ti] >= 0)
          continue;
        const auto& t = m_tracks[ti];
        if(t.state == track_state::lost)
          continue;
        for(const auto di : m_low)
        {
          float c;
          if(admissible(t, dets[di], frame_dt, false, c))
            m_cands.push_back({c, std::int32_t(ti), di});
        }
      }
      greedy_assignment(m_cands, m_t_match.data(), m_tracks.size(), m_d_used.data(), n);
    }

    // Apply matches / misses.
    for(std::size_t ti = 0; ti < m_tracks.size(); ti++)
    {
      auto& t = m_tracks[ti];
      const auto di = m_t_match[ti];
      if(di >= 0)
      {
        apply_match(t, dets[di]);
        out[di] = t.id;
      }
      else
      {
        t.hit_history <<= 1;
        t.consecutive_misses++;
      }
    }

    // Births: unmatched detections above the birth threshold.
    for(std::size_t i = 0; i < n; i++)
    {
      if(m_d_used[i] || dets[i].confidence < m_cfg.new_conf)
        continue;
      out[i] = birth(dets[i]);
    }

    refresh_states_and_expire();
    smooth_outputs(dt);
    return out;
  }

  //! update() convenience over a vector.
  const std::vector<std::int32_t>& update(const std::vector<detection>& dets, float dt)
  {
    return update(dets.data(), dets.size(), dt);
  }

private:
  // TCM internals. Not exposed: conf_weight is the tunable; these only shape
  // how fast the per-track confidence estimate adapts. Confidence lives in
  // [0,1] and detector jitter on it is typically a few percent.
  static constexpr float conf_sigma_a = 2.f;    // confidence units / s^2
  static constexpr float conf_meas_std = 0.05f; // confidence jitter std

  struct vacated
  {
    std::int32_t slot;
    std::array<float, N> position;
    double time;
  };

  static float
  distance(const std::array<float, N>& a, const std::array<float, N>& b) noexcept
  {
    float d2 = 0.f;
    for(std::size_t i = 0; i < N; i++)
    {
      const float d = a[i] - b[i];
      d2 += d * d;
    }
    return std::sqrt(d2);
  }

  void step_time(float dt)
  {
    if(dt < 0.f)
      dt = 0.f;
    m_now += dt;
    if(dt > 0.f)
    {
      const auto qc = kalman_pv_filter::cwna(conf_sigma_a, dt);
      for(auto& t : m_tracks)
      {
        t.kf.predict(dt, m_cfg.accel_sigma);
        t.conf_kf.predict(dt, qc);
        t.age += dt;
        t.time_since_seen += dt;
      }
    }
  }

  //! Association gate + cost. Returns false if the pair is implausible.
  //! @p high_stage selects the TCM confidence predictor (Kalman for the
  //! high-confidence stage, two-point linear for the low-confidence one).
  bool admissible(
      const track& t, const detection& d, float frame_dt, bool high_stage,
      float& cost) const noexcept
  {
    const auto pred = t.kf.position();
    const float dist = distance(pred, d.position);

    // The gate the config documents: one frame's travel budget, widened
    // sub-linearly with how long the track has been unseen.
    //
    // The measurement-noise allowance is not optional. `dist` compares a
    // predicted position against a *noisy* measurement, so it is on the order
    // of meas_std even for a perfectly tracked, perfectly stationary object.
    // Without the term, any detector whose noise approaches max_speed * dt
    // (0.033 units at 2 units/s and 60 fps) has its own detections gated out:
    // the track then coasts, dies and is reborn with a new id, several hundred
    // times a minute, with nothing else in the scene to confuse it with.
    // The revive gate below already carries the same allowance.
    const float coast = std::max(m_cfg.coast_time, 1e-3f);
    const float gate_r = std::max(
        m_cfg.max_speed * frame_dt * (1.f + t.time_since_seen / coast)
            + 3.f * m_cfg.meas_std,
        1e-6f);

    switch(m_cfg.gate)
    {
      case track_motion_gate::off:
        break;
      case track_motion_gate::max_speed:
        if(dist > gate_r)
          return false;
        break;
      case track_motion_gate::mahalanobis:
        // Covariance growth over missed frames widens this gate naturally.
        if(t.kf.gating_distance2(d.position, m_cfg.meas_std * m_cfg.meas_std)
           > m_cfg.mahalanobis_thresh)
          return false;
        break;
    }

    cost = dist / gate_r;

    // OC-SORT direction consistency, multi-dt (OC-SORT sec. 3.2 + Table 7):
    // prefer the detection that continues the track's OBSERVED motion,
    // summing the angle term over temporal baselines of 1, 2 and 3 detection
    // frames. A single-frame direction is dominated by measurement noise; a
    // longer baseline averages it out (their gains stop at dt=3 and decline
    // at dt=5, so we stop at 3). Observed displacements, not the Kalman
    // velocity: the Kalman estimate is effectively a dt~1 quantity, and the
    // real variable here is the temporal baseline.
    if(m_cfg.dir_weight > 0.f && t.obs_count >= 2)
    {
      const int max_dt = std::min<int>(3, t.obs_count - 1);
      float dir_cost = 0.f;
      int dir_terms = 0;
      for(int k = 1; k <= max_dt; k++)
      {
        // v: track displacement over the last k observation frames.
        // w: displacement from the observation k frames before the candidate
        //    to the candidate detection (the same k-frame span, one frame on).
        const auto& h0 = t.obs_hist[0];
        const auto& hv = t.obs_hist[std::size_t(k)];
        const auto& hw = t.obs_hist[std::size_t(k - 1)];
        float nv = 0.f, nw = 0.f, dot = 0.f;
        for(std::size_t i = 0; i < N; i++)
        {
          const float v = h0[i] - hv[i];
          const float w = d.position[i] - hw[i];
          nv += v * v;
          nw += w * w;
          dot += v * w;
        }
        if(nv > 1e-8f && nw > 1e-8f)
        {
          const float cosang
              = std::clamp(dot / (std::sqrt(nv) * std::sqrt(nw)), -1.f, 1.f);
          dir_cost += std::acos(cosang) / 3.14159265f;
          dir_terms++;
        }
      }
      // Averaged over the available baselines so the term keeps the [0,1]
      // scale (and the 0.2 default weight) of the single-dt original;
      // measured: the raw sum triples the penalty on true matches whose
      // one-frame direction is noise, and costs id switches under clutter.
      if(dir_terms > 0)
        cost += m_cfg.dir_weight * (dir_cost / float(dir_terms));
    }

    // Hybrid-SORT TCM: a track's detection confidence trends smoothly (it
    // sinks as an occluder approaches, rises on exit), so the predicted
    // confidence discriminates between tracks exactly when their positions
    // are entangled. Kalman prediction in the high-confidence stage; in the
    // low-confidence stage a two-point linear extrapolation instead, because
    // confidence jumps at occlusion boundaries and the Kalman lags there.
    if(m_cfg.conf_weight > 0.f)
    {
      float chat;
      if(high_stage)
        chat = t.conf_kf.p;
      else if(t.conf_prev >= 0.f)
        chat = t.confidence + (t.confidence - t.conf_prev);
      else
        chat = t.confidence;
      chat = std::clamp(chat, 0.f, 1.f);
      cost += m_cfg.conf_weight * std::abs(chat - d.confidence);
    }
    return true;
  }

  void apply_match(track& t, const detection& d)
  {
    const float r = std::max(m_cfg.meas_std * m_cfg.meas_std, 1e-12f);
    const float gap = t.time_since_seen;
    const bool was_lost = t.state == track_state::lost;

    if(was_lost && m_cfg.revive_reupdate && t.hits > 0)
    {
      // OC-SORT ORU: restore the filter to its state at the last measurement
      // and re-run it along a straight virtual trajectory across the gap, so
      // the revived track carries a sane velocity instead of lurching from the
      // coasted prediction to the new position.
      t.kf = t.kf_at_meas;
      const int steps
          = std::clamp(int(std::lround(gap / std::max(m_est_period, 1e-3f))), 1, 32);
      const float step_dt = gap / float(steps);
      for(int k = 1; k <= steps; k++)
      {
        const float a = float(k) / float(steps);
        std::array<float, N> z;
        for(std::size_t i = 0; i < N; i++)
          z[i] = t.last_meas[i] + a * (d.position[i] - t.last_meas[i]);
        t.kf.predict(step_dt, m_cfg.accel_sigma);
        t.kf.update(z, r);
      }
    }
    else
    {
      t.kf.update(d.position, r);
      // Velocity re-seed after a shorter gap: the observation-implied velocity
      // beats the drifted one.
      if(gap > 2.f * m_est_period && gap > 1e-4f && t.hits > 0)
      {
        std::array<float, N> v;
        for(std::size_t i = 0; i < N; i++)
          v[i] = (d.position[i] - t.last_meas[i]) / gap;
        t.kf.set_velocity(v);
      }
    }

    t.hit_history = (t.hit_history << 1) | 1u;
    t.hits++;
    t.consecutive_misses = 0;
    t.time_since_seen = 0.f;
    t.conf_prev = t.confidence; // c[t-2] for the linear TCM extrapolation
    t.confidence = d.confidence;
    t.conf_kf.update(d.confidence, conf_meas_std * conf_meas_std);
    // Push into the observed-position ring (obs_hist[0] tracks last_meas).
    for(std::size_t k = t.obs_hist.size() - 1; k > 0; k--)
      t.obs_hist[k] = t.obs_hist[k - 1];
    t.obs_hist[0] = d.position;
    if(t.obs_count < t.obs_hist.size())
      t.obs_count++;
    t.last_meas = d.position;
    t.kf_at_meas = t.kf;

    switch(t.state)
    {
      case track_state::provisional: {
        const auto window_mask
            = m_cfg.confirm_window >= 32 ? ~0u : ((1u << m_cfg.confirm_window) - 1u);
        const auto window_hits = std::popcount(t.hit_history & window_mask);
        if(d.confidence >= m_cfg.instant_confirm
           || (t.age >= m_cfg.confirm_time && window_hits >= int(m_cfg.confirm_hits)))
        {
          t.state = track_state::confirmed;
          m_events.confirmed.push_back(t.id);
          allocate_slot(t);
        }
        break;
      }
      case track_state::lost:
        t.state = track_state::revived;
        t.reacquired = true;
        m_events.revived.push_back(t.id);
        break;
      case track_state::coasting:
      case track_state::revived:
        t.state = track_state::confirmed;
        break;
      default:
        break;
    }
  }

  std::int32_t birth(const detection& d)
  {
    track t;
    t.id = m_next_id++;
    t.creation_time = m_now;
    t.confidence = d.confidence;
    t.last_meas = d.position;
    t.hit_history = 1;
    t.hits = 1;
    // Initial uncertainty: position at measurement noise, velocity wide open
    // (up to max_speed, 1 sigma at half of it).
    const float vp = std::max(m_cfg.meas_std * m_cfg.meas_std, 1e-12f);
    const float sv = 0.5f * std::max(m_cfg.max_speed, 1e-3f);
    t.kf.initiate(d.position, vp, sv * sv);
    t.kf_at_meas = t.kf;
    // TCM confidence filter: position at the detector's confidence jitter,
    // rate wide open enough to latch onto an occlusion ramp quickly.
    t.conf_kf.initiate(
        d.confidence, conf_meas_std * conf_meas_std, conf_sigma_a * conf_sigma_a);
    t.obs_hist[0] = d.position;
    t.obs_count = 1;
    t.filtered = d.position;
    for(auto& s : t.smoothers)
      s.assign_parameters(m_proto);
    m_events.entered.push_back(t.id);
    if(d.confidence >= m_cfg.instant_confirm)
    {
      t.state = track_state::confirmed;
      m_events.confirmed.push_back(t.id);
      allocate_slot(t);
    }
    m_tracks.push_back(std::move(t));
    return m_tracks.back().id;
  }

  void refresh_states_and_expire()
  {
    // Missed-frame transitions are time-based so that they also happen when the
    // detector goes silent and only advance() is being called.
    const float missed = 1.5f * m_est_period;
    for(auto& t : m_tracks)
    {
      switch(t.state)
      {
        case track_state::confirmed:
        case track_state::revived:
          if(t.time_since_seen > missed)
            t.state = track_state::coasting;
          break;
        case track_state::coasting:
          if(t.time_since_seen > m_cfg.coast_time)
            t.state = m_cfg.revive ? track_state::lost : track_state::expired;
          break;
        case track_state::lost:
          if(t.time_since_seen > m_cfg.coast_time + m_cfg.revive_time)
            t.state = track_state::expired;
          break;
        case track_state::provisional:
          // A tentative track may only miss as many consecutive frames as the
          // M-of-N window allows; it also dies with the coast window.
          if(t.consecutive_misses > m_cfg.confirm_window - m_cfg.confirm_hits
             || t.time_since_seen > m_cfg.coast_time)
            t.state = track_state::expired;
          break;
        default:
          break;
      }
    }

    // Exactly-once exit: the event fires here and the track is removed in the
    // same sweep, so it can never be reported dead twice.
    m_tracks.erase(
        std::remove_if(
            m_tracks.begin(), m_tracks.end(),
            [&](track& t) {
      if(t.state != track_state::expired)
        return false;
      free_slot(t);
      m_events.exited.push_back(t.id);
      return true;
    }),
        m_tracks.end());

    // Confirmed tracks left unslotted (quarantine, or a full bank) retry as
    // slots free up - without stealing, so one slot cannot ping-pong between
    // two tracks.
    for(auto& t : m_tracks)
      if(t.slot < 0 && t.emitted(false))
        allocate_slot(t, false);
  }

  void smooth_outputs(float dt)
  {
    for(auto& t : m_tracks)
    {
      const auto p = t.kf.position();
      if(m_cfg.smooth)
      {
        for(std::size_t i = 0; i < N; i++)
          t.filtered[i] = t.smoothers[i](p[i], dt);
      }
      else
      {
        t.filtered = p;
      }
    }
  }

  // --- Slots ---------------------------------------------------------------

  bool slot_occupied(std::int32_t s) const noexcept
  {
    for(const auto& t : m_tracks)
      if(t.slot == s)
        return true;
    return false;
  }

  bool slot_quarantined(std::int32_t s) const noexcept
  {
    for(const auto& v : m_vacated)
      if(v.slot == s)
        return true;
    return false;
  }

  void purge_vacated()
  {
    m_vacated.erase(
        std::remove_if(
            m_vacated.begin(), m_vacated.end(),
            [&](const vacated& v) { return m_now - v.time > m_cfg.slot_hold_time; }),
        m_vacated.end());
  }

  void take_vacated(std::int32_t s)
  {
    m_vacated.erase(
        std::remove_if(
            m_vacated.begin(), m_vacated.end(),
            [&](const vacated& v) { return v.slot == s; }),
        m_vacated.end());
  }

  void allocate_slot(track& t, bool allow_steal = true)
  {
    const auto count = std::int32_t(m_cfg.slot_count);
    if(count <= 0 || t.slot >= 0)
      return;
    purge_vacated();

    // A track re-entering near a spot something recently left is, more often
    // than not, the same physical object: give it the vacated slot back.
    if(m_cfg.allocation == track_slot_allocation::nearest_vacated && !m_vacated.empty())
    {
      const auto pos = t.kf.position();
      float best = std::numeric_limits<float>::max();
      std::int32_t best_slot = -1;
      for(const auto& v : m_vacated)
      {
        const float radius
            = m_cfg.max_speed * float(m_now - v.time) + 10.f * m_cfg.meas_std;
        const float d = distance(pos, v.position);
        if(d <= radius && d < best && !slot_occupied(v.slot))
        {
          best = d;
          best_slot = v.slot;
        }
      }
      if(best_slot >= 0)
      {
        take_vacated(best_slot);
        t.slot = best_slot;
        return;
      }
    }

    // Free and not quarantined, in the policy's order.
    const auto try_range = [&](bool allow_quarantined) -> std::int32_t {
      if(m_cfg.allocation == track_slot_allocation::round_robin)
      {
        for(std::int32_t k = 0; k < count; k++)
        {
          const std::int32_t s = (m_rr_cursor + k) % count;
          if(!slot_occupied(s) && (allow_quarantined || !slot_quarantined(s)))
          {
            m_rr_cursor = (s + 1) % count;
            return s;
          }
        }
      }
      else
      {
        for(std::int32_t s = 0; s < count; s++)
          if(!slot_occupied(s) && (allow_quarantined || !slot_quarantined(s)))
            return s;
      }
      return -1;
    };

    // Quarantined slots are strictly held back: an unslotted track retries
    // every step (see refresh_states_and_expire), so it picks the slot up as
    // soon as the hold lapses.
    const std::int32_t s = try_range(false);
    if(s >= 0)
    {
      take_vacated(s);
      t.slot = s;
      return;
    }

    // All slots owned: steal, if the policy allows. Only at confirmation time -
    // a retry must not steal, or two tracks would trade one slot forever.
    if(!allow_steal || m_cfg.steal == track_slot_steal::never)
      return;
    track* victim = nullptr;
    for(auto& o : m_tracks)
    {
      if(o.slot < 0 || &o == &t)
        continue;
      if(!victim)
      {
        victim = &o;
        continue;
      }
      const bool better = m_cfg.steal == track_slot_steal::stalest
                              ? o.time_since_seen > victim->time_since_seen
                              : o.confidence < victim->confidence;
      if(better)
        victim = &o;
    }
    if(victim)
    {
      t.slot = victim->slot;
      victim->slot = -1;
    }
  }

  void free_slot(track& t)
  {
    if(t.slot < 0)
      return;
    m_vacated.push_back({t.slot, t.kf.position(), m_now});
    t.slot = -1;
  }

  config m_cfg{};
  one_euro_filter<float> m_proto{};
  std::vector<track> m_tracks;
  std::int32_t m_next_id = 1;
  double m_now = 0.;
  double m_last_frame_time = 0.;
  float m_est_period = 1.f / 30.f;
  std::int32_t m_rr_cursor = 0;

  track_events m_events;
  ossia::small_vector<vacated, 8> m_vacated;

  // Per-frame scratch, reused so the hot path does not allocate once warm.
  std::vector<std::int32_t> m_assign;
  std::vector<std::int32_t> m_high, m_low, m_t_match;
  std::vector<char> m_d_used;
  std::vector<match_candidate> m_cands;
};

}
