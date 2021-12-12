cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

enable_testing()

add_executable(test_roka)

get_target_property(ROKA_HEADERS roka INTERFACE_SOURCES)

set(TEST_ROKA_SOURCES
    test_roka.cpp
)

target_sources(test_roka PRIVATE
    ${ROKA_HEADERS}
    ${TEST_ROKA_SOURCES}
)

source_group("Roka Headers" FILES ${ROKA_HEADERS})

target_compile_features(test_roka PRIVATE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_roka PRIVATE /MP /W4)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(test_roka PRIVATE
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
    )
    target_compile_options(test_roka PRIVATE
        -Wall -Wextra -pedantic-errors -Werror=pedantic
        $<$<CONFIG:Debug>:-O0 -g3>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g3>
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_definitions(test_roka PRIVATE
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
    )
    target_compile_options(test_roka PRIVATE
        -Wall -Wextra -pedantic-errors -Werror=pedantic
        $<$<CONFIG:Debug>:-O0 -g3>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g3>
    )
endif()

target_link_libraries(test_roka PRIVATE roka)

add_test(
    NAME test_roka
    COMMAND $<TARGET_FILE:test_roka>
)
