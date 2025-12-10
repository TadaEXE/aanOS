#include "tty/tty.hpp"

#include <string_view>

#include "hal/keyboard.hpp"
#include "input/keymap.hpp"

namespace tty {

void Tty::write_char(char c) noexcept {
  display.put_char(c);
  display.flush();
}

void Tty::write(std::string_view sv) noexcept {
  for (auto c : sv) {
    display.put_char(c);
  }
  display.flush();
}

void Tty::write(const ctr::String& s) noexcept {
  write(std::string_view{s.data(), s.size()});
}

void Tty::write_line(std::string_view sv) noexcept {
  write(sv);
  write_char('\n');
}

void Tty::write_line(ctr::String& s) noexcept {
  write_line(std::string_view{s.data(), s.size()});
}

char Tty::read_char_blocking() noexcept {
  for (;;) {
    hal::KeyEvent ev{};
    char c = 0;

    if (!keyboard.poll(ev)) continue;
    if (ev.type == hal::KeyEventType::Release) continue;
    if (!input::key_event_to_char(ev, c)) continue;

    return c;
  }
}

void Tty::readline(ctr::String& out, std::string_view prompt) noexcept {
  out.clear();
  write(prompt);

  for (;;) {
    hal::KeyEvent ev{};
    char c = 0;

    if (!keyboard.poll(ev)) continue;
    if (ev.type == hal::KeyEventType::Release) continue;

    if (!input::key_event_to_char(ev, c)) {
      using hal::Key;
      switch (ev.key) {
        case Key::Backspace:
          if (!out.empty()) {
            out.pop_back();
            display.backspace();
          }
          break;
        case Key::Right:
          display.move_right(1);
          break;
        case Key::Left:
          if (display.cursor().x > prompt.size() && !raw_mode) { display.move_left(1); }
          break;
        case Key::Up:
          if (raw_mode) display.move_up(1);
          break;
        case Key::Down:
          if (raw_mode) display.move_down(1);
          break;
        case Key::PageUp:
          display.scroll_down(1);
          break;
        case Key::PageDown:
          display.scroll_up(1);
          break;
        case Key::End: {
          if (ev.has_mod(hal::KeyMod::Ctrl)) {
            display.move_end();
          } else {
            display.move_line_end();
          }
          break;
        }
        default:
          continue;
      }
      display.flush();
      continue;
    } else if (c == '\n' || c == '\r') {
      display.move_end();
      // display.move_line_end();
      display.put_char('\n');
      display.flush();
      return;
    }

    out.push_back(c);
    display.put_char(c);
    display.flush();
  }
}

}  // namespace tty
