set(MODULE_NAME "klibc")

set(MODULE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/src/stdlib.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/stdio.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/string.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/vsnprintf.cpp"
)

set(MODULE_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

