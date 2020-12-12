cmake_minimum_required(VERSION 3.14)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(test_roka)
enable_testing()

set(ROKA_HEADERS
    ../include/roka/roka.hpp
)

set(TEST_ROKA_SOURCES
    test_roka.cpp
)

add_executable(test_roka
    ${ROKA_HEADERS}
    ${TEST_ROKA_SOURCES}
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group("Roka Headers" FILES ${ROKA_HEADERS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_roka PRIVATE /MP /W4)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(test_roka PRIVATE
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
    )
    target_compile_options(test_roka PRIVATE -Wall -Wextra -pedantic-errors -Werror=pedantic)
    target_compile_options(test_roka PRIVATE
        $<$<CONFIG:Debug>:-O0 -g3>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g3>
    )
endif()

include_directories(../include)

add_test(
    NAME test_roka
    COMMAND $<TARGET_FILE:test_roka>
)
