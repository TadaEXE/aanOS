set(X86_ROOT "${CMAKE_CURRENT_LIST_DIR}")
set(X86_COMMON_DIR "${X86_ROOT}/common")

list(APPEND ARCH_INCLUDE_DIRS
    ${X86_COMMON_DIR}
)

list(APPEND ARCH_SOURCES
    ${X86_COMMON_DIR}/graphics/framebuffer.cpp
    ${X86_COMMON_DIR}/input/keyboard.cpp
    ${X86_COMMON_DIR}/io/serial.cpp
)

if(ARCH_VARIANT STREQUAL "i386")
  include("${X86_ROOT}/i386.cmake")
elseif(ARCH_VARIANT STREQUAL "x86_64")
  include("${X86_ROOT}/x86_64.cmake")
else()
  message(FATAL_ERROR "Unsupported ARCH_VARIANT='${ARCH_VARIANT}' for x86")
endif()

