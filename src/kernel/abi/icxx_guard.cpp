namespace __cxxabiv1 {
// This is not thread-safe as I don't need it be, yet.
// TODO: Make this thread-safe, after scheduler is implemented.

// 64-bit guard type as required by the Itanium C++ ABI.
__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard* g) {
  if (*reinterpret_cast<unsigned char*>(g) == 0) {
    return 1;
  }
  return 0;
}

extern "C" void __cxa_guard_release(__guard* g) {
  *reinterpret_cast<unsigned char*>(g) = 1;
}

extern "C" void __cxa_guard_abort(__guard* /*g*/) {}

}  // namespace __cxxabiv1
