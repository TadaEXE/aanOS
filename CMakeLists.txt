cmake_minimum_required(VERSION 3.16)
project(editOS LANGUAGES CXX ASM)

# Freestanding 32-bit C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(ASM)

# All kernel sources (add more here as you create them)
set(KERNEL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/boot.s
    ${CMAKE_SOURCE_DIR}/src/kernel/main.cpp
    ${CMAKE_SOURCE_DIR}/src/hw/console/text_mode_console.cpp
    ${CMAKE_SOURCE_DIR}/src/hw/peripherals/keyboard.cpp
    ${CMAKE_SOURCE_DIR}/src/input/editor.cpp
    ${CMAKE_SOURCE_DIR}/src/memory/heap.cpp
    ${CMAKE_SOURCE_DIR}/src/hw/boot/multiboot2.cpp
    ${CMAKE_SOURCE_DIR}/src/hw/graphics/framebuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/hw/io/serial.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/canvas.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/shapes.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/window.cpp
    ${CMAKE_SOURCE_DIR}/src/logging/serial_logging.cpp
    ${CMAKE_SOURCE_DIR}/src/abi/icxx_guard.cpp
    ${CMAKE_SOURCE_DIR}/src/abi/icxx_pure_virtual.cpp
)

# Object library that builds all kernel sources
add_library(kernel_obj OBJECT ${KERNEL_SOURCES})

# Allow #include "hw/console.hpp" etc.
target_include_directories(kernel_obj PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Per-language compile options (so we don't feed C++ flags to the assembler)
target_compile_options(kernel_obj PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:
        -m32
        -ffreestanding
        # -fno-threadsafe-statics
        -fno-exceptions
        -fno-rtti
        -fno-pic
        -fno-pie
        -O2
        -Wall
        -Wextra
    >
    $<$<COMPILE_LANGUAGE:ASM>:
        -m32
    >
)

# Actual kernel binary: link the object library
add_executable(kernel $<TARGET_OBJECTS:kernel_obj>)

# We want the output file to be named kernel.elf
set_target_properties(kernel PROPERTIES
    OUTPUT_NAME "kernel.elf"
)

# Link options: 32-bit, no stdlib, custom linker script
target_link_options(kernel PRIVATE
    -m32
    -nostdlib
    -no-pie
    -Wl,-T,${CMAKE_SOURCE_DIR}/linker.ld
    -Wl,-N
)

# ISO creation
set(ISO_FILE ${CMAKE_BINARY_DIR}/editOS.iso)

add_custom_command(
    OUTPUT ${ISO_FILE}
    COMMAND ${CMAKE_COMMAND} -E rm -rf iso
    COMMAND ${CMAKE_COMMAND} -E make_directory iso/boot/grub
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel> iso/boot/kernel.elf
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/grub.cfg iso/boot/grub/grub.cfg
    COMMAND grub-mkrescue -o editOS.iso iso
    DEPENDS kernel ${CMAKE_SOURCE_DIR}/grub.cfg
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
)

# Default target: build the ISO
add_custom_target(iso ALL DEPENDS ${ISO_FILE})

# Optional: build just the kernel ELF with `cmake --build . --target kernel`

