#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

namespace ossia
{

template <typename T>
class triple_buffer
{
  static_assert(std::is_nothrow_swappable_v<T>, "T must be nothrow swappable");

  // State encoding (packed into one atomic uint8_t):
  //   bits [0:1] - middle buffer index (0, 1, or 2)
  //   bit  [2]   - dirty flag (1 = producer has written new data)
  //
  // The producer owns a thread-local `write_idx` and the consumer owns a
  // thread-local `read_idx`. The only shared atomic is `mid_state`.
  //
  // Producer: write to slot[write_idx], then exchange(write_idx | DIRTY).
  // Consumer: if dirty, exchange(read_idx | 0), then read from new read slot.

private:
  static constexpr uint8_t index_mask = 0x03;
  static constexpr uint8_t dirty_bit = 0x04;

  struct alignas(64) slot
  {
    T data{};

    slot() = default;

    template <typename U>
    explicit slot(U&& val)
        : data(std::forward<U>(val))
    {
    }
  };

  std::array<slot, 3> m_buffers{};

  alignas(64) std::atomic<uint8_t> m_mid_state{1};
  alignas(64) uint8_t m_write_idx{0};
  alignas(64) uint8_t m_read_idx{2};

  triple_buffer(const triple_buffer&) = delete;
  triple_buffer& operator=(const triple_buffer&) = delete;
  triple_buffer(triple_buffer&&) = delete;
  triple_buffer& operator=(triple_buffer&&) = delete;

public:
  explicit triple_buffer(const T& init) noexcept(std::is_nothrow_copy_constructible_v<T>)
      : m_buffers{slot{init}, slot{init}, slot{init}}
  {
  }

  triple_buffer() noexcept(std::is_nothrow_default_constructible_v<T>)
    requires std::is_default_constructible_v<T>
  {
  }

  void produce(T& value) noexcept
  {
    using std::swap;
    swap(m_buffers[m_write_idx].data, value);

    const uint8_t new_mid = static_cast<uint8_t>(m_write_idx | dirty_bit);
    const uint8_t old_mid = m_mid_state.exchange(new_mid, std::memory_order_acq_rel);
    m_write_idx = old_mid & index_mask;
  }

  void produce(T&& value) noexcept(std::is_nothrow_move_assignable_v<T>)
  {
    m_buffers[m_write_idx].data = std::move(value);

    const uint8_t new_mid = static_cast<uint8_t>(m_write_idx | dirty_bit);
    const uint8_t old_mid = m_mid_state.exchange(new_mid, std::memory_order_acq_rel);
    m_write_idx = old_mid & index_mask;
  }

  // Consume latest data if available.  Returns true and updates `result`
  // if new data was produced since the last consume(); false otherwise.
  bool consume(T& result) noexcept
  {
    const uint8_t state = m_mid_state.load(std::memory_order_acquire);
    if(!(state & dirty_bit))
    {
      return false;
    }

    const uint8_t new_mid = m_read_idx;
    const uint8_t old_mid = m_mid_state.exchange(new_mid, std::memory_order_acq_rel);
    m_read_idx = old_mid & index_mask;

    using std::swap;
    swap(result, m_buffers[m_read_idx].data);
    return true;
  }

  // Return a const reference to the consumer's current slot.
  // Only available for copyable types.
  //
  // Note: after consume(), the slot holds the value that was swapped *in*
  // from the caller's `result`.  For a stable "last consumed value",
  // keep your own copy.
  const T& read_buffer() const noexcept
    requires std::is_copy_assignable_v<T>
  {
    return m_buffers[m_read_idx].data;
  }

  bool has_new_data() const noexcept
  {
    return m_mid_state.load(std::memory_order_acquire) & dirty_bit;
  }
};

template <typename T>
  requires std::is_trivially_copyable_v<T>
class triple_buffer<T>
{
private:
  static constexpr uint8_t index_mask = 0x03;
  static constexpr uint8_t dirty_bit = 0x04;

  struct alignas(64) slot
  {
    T data{};
  };

  std::array<slot, 3> m_buffers{};

  alignas(64) std::atomic<uint8_t> m_mid_state{1};
  alignas(64) uint8_t m_write_idx{0};
  alignas(64) uint8_t m_read_idx{2};

  // Cached copy of last consumed value so read() is always stable.
  alignas(64) T m_last_read{};

  triple_buffer(const triple_buffer&) = delete;
  triple_buffer& operator=(const triple_buffer&) = delete;
  triple_buffer(triple_buffer&&) = delete;
  triple_buffer& operator=(triple_buffer&&) = delete;

public:
  explicit triple_buffer(T init) noexcept
      : m_buffers{slot{init}, slot{init}, slot{init}}
      , m_last_read{init}
  {
  }

  triple_buffer() noexcept = default;

  void produce(T value) noexcept
  {
    m_buffers[m_write_idx].data = value;

    const uint8_t new_mid = static_cast<uint8_t>(m_write_idx | dirty_bit);
    const uint8_t old_mid = m_mid_state.exchange(new_mid, std::memory_order_acq_rel);
    m_write_idx = old_mid & index_mask;
  }

  // Consume latest data if available.  Returns true and updates `result`
  //with the newest value; false if no new data since last consume().
  bool consume(T& result) noexcept
  {
    const uint8_t state = m_mid_state.load(std::memory_order_acquire);
    if(!(state & dirty_bit))
    {
      return false;
    }

    const uint8_t new_mid = m_read_idx;
    const uint8_t old_mid = m_mid_state.exchange(new_mid, std::memory_order_acq_rel);
    m_read_idx = old_mid & index_mask;

    m_last_read = m_buffers[m_read_idx].data;
    result = m_last_read;
    return true;
  }

  // Always valid
  T read() const noexcept { return m_last_read; }

  bool has_new_data() const noexcept
  {
    return m_mid_state.load(std::memory_order_acquire) & dirty_bit;
  }
};

template <typename T, typename Container = std::vector<T>>
class triple_buffer_raw
{
  static_assert(
      std::is_nothrow_swappable_v<Container>, "Container must be nothrow swappable");

private:
  static constexpr uint8_t index_mask = 0x03;
  static constexpr uint8_t dirty_bit = 0x04;

  struct alignas(64) slot
  {
    Container data{};

    slot() = default;

    template <typename U>
    explicit slot(U&& val)
        : data(std::forward<U>(val))
    {
    }
  };

  std::array<slot, 3> m_buffers{};

  alignas(64) std::atomic<uint8_t> m_mid_state{1};
  alignas(64) uint8_t m_write_idx{0};
  alignas(64) uint8_t m_read_idx{2};

  triple_buffer_raw(const triple_buffer_raw&) = delete;
  triple_buffer_raw& operator=(const triple_buffer_raw&) = delete;
  triple_buffer_raw(triple_buffer_raw&&) = delete;
  triple_buffer_raw& operator=(triple_buffer_raw&&) = delete;

public:
  triple_buffer_raw() noexcept(std::is_nothrow_default_constructible_v<Container>)
      = default;

  // Pre-allocate each slot with a given capacity.
  explicit triple_buffer_raw(std::size_t initial_capacity)
  {
    for(auto& s : m_buffers)
      s.data.reserve(initial_capacity);
  }

  // Direct access to the write slot. The producer writes into this
  // container however it likes, then calls publish().
  Container& write_buffer() noexcept { return m_buffers[m_write_idx].data; }

  // Publish the current write buffer: swap it into the middle slot
  // and pick up a new (stale) write slot.
  void publish() noexcept
  {
    const uint8_t new_mid = static_cast<uint8_t>(m_write_idx | dirty_bit);
    const uint8_t old_mid = m_mid_state.exchange(new_mid, std::memory_order_acq_rel);
    m_write_idx = old_mid & index_mask;
  }

  // Convenience: assign from an iterator range, then publish.
  template <typename InputIt>
  void produce(InputIt first, InputIt last)
  {
    m_buffers[m_write_idx].data.assign(first, last);
    publish();
  }

  // Convenience: assign from a span, then publish.
  void produce(std::span<const T> src)
  {
    m_buffers[m_write_idx].data.assign(src.begin(), src.end());
    publish();
  }

  // Attempt to consume. Returns true if new data was swapped in.
  // After a successful consume(), read_span() / read_buffer()
  // reflect the new data.
  bool consume() noexcept
  {
    const uint8_t state = m_mid_state.load(std::memory_order_acquire);
    if(!(state & dirty_bit))
      return false;

    const uint8_t new_mid = m_read_idx;
    const uint8_t old_mid = m_mid_state.exchange(new_mid, std::memory_order_acq_rel);
    m_read_idx = old_mid & index_mask;
    return true;
  }

  // View of the consumer's current slot.
  std::span<const T> read_span() const noexcept
  {
    const auto& c = m_buffers[m_read_idx].data;
    return {c.data(), c.size()};
  }

  const Container& read_buffer() const noexcept { return m_buffers[m_read_idx].data; }

  bool has_new_data() const noexcept
  {
    return m_mid_state.load(std::memory_order_acquire) & dirty_bit;
  }
};
}
