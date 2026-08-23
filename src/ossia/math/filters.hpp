#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/math.hpp>
#include <ossia/detail/small_vector.hpp>

#include <cmath>

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>

/**
 * \file filters.hpp
 *
 * Allocation-free scalar smoothing filters, meant to be used per-component on
 * the audio or the execution thread.
 *
 * Everything here is an aggregate: trivially copyable, cheap to reset by
 * assignment, and cheap to keep one per component or per tracked object. No
 * virtuals, and no transcendentals in the hot path - the low-pass coefficient
 * is the usual first-order approximation, a single divide.
 *
 * The time-varying filters take an explicit dt (in seconds) rather than a fixed
 * sample rate: score ticks value processes at a variable block rate, so a rate
 * captured once at prepare() time is wrong as soon as the buffer size changes.
 */

namespace ossia
{

/**
 * @brief Smoothing factor of a first-order low-pass, for a sample spaced
 * @p dt seconds from the previous one.
 *
 * @param cutoff Cutoff frequency, in Hz. Must be > 0.
 * @param dt Time since the previous sample, in seconds. Must be > 0.
 * @return alpha in ]0, 1[ ; 1 would be passthrough, 0 a frozen output.
 */
template <typename T>
[[nodiscard]] OSSIA_INLINE T lowpass_alpha(T cutoff, T dt) noexcept
{
  const T r = T(ossia::two_pi) * cutoff * dt;
  return r / (r + T(1));
}

/**
 * @brief Smoothing factor of a first-order lag with a given time constant.
 *
 * Equivalent to lowpass_alpha(1 / (2 * pi * tau), dt), spelled in the unit that
 * is usually more natural when a UI exposes a "smoothing time" instead of a
 * cutoff.
 *
 * @param tau Time constant, in seconds. Must be > 0.
 * @param dt Time since the previous sample, in seconds. Must be > 0.
 */
template <typename T>
[[nodiscard]] OSSIA_INLINE T lag_alpha(T tau, T dt) noexcept
{
  return dt / (dt + tau);
}

/**
 * @brief First-order low-pass. The building block for everything below.
 *
 * The coefficient is passed per-sample instead of being stored, so that the
 * same filter can be driven by a varying dt or a varying cutoff without
 * recomputing any internal state.
 */
template <typename T = float>
struct one_pole_filter
{
  using value_type = T;

  //! Last output. Only meaningful once primed.
  T y{};

  //! False until the first sample went through: the first sample is passed
  //! along untouched rather than being smoothed from an arbitrary zero.
  bool primed{};

  //! Forget the history: the next sample restarts the filter.
  OSSIA_INLINE void reset() noexcept
  {
    y = T{};
    primed = false;
  }

  //! Restart the filter as if @p x had just been output.
  OSSIA_INLINE void reset(T x) noexcept
  {
    y = x;
    primed = true;
  }

  //! No runtime parameters: the coefficient is passed per-sample.
  OSSIA_INLINE void assign_parameters(const one_pole_filter&) noexcept { }

  /**
   * @param x Input sample.
   * @param alpha Smoothing factor in [0, 1], e.g. from lowpass_alpha().
   */
  [[nodiscard]] OSSIA_INLINE T operator()(T x, T alpha) noexcept
  {
    if(!primed) [[unlikely]]
    {
      primed = true;
      return y = x;
    }

    // One FMA, versus two multiplies for the alpha * x + (1 - alpha) * y form.
    return y += alpha * (x - y);
  }
};

/**
 * @brief One-euro filter: a low-pass whose cutoff rises with the speed of the
 * signal.
 *
 * Casiez, Roussel and Vogel, "1 euro filter: a simple speed-based low-pass
 * filter for noisy input in interactive systems", CHI 2012.
 *
 * The point is the jitter/lag tradeoff: at rest the cutoff drops to
 * @ref min_cutoff and the output is very smooth, while a fast movement raises
 * the cutoff so the output stays responsive. Per the authors: if lag during
 * fast motion is the problem, raise @ref beta ; if jitter at rest is the
 * problem, lower @ref min_cutoff.
 *
 * The derivative is taken against the previous *raw* input, as in the reference
 * implementation.
 */
template <typename T = float>
struct one_euro_filter
{
  using value_type = T;

  //! Cutoff at zero speed, in Hz. Lower is smoother, and laggier.
  T min_cutoff{1};

  //! How much the speed raises the cutoff. 0 degrades to a plain low-pass.
  T beta{0};

  //! Cutoff of the low-pass applied to the derivative itself, in Hz.
  T d_cutoff{1};

  //! Smoother for the value.
  one_pole_filter<T> x_filter{};

  //! Smoother for the derivative.
  one_pole_filter<T> dx_filter{};

  //! Previous raw input.
  T x_prev{};

  //! Forget the history: the next sample restarts the filter.
  OSSIA_INLINE void reset() noexcept
  {
    x_filter.reset();
    dx_filter.reset();
    x_prev = T{};
  }

  //! Adopt @p p's tuning, keeping the history.
  OSSIA_INLINE void assign_parameters(const one_euro_filter& p) noexcept
  {
    min_cutoff = p.min_cutoff;
    beta = p.beta;
    d_cutoff = p.d_cutoff;
  }

  /**
   * @param x Input sample.
   * @param dt Time since the previous sample, in seconds.
   *
   * A dt <= 0 returns the previous output unchanged rather than dividing by
   * zero, so that a duplicated timestamp cannot poison the state.
   */
  [[nodiscard]] OSSIA_INLINE T operator()(T x, T dt) noexcept
  {
    if(dt <= T(0)) [[unlikely]]
      return x_filter.primed ? x_filter.y : x;

    // Shared between both alphas: alpha = (k * cutoff) / (k * cutoff + 1).
    const T k = T(ossia::two_pi) * dt;

    // First sample: no derivative to speak of.
    const T dx = x_filter.primed ? (x - x_prev) / dt : T(0);
    x_prev = x;

    const T kd = k * d_cutoff;
    const T edx = dx_filter(dx, kd / (kd + T(1)));

    const T cutoff = min_cutoff + beta * std::abs(edx);
    const T kc = k * cutoff;
    return x_filter(x, kc / (kc + T(1)));
  }
};

/**
 * @brief Simple moving average over the last @p N samples.
 *
 * O(1) per sample: the window sum is kept incrementally instead of being
 * recomputed. The accumulator is widened to double for float inputs, because a
 * running sum drifts over a long performance.
 *
 * Until @p N samples have been seen the average is taken over what is
 * available, so the output is usable from the first sample on.
 */
template <typename T = float, std::size_t N = 8>
struct moving_average_filter
{
  static_assert(N >= 1, "a moving average needs at least one sample");

  using value_type = T;

  //! Widened so that the incremental sum does not drift.
  using accumulator_type = std::conditional_t<std::is_same_v<T, float>, double, T>;

  //! Chronological ring of the window contents.
  std::array<T, N> history{};

  //! Running sum of the first @ref count entries of the window.
  accumulator_type sum{};

  //! Where the next sample goes in @ref history.
  std::uint32_t head{};

  //! How many entries of @ref history are valid, saturating at N.
  std::uint32_t count{};

  //! Forget the history: the next sample restarts the filter.
  OSSIA_INLINE void reset() noexcept
  {
    sum = accumulator_type{};
    head = 0;
    count = 0;
  }

  //! No runtime parameters: the window size is a template argument.
  OSSIA_INLINE void assign_parameters(const moving_average_filter&) noexcept { }

  [[nodiscard]] OSSIA_INLINE T operator()(T x) noexcept
  {
    if(count == N)
      sum -= history[head];
    else
      ++count;

    sum += x;
    history[head] = x;
    head = (head + 1 == N) ? 0 : head + 1;

    return T(sum / accumulator_type(count));
  }
};

/**
 * @brief Running median over the last @p N samples.
 *
 * Unlike an average, this rejects isolated outliers outright, which is what one
 * wants against the occasional bogus reading of a sensor or a tracker.
 *
 * Kept as a chronological ring plus a sorted copy of the same window: an insert
 * is a binary search and a small contiguous move, which for the window sizes
 * this is meant for beats any heap-based scheme by a wide margin.
 *
 * @note Feeding a NaN breaks the ordering invariant of the sorted window, and
 * with it every subsequent output. Filter them out upstream.
 */
template <typename T = float, std::size_t N = 5>
struct median_filter
{
  static_assert(N >= 1, "a median needs at least one sample");

  using value_type = T;

  //! Chronological ring of the window contents.
  std::array<T, N> history{};

  //! The same contents, kept sorted over the first @ref count entries.
  std::array<T, N> sorted{};

  //! Where the next sample goes in @ref history.
  std::uint32_t head{};

  //! How many entries are valid, saturating at N.
  std::uint32_t count{};

  //! Forget the history: the next sample restarts the filter.
  OSSIA_INLINE void reset() noexcept
  {
    head = 0;
    count = 0;
  }

  //! No runtime parameters: the window size is a template argument.
  OSSIA_INLINE void assign_parameters(const median_filter&) noexcept { }

  [[nodiscard]] OSSIA_INLINE T operator()(T x) noexcept
  {
    const auto begin = sorted.begin();

    if(count == N)
    {
      // Drop the sample falling out of the window, then insert the new one.
      const auto last = begin + N;
      const auto old = std::lower_bound(begin, last, history[head]);
      std::move(old + 1, last, old);

      const auto pos = std::lower_bound(begin, last - 1, x);
      std::move_backward(pos, last - 1, last);
      *pos = x;
    }
    else
    {
      const auto last = begin + count;
      const auto pos = std::lower_bound(begin, last, x);
      std::move_backward(pos, last, last + 1);
      *pos = x;
      ++count;
    }

    history[head] = x;
    head = (head + 1 == N) ? 0 : head + 1;

    const std::uint32_t n = count;
    if(n & 1u)
      return sorted[n / 2];
    return (sorted[n / 2 - 1] + sorted[n / 2]) * T(0.5);
  }
};

/**
 * @brief Applies one independent @p Filter per component of a vector-valued
 * signal.
 *
 * The filters are owned per-component so that, for instance, the x of a tracked
 * point never bleeds into its y - and, when used per tracked object, so that
 * one object's history never bleeds into another's.
 *
 * @tparam N How many components fit without allocating.
 */
template <typename Filter, std::size_t N = 4>
struct multi_filter
{
  using filter_type = Filter;
  using value_type = typename Filter::value_type;

  //! Copied into every new component. Set the parameters here, then call
  //! configure() to push them onto the components already in flight.
  Filter prototype{};

  //! One filter per component.
  ossia::small_vector<Filter, N> filters{};

  /**
   * @brief Make sure there are exactly @p n components.
   *
   * A change of component count resets the temporal state: there is no
   * meaningful way to map an old component onto a new one.
   */
  OSSIA_INLINE void ensure(std::size_t n)
  {
    if(filters.size() != n) [[unlikely]]
      filters.assign(n, prototype);
  }

  //! Push the prototype's tuning onto the live filters, keeping their history.
  OSSIA_INLINE void configure() noexcept
  {
    for(auto& f : filters)
      f.assign_parameters(prototype);
  }

  //! Forget every component's history, keeping the component count.
  void reset() noexcept
  {
    for(auto& f : filters)
      f.reset();
  }

  //! Drop every component.
  void clear() noexcept { filters.clear(); }

  /**
   * @brief Filter @p n components in place.
   *
   * @param v Components, modified in place.
   * @param n How many.
   * @param args Forwarded to each filter, e.g. the dt of a one_euro_filter.
   */
  template <typename... Args>
  OSSIA_INLINE void operator()(value_type* v, std::size_t n, Args... args) noexcept
  {
    ensure(n);
    for(std::size_t i = 0; i < n; i++)
      v[i] = filters[i](v[i], args...);
  }
};

/**
 * @brief Move @p a onto the branch closest to @p reference.
 *
 * Angles wrap, so a naive filter run over them lurches by a full turn every
 * time the signal crosses the discontinuity. Unwrapping against the previous
 * value first makes them safe to smooth.
 *
 * Branchless, unlike the usual while-loop form.
 */
template <typename T>
[[nodiscard]] OSSIA_INLINE T unwrap_angle(T a, T reference) noexcept
{
  // remainder() lands in [-pi, pi] for a divisor of two_pi.
  return reference + T(ossia::remainder(a - reference, T(ossia::two_pi)));
}

}
