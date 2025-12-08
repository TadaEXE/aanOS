#pragma once

namespace hal::sys {

[[noreturn]] void halt() noexcept;
[[noreturn]] void reboot() noexcept;
[[noreturn]] void shutdown() noexcept;

}  // namespace hal::sys
