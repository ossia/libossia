#pragma once
#include <ossia/network/dataspace/gain.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>

/**
 * Level meter ballistics: what a meter needle or bar shows for a signal,
 * per the standards that define each kind of meter.
 *
 * Each follower takes blocks of samples and holds a linear level, scaled so
 * that a steady sine of amplitude A reads A (except the RMS follower, which
 * reads the RMS value unless asked for AES-17 sine referencing).
 * Nothing allocates after prepare().
 */
namespace ossia::meters
{
//! Per-sample factor of an exponential fall of `db_per_s` decibels per second.
inline double fall_coefficient(double db_per_s, double rate) noexcept
{
  return std::pow(10., -db_per_s / (20. * rate));
}

/**
 * Digital sample peak meter (IEC 60268-18): instant attack, release at a
 * constant rate in dB/s.
 */
struct sample_peak
{
  double level{};
  double fall{};

  void prepare(double rate, double fall_db_per_s = 20.) noexcept
  {
    fall = fall_coefficient(fall_db_per_s, rate);
    level = 0.;
  }

  void process(const auto* in, int n) noexcept
  {
    double l = level;
    for(int i = 0; i < n; i++)
      l = std::max(double(std::abs(in[i])), l * fall);
    level = l;
  }
};

/**
 * Mean-square follower: a one-pole average of x^2 that reaches 99% of a step
 * after `integration` seconds; level() is its square root.
 *
 * With sine_referenced (AES-17), the reading is raised by 3.01 dB so that a
 * sine reads its peak amplitude, as K-System meters do.
 */
struct rms
{
  double ms{};
  double coef{};
  bool sine_referenced{};

  void prepare(double rate, double integration = 0.3, bool aes17 = false) noexcept
  {
    const double tau = integration / std::log(100.);
    coef = 1. - std::exp(-1. / (tau * rate));
    sine_referenced = aes17;
    ms = 0.;
  }

  void process(const auto* in, int n) noexcept
  {
    double m = ms;
    for(int i = 0; i < n; i++)
    {
      const double x = in[i];
      m += coef * (x * x - m);
    }
    ms = m;
  }

  double level() const noexcept
  {
    const double r = std::sqrt(std::max(ms, 0.));
    return sine_referenced ? r * std::numbers::sqrt2 : r;
  }
};

/**
 * VU meter (IEC 60268-17): full-wave average through a second-order system
 * that reaches 99% of a steady tone in 300 ms with a 1.25% overshoot
 * (the standard allows 1 to 1.5%). Scaled so that a sine reads its RMS
 * value; 0 VU is wherever the reference level puts it.
 */
struct vu
{
  static constexpr double zeta = 0.8127169864286775;
  //! 300 ms to 99% of a step, for that damping
  static constexpr double omega = 13.511933333385695;
  //! Full-wave average of a sine to its RMS value: pi / (2 sqrt(2))
  static constexpr double sine_average_to_rms = 1.1107207345395915;

  double y{}, v{};
  double dt{};

  void prepare(double rate) noexcept
  {
    dt = 1. / rate;
    y = v = 0.;
  }

  void process(const auto* in, int n) noexcept
  {
    double yy = y, vv = v;
    const double w2 = omega * omega, d = 2. * zeta * omega;
    for(int i = 0; i < n; i++)
    {
      const double x = std::abs(double(in[i]));
      vv += (w2 * (x - yy) - d * vv) * dt;
      yy += vv * dt;
    }
    y = yy;
    v = vv;
  }

  double level() const noexcept { return std::max(y, 0.) * sine_average_to_rms; }
};

/**
 * Quasi-peak programme meter (IEC 60268-10).
 *
 * The rise follows the signal's rectified value through a one-pole charge
 * whose speed is set so that a 5 kHz tone burst lasting the integration time
 * reads 2 dB below the same tone held steady; the fall is exponential at the
 * standard's rate. Type I (DIN, Nordic): 5 ms, 20 dB in 1.5 s.
 * Type II (BBC, EBU): 10 ms, 24 dB in 2.8 s.
 */
struct ppm
{
  enum type
  {
    type_i,
    type_ii
  };

  double env{};
  double attack{};
  double fall{};
  double gain{1.};

  static constexpr double integration_time(type t) noexcept
  {
    return t == type_i ? 0.005 : 0.010;
  }
  static constexpr double fall_db_per_s(type t) noexcept
  {
    return t == type_i ? 20. / 1.5 : 24. / 2.8;
  }

  //! The design the coefficients were computed for: preparing again for
  //! the same one only clears the envelope.
  double designed_rate{};
  type designed_type{};

  void prepare(double rate, type t) noexcept
  {
    env = 0.;
    if(rate == designed_rate && t == designed_type)
      return;
    designed_rate = rate;
    designed_type = t;
    fall = fall_coefficient(fall_db_per_s(t), rate);

    // Steady reading for a given attack, to scale a steady sine to its peak
    auto steady = [&](double a) {
      ppm p{.attack = a, .fall = fall};
      return p.tone_reading(rate, 0.1);
    };
    auto burst = [&](double a) {
      ppm p{.attack = a, .fall = fall};
      return p.tone_reading(rate, integration_time(t));
    };

    // The burst reading rises with the attack coefficient: bisect it
    const double target = ossia::detail::DecibelsToLinearGain(-2.);
    double lo = 1e-7, hi = 1.;
    for(int i = 0; i < 60; i++)
    {
      const double mid = std::sqrt(lo * hi);
      if(burst(mid) / steady(mid) < target)
        lo = mid;
      else
        hi = mid;
    }
    attack = std::sqrt(lo * hi);
    gain = 1. / steady(attack);
  }

  void process(const auto* in, int n) noexcept
  {
    double e = env;
    for(int i = 0; i < n; i++)
    {
      const double x = std::abs(double(in[i]));
      e *= fall;
      if(x > e)
        e += attack * (x - e);
    }
    env = e;
  }

  double level() const noexcept { return env * gain; }

  //! Highest reading reached during a 5 kHz unit sine lasting `duration`.
  double tone_reading(double rate, double duration) noexcept
  {
    env = 0.;
    double peak = 0.;
    const int n = std::max(1, int(duration * rate));
    // The sine by rotation of a phasor, renormalized now and then
    const double w = 2. * std::numbers::pi * 5000. / rate;
    const double cw = std::cos(w), sw = std::sin(w);
    double re = 1., im = 0.;
    for(int i = 0; i < n; i++)
    {
      process(&im, 1);
      peak = std::max(peak, env);
      const double r = re * cw - im * sw;
      im = re * sw + im * cw;
      re = r;
      if((i & 1023) == 1023)
      {
        const double norm = 1. / std::sqrt(re * re + im * im);
        re *= norm;
        im *= norm;
      }
    }
    env = 0.;
    return peak;
  }
};

/**
 * True peak (ITU-R BS.1770 Annex 2): the peak of the signal oversampled
 * 4 times below 96 kHz, twice below 192 kHz, with a 48-tap windowed-sinc
 * polyphase interpolator; at 192 kHz and above, the sample peak. Holds the
 * highest absolute value of the last process() call in `block_peak`.
 */
struct true_peak
{
  static constexpr int taps_per_phase = 12;
  static constexpr int max_factor = 4;

  int factor{1};
  std::array<std::array<double, taps_per_phase>, max_factor> coefs{};
  //! The last taps_per_phase samples, stored twice so that the newest ones
  //! are always contiguous: history[pos + 1 .. pos + taps_per_phase].
  std::array<double, 2 * taps_per_phase> history{};
  int pos{};
  double block_peak{};

  void prepare(double rate) noexcept
  {
    factor = rate < 96000. ? 4 : rate < 192000. ? 2 : 1;
    history.fill(0.);
    pos = 0;

    // Kaiser-windowed sinc, cutoff at the original Nyquist frequency
    const int len = taps_per_phase * factor;
    const double beta = 7.865;
    auto bessel_i0 = [](double x) {
      double sum = 1., term = 1.;
      for(int k = 1; k < 32; k++)
      {
        term *= (x / (2. * k)) * (x / (2. * k));
        sum += term;
      }
      return sum;
    };
    const double i0b = bessel_i0(beta);
    for(int t = 0; t < len; t++)
    {
      const double m = t - (len - 1) / 2.;
      const double x = m / factor;
      const double sinc
          = std::abs(x) < 1e-12 ? 1. : std::sin(std::numbers::pi * x) / (std::numbers::pi * x);
      const double r = 2. * t / (len - 1) - 1.;
      const double win = bessel_i0(beta * std::sqrt(std::max(0., 1. - r * r))) / i0b;
      coefs[t % factor][t / factor] = sinc * win;
    }
  }

  void process(const auto* in, int n) noexcept
  {
    double peak = 0.;
    if(factor == 1)
    {
      for(int i = 0; i < n; i++)
        peak = std::max(peak, std::abs(double(in[i])));
      block_peak = peak;
      return;
    }
    for(int i = 0; i < n; i++)
    {
      pos = pos + 1 == taps_per_phase ? 0 : pos + 1;
      history[pos] = history[pos + taps_per_phase] = in[i];
      const double* h = history.data() + pos + 1;
      for(int ph = 0; ph < factor; ph++)
      {
        double acc = 0.;
        const auto& c = coefs[ph];
        for(int k = 0; k < taps_per_phase; k++)
          acc += c[k] * h[k];
        peak = std::max(peak, std::abs(acc));
      }
    }
    block_peak = peak;
  }
};

/**
 * Correlation between two channels, in [-1, 1]: their normalized cross
 * product averaged over about `integration` seconds. 0 for silence.
 */
struct correlation
{
  double lr{}, ll{}, rr{};
  double coef{};

  void prepare(double rate, double integration = 0.3) noexcept
  {
    const double tau = integration / std::log(100.);
    coef = 1. - std::exp(-1. / (tau * rate));
    lr = ll = rr = 0.;
  }

  void process(const auto* l, const auto* r, int n) noexcept
  {
    double a = lr, b = ll, c = rr;
    for(int i = 0; i < n; i++)
    {
      const double x = l[i], y = r[i];
      a += coef * (x * y - a);
      b += coef * (x * x - b);
      c += coef * (y * y - c);
    }
    lr = a;
    ll = b;
    rr = c;
  }

  double value() const noexcept
  {
    const double d = std::sqrt(ll * rr);
    return d > 1e-12 ? std::clamp(lr / d, -1., 1.) : 0.;
  }
};

/**
 * Peak hold: keeps the highest level for `hold` seconds, then lets it fall
 * at `fall_db_per_s`. Fed once per block with that block's level.
 */
struct peak_hold
{
  double value{};
  double hold_frames{};
  double since{};
  double fall_per_frame{};

  void prepare(double rate, double hold_s = 1.5, double fall_db_per_s = 20.) noexcept
  {
    hold_frames = hold_s * rate;
    fall_per_frame = fall_coefficient(fall_db_per_s, rate);
    value = 0.;
    since = 0.;
  }

  void feed(double level, int frames) noexcept
  {
    if(level >= value)
    {
      value = level;
      since = 0.;
      return;
    }
    since += frames;
    if(since > hold_frames)
      value = std::max(level, value * std::pow(fall_per_frame, frames));
  }
};
}
