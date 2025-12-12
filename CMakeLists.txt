cmake_minimum_required(VERSION 3.16)
project(editOS LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(ASM)

set(ARCH_FAMILY "x86" CACHE STRING "Target architecture (e.g. x86, x86_64)")
set(ARCH_VARIANT "i386" CACHE STRING "")
set(TARGET_TRIPLET "i386-elf" CACHE STRING "")
set(QEMU_SYSTEM "qemu-system-i386" CACHE STRING "QEMU system binary")

# ----------------------------------------------------------------------
# Toolchain
# ----------------------------------------------------------------------
set(TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/cross-compiler/${TARGET_TRIPLET}-toolchain/bin)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-as)
set(CMAKE_C_COMPILER   ${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-g++)

# ----------------------------------------------------------------------
# Helpers / arch config
# ----------------------------------------------------------------------
include("${CMAKE_SOURCE_DIR}/cmake/module-util.cmake")
include("${CMAKE_SOURCE_DIR}/src/arch/arch.cmake")

# ----------------------------------------------------------------------
# Compile configurations
# ----------------------------------------------------------------------
add_library(kernel_compile_config INTERFACE)
add_library(module_compile_config INTERFACE)

# Kernel-side code: can see kernel + kernel/modules + libs + klibc
target_include_directories(kernel_compile_config INTERFACE
    "${CMAKE_SOURCE_DIR}/src/kernel"
    "${CMAKE_SOURCE_DIR}/src/kernel/modules"
    "${CMAKE_SOURCE_DIR}/src/kernel/modules/klibc/include"
    "${CMAKE_SOURCE_DIR}/src/libs"
)

# Support modules: can see libs + klibc, but not kernel/*
target_include_directories(module_compile_config INTERFACE
    "${CMAKE_SOURCE_DIR}/src/libs"
    "${CMAKE_SOURCE_DIR}/src/kernel/modules/klibc/include"
)

foreach(cfg IN ITEMS kernel_compile_config module_compile_config)
    target_compile_options(${cfg} INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:
            -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/src/=/editOS/
            -ffreestanding
            -fno-exceptions
            -fno-rtti
            -O2
            -Wall
            -Wextra
            -std=c++20
        >
        $<$<COMPILE_LANGUAGE:C>:
            -ffreestanding
            -fno-builtin
        >
        $<$<COMPILE_LANGUAGE:ASM>:
        >
    )
endforeach()

# ----------------------------------------------------------------------
# Kernel sources (core only – modules come via add_kernel_module/add_module)
# ----------------------------------------------------------------------
set(KERNEL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/kernel/kernel.cpp"

    "${CMAKE_SOURCE_DIR}/src/kernel/drv/global_core.cpp"

    "${CMAKE_SOURCE_DIR}/src/kernel/panic/panic_impl.cpp"

    "${CMAKE_SOURCE_DIR}/src/kernel/boot/multiboot2.cpp"

    "${CMAKE_SOURCE_DIR}/src/kernel/memory/builtin/bm_heap.cpp"
    "${CMAKE_SOURCE_DIR}/src/kernel/memory/builtin/bm_page_frame_allocator.cpp"
    "${CMAKE_SOURCE_DIR}/src/kernel/memory/global_hooks.cpp"
    "${CMAKE_SOURCE_DIR}/src/kernel/memory/heap.cpp"

    "${CMAKE_SOURCE_DIR}/src/kernel/logging/logging.cpp"

    "${CMAKE_SOURCE_DIR}/src/kernel/tty/tty.cpp"
    "${CMAKE_SOURCE_DIR}/src/kernel/shell/shell.cpp"
)

# ----------------------------------------------------------------------
# Kernel object library + final kernel ELF
# ----------------------------------------------------------------------
add_library(kernel_obj OBJECT ${KERNEL_SOURCES} ${ARCH_SOURCES})
target_link_libraries(kernel_obj PRIVATE kernel_compile_config)

if(DEFINED ARCH_INCLUDE_DIRS)
    target_include_directories(kernel_obj PRIVATE ${ARCH_INCLUDE_DIRS})
endif()

add_executable(kernel $<TARGET_OBJECTS:kernel_obj>)
set(KERNEL_TARGET kernel)

set_target_properties(kernel PROPERTIES
    OUTPUT_NAME "kernel.elf"
)

if(NOT DEFINED ARCH_LINKER_SCRIPT)
  message(FATAL_ERROR "ARCH_LINKER_SCRIPT not set by arch.cmake")
endif()
target_link_options(kernel PRIVATE
    -nostdlib
    -lgcc
    -z noexecstack
    -Wl,-T,${ARCH_LINKER_SCRIPT}
    -Wl,-N
)

# ----------------------------------------------------------------------
# Modules
# ----------------------------------------------------------------------
# Kernel feature modules (under src/kernel/modules/*)
add_kernel_module("${CMAKE_SOURCE_DIR}/src/kernel/modules/ui")
add_kernel_module("${CMAKE_SOURCE_DIR}/src/kernel/modules/gfx")
add_kernel_module("${CMAKE_SOURCE_DIR}/src/kernel/modules/input")
add_kernel_module("${CMAKE_SOURCE_DIR}/src/kernel/modules/klibc")

# Generic/support/runtime modules
add_module("${CMAKE_SOURCE_DIR}/src/abi")
add_module("${CMAKE_SOURCE_DIR}/src/libs/math")
add_module("${CMAKE_SOURCE_DIR}/src/libs/containers")
# add_module("${CMAKE_SOURCE_DIR}/src/apps/editor")

flush_pending_kernel_modules()

# ----------------------------------------------------------------------
# ISO creation
# ----------------------------------------------------------------------
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

add_custom_target(iso ALL DEPENDS ${ISO_FILE})

add_custom_target(run
    COMMAND ${QEMU_SYSTEM}
            -cdrom ${ISO_FILE}
            -chardev vc,id=serialgui,logfile=serial.log,logappend=on
            -serial chardev:serialgui
            -display gtk,zoom-to-fit=on
    DEPENDS iso
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

