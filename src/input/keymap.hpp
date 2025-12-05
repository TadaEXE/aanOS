#pragma once

#include "hal/keyboard.hpp"

namespace input {
bool key_event_to_char(const hal::KeyEvent& ev, char& out) noexcept;
}
