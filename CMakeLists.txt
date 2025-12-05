cmake_minimum_required(VERSION 3.16)
project(editOS LANGUAGES CXX ASM)

# Freestanding 32-bit C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(ASM)

set(ARCH "x86" CACHE STRING "Target architecture (e.g. x86, x86_64)")

set(ARCH_SOURCES
    ${CMAKE_SOURCE_DIR}/src/arch/${ARCH}/boot/boot.s
    ${CMAKE_SOURCE_DIR}/src/arch/${ARCH}/boot/entry_point.cpp

    ${CMAKE_SOURCE_DIR}/src/arch/${ARCH}/system/system.cpp

    ${CMAKE_SOURCE_DIR}/src/arch/${ARCH}/graphics/framebuffer.cpp

    ${CMAKE_SOURCE_DIR}/src/arch/${ARCH}/input/keyboard.cpp

    ${CMAKE_SOURCE_DIR}/src/arch/${ARCH}/io/serial.cpp
)

set(KERNEL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/kernel/main.cpp

    ${CMAKE_SOURCE_DIR}/src/kernel/memory/builtin/bm_heap.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/memory/global_hooks.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/memory/heap.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/abi/icxx/atexit.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/abi/icxx/guard.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/abi/icxx/pure_virtual.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/abi/gcc/int_arithmetic.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/boot/multiboot2.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/runtime/cstring.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/logging/logging.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/tty/tty.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/shell/shell.cpp

    # ${CMAKE_SOURCE_DIR}/src/apps/editor/editor.cpp

    ${CMAKE_SOURCE_DIR}/src/gfx/canvas.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/shapes.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/text/text.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/text/bitmap_font.cpp

    ${CMAKE_SOURCE_DIR}/src/ui/core/window.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/core/text_area.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/tty/tty_text_area.cpp

    ${CMAKE_SOURCE_DIR}/src/hal/keyboard.cpp

    ${CMAKE_SOURCE_DIR}/src/input/keymap_us.cpp

    ${CMAKE_SOURCE_DIR}/src/math/bit_logic.cpp

)
# Object library that builds all kernel sources
add_library(kernel_obj OBJECT ${KERNEL_SOURCES} ${ARCH_SOURCES})

# Allow #include "hw/console.hpp" etc.
target_include_directories(kernel_obj PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Per-language compile options (so we don't feed C++ flags to the assembler)
target_compile_options(kernel_obj PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/src/=/editOS/
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
        -std=c++20
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

add_custom_target(run
    COMMAND qemu-system-i386
            -cdrom ${ISO_FILE}
            -chardev vc,id=serialgui,logfile=serial.log,logappend=on
            -serial chardev:serialgui
            -display gtk,zoom-to-fit=on
    DEPENDS iso
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

