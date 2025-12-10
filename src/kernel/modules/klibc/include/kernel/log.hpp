#pragma once
#include <cstdarg>

/// Log a C-style formatted message, that will always put a newline.
/// Cancel the automatic new line by putting an '\\' at the end of the fmt.
/// Currently the following formats are supported:
///   %c - char
///   %s - cstr
///   %d - int
///   %i - int
///   %u - uint
///   %x - hex
///   %p - ptr
///   %o - loggable obj
///   TODO: Rework these to variadic temptlates
void log_msg(const char* fmt, ...);
void log_msg(const char* fmt, va_list args);
