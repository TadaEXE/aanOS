cmake_minimum_required(VERSION 3.16)
set( CMAKE_VERBOSE_MAKEFILE on )
project(editOS LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

enable_language(ASM)

set(CMAKE_ASM_COMPILER  x86_64-elf-as)
set(CMAKE_C_COMPILER   x86_64-elf-gcc)
set(CMAKE_CXX_COMPILER x86_64-elf-g++)

set(SYSROOT
  "${CMAKE_SOURCE_DIR}/sysroot"
  CACHE PATH "Sysroot used for cross builds"
)

set(ARCH "x86" CACHE STRING "Target architecture (e.g. x86, x86_64)")

set(LIB_C_SOURCES
    ${CMAKE_SOURCE_DIR}/src/libc/src/abort.cpp
    ${CMAKE_SOURCE_DIR}/src/libc/src/stdio.cpp
    ${CMAKE_SOURCE_DIR}/src/libc/src/string.cpp
)

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
    ${CMAKE_SOURCE_DIR}/src/kernel/logging/logging.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/tty/tty.cpp
    ${CMAKE_SOURCE_DIR}/src/kernel/shell/shell.cpp

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

add_library(kernel_obj OBJECT ${KERNEL_SOURCES} ${ARCH_SOURCES})

add_library(libc_obj STATIC ${LIB_C_SOURCES})
target_include_directories(libc_obj PUBLIC
  ${CMAKE_SOURCE_DIR}/src/libc/include
  ${CMAKE_SOURCE_DIR}/src
)
target_compile_options(libc_obj PRIVATE
  -ffreestanding
  -fno-builtin
)

target_include_directories(kernel_obj PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/libc/include
)

target_compile_options(kernel_obj PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:
        --sysroot=${SYSROOT}
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/src/=/editOS/
        -ffreestanding
        -fno-exceptions
        -fno-rtti
        # -nostdinc++
        -O2
        -Wall
        -Wextra
        -std=c++20
    >
    $<$<COMPILE_LANGUAGE:ASM>:
    >
)

# Actual kernel binary: link the object library
add_executable(kernel $<TARGET_OBJECTS:kernel_obj>)

set_target_properties(kernel PROPERTIES
    OUTPUT_NAME "kernel.elf"
)

target_link_libraries(kernel PRIVATE libc_obj)

# Link options: 32-bit, no stdlib, custom linker script
target_link_options(kernel PRIVATE
    --sysroot=${SYSROOT}
    -nostdlib
    -lgcc
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

add_custom_target(analyze
    COMMAND elf-size-analyze -H -R -o -m 10 $<TARGET_FILE:kernel>
    COMMAND elf-size-analyze -H -R -W $<TARGET_FILE:kernel> > ram_report.html
    COMMAND elf-size-analyze -H -F -W $<TARGET_FILE:kernel> > rom_report.html
    DEPENDS kernel
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
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

set(CMAKE_INSTALL_PREFIX "${SYSROOT}" CACHE PATH "Install prefix" FORCE)

install(TARGETS libc_obj ARCHIVE
  DESTINATION ${SYSROOT}/usr/lib
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/libc/include/
  DESTINATION ${SYSROOT}/usr/include
)

