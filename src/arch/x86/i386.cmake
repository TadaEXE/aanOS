set(X86_I386_DIR "${CMAKE_CURRENT_LIST_DIR}/i386")

list(APPEND ARCH_SOURCES
    ${X86_I386_DIR}/boot/boot.s
    ${X86_I386_DIR}/boot/entry_point.cpp
    ${X86_I386_DIR}/memory/paging.cpp
    ${X86_I386_DIR}/system/system.cpp
)

set(ARCH_LINKER_SCRIPT
    "${CMAKE_CURRENT_LIST_DIR}/i386/linker.ld"
)
