#pragma once

#include <cstddef>

namespace memory {

/// Very simple bump allocator.
///
/// - Grows forward inside a fixed-size buffer
/// - Never frees (until reboot)
/// - Returns nullptr on OOM (global new will just spin then)
void* alloc(size_t size, size_t align = alignof(max_align_t)) noexcept;

void free(void* /*ptr*/) noexcept;  // currently a no-op

/// Optional: reset heap to empty (not used yet, but handy for tests)
void reset() noexcept;

}  // namespace memory
