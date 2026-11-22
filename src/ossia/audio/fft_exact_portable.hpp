#pragma once

#include <algorithm>
#include <bit>
#include <cmath>
#include <complex>
#include <cstddef>
#include <limits>
#include <numbers>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ossia::detail
{
class exact_fft_portable
{
  using complex = std::complex<float>;

public:
  explicit exact_fft_portable(std::size_t size, bool inverse)
      : m_size{size}
      , m_work(working_size(size))
      , m_twiddles(m_work.size() / 2)
      , m_chirp(m_work.size() == size ? 0 : size)
      , m_kernel(m_chirp.empty() ? 0 : m_work.size())
  {
    const double sign = inverse ? 1. : -1.;
    const double step = (m_chirp.empty() ? sign : -1.) * 2. * std::numbers::pi
                        / static_cast<double>(m_work.size());
    for(std::size_t i = 1; i < m_twiddles.size(); ++i)
      m_twiddles[i] = unit_root(step * static_cast<double>(i));

    if(m_chirp.empty())
      return;

    // Bluestein rewrites the DFT as a chirp multiplication, a convolution
    // with the conjugate chirp at positive and negative lags, and another
    // chirp multiplication. The power-of-two work size prevents wraparound.
    const auto period = 2 * size;
    const double chirp_step = sign * std::numbers::pi / static_cast<double>(size);
    m_kernel[0] = {1.f, 0.f};
    std::size_t square = 1;
    for(std::size_t i = 1; i < size; ++i)
    {
      // Advance i^2 modulo 2N without squaring an index or forming a large angle.
      m_chirp[i] = unit_root(chirp_step * static_cast<double>(square));
      m_kernel[i] = std::conj(m_chirp[i]);
      m_kernel[m_kernel.size() - i] = m_kernel[i];
      square += 2 * i + 1;
      if(square >= period)
        square -= period;
    }

    natural_to_reversed(m_kernel);
    // Normalize the convolution once, not the caller's transform.
    const float scale = 1.f / static_cast<float>(m_kernel.size());
    for(auto& bin : m_kernel)
      bin *= scale;
  }

  [[nodiscard]] std::span<complex> input() noexcept { return {m_work.data(), m_size}; }

  void execute() noexcept
  {
    if(m_chirp.empty())
    {
      natural_to_reversed(m_work);
      // Bit reversal is its own inverse. Radix-2 plans need no other buffer.
      for(std::size_t i = 1, j = 0; i < m_size; ++i)
      {
        auto bit = m_size / 2;
        for(; j & bit; bit /= 2)
          j ^= bit;
        j ^= bit;
        if(i < j)
          std::swap(m_work[i], m_work[j]);
      }
      return;
    }

    for(std::size_t i = 1; i < m_size; ++i)
      m_work[i] *= m_chirp[i];
    std::fill(m_work.begin() + m_size, m_work.end(), complex{});
    natural_to_reversed(m_work);
    for(std::size_t i = 0; i < m_work.size(); ++i)
      m_work[i] *= m_kernel[i];
    convolution_inverse();
  }

  [[nodiscard]] std::span<const complex> output() const noexcept
  {
    return {m_work.data(), m_size};
  }

private:
  static std::size_t working_size(std::size_t size)
  {
    if(size == 0)
      throw std::invalid_argument{"FFT size must be positive"};
    if(std::has_single_bit(size))
      return size;

    constexpr auto max_power_of_two
        = std::size_t{1} << (std::numeric_limits<std::size_t>::digits - 1);
    if(size > max_power_of_two / 2)
      throw std::length_error{"FFT convolution size is too large"};
    return std::bit_ceil(2 * size - 1);
  }

  static complex unit_root(double angle) noexcept
  {
    return {static_cast<float>(std::cos(angle)), static_cast<float>(std::sin(angle))};
  }

  // Decimation in frequency: the spectrum is stored in bit-reversed order.
  void natural_to_reversed(std::span<complex> data) const noexcept
  {
    for(std::size_t half = data.size() / 2, step = 1; half != 0; half /= 2, step *= 2)
    {
      const auto width = 2 * half;
      for(std::size_t base = 0; base < data.size(); base += width)
      {
        const auto first = data[base];
        data[base] += data[base + half];
        data[base + half] = first - data[base + half];
        for(std::size_t j = 1, root = step; j < half; ++j, root += step)
        {
          const auto a = data[base + j];
          const auto b = data[base + half + j];
          data[base + j] = a + b;
          data[base + half + j] = (a - b) * m_twiddles[root];
        }
      }
    }
  }

  // Decimation in time consumes the bit-reversed convolution spectrum and
  // returns natural order, so Bluestein needs no explicit permutations.
  void convolution_inverse() noexcept
  {
    const auto last_half = m_work.size() / 2;
    for(std::size_t half = 1, step = last_half; half < last_half; half *= 2, step /= 2)
    {
      const auto width = 2 * half;
      for(std::size_t base = 0; base < m_work.size(); base += width)
      {
        const auto first = m_work[base];
        m_work[base] += m_work[base + half];
        m_work[base + half] = first - m_work[base + half];
        for(std::size_t j = 1, root = step; j < half; ++j, root += step)
        {
          const auto a = m_work[base + j];
          const auto b = m_work[base + half + j] * std::conj(m_twiddles[root]);
          m_work[base + j] = a + b;
          m_work[base + half + j] = a - b;
        }
      }
    }

    // The last stage only needs the first N outputs, all within its first half.
    m_work[0] += m_work[last_half];
    for(std::size_t i = 1; i < m_size; ++i)
      m_work[i] = (m_work[i] + m_work[last_half + i] * std::conj(m_twiddles[i]))
                  * m_chirp[i];
  }

  std::size_t m_size;
  std::vector<complex> m_work;
  std::vector<complex> m_twiddles;
  std::vector<complex> m_chirp;
  std::vector<complex> m_kernel;
};
}
