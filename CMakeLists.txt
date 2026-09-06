cmake_minimum_required(VERSION 3.20)

project(NeutronCascade LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

add_executable(
    neutron_visualizer
    src/main.cpp
    src/simulation.cpp
    src/simulation_logger.cpp

)

target_include_directories(
    neutron_visualizer
    PRIVATE include
)

target_link_libraries(
    neutron_visualizer
    PRIVATE raylib
)

if(MSVC)
    target_compile_options(
        neutron_visualizer
        PRIVATE
        /W4
        /permissive-
    )
else()
    target_compile_options(
        neutron_visualizer
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()


enable_testing()

add_executable(
    test_reproducibility
    tests/test_reproducibility.cpp
    src/simulation.cpp
)

target_include_directories(
    test_reproducibility
    PRIVATE include
)

add_test(
    NAME reproducibility
    COMMAND test_reproducibility
)

add_executable(
    test_population_cap
    tests/test_population_cap.cpp
    src/simulation.cpp
)

target_include_directories(
    test_population_cap
    PRIVATE include
)

add_test(
    NAME population_cap
    COMMAND test_population_cap
)

add_executable(
    test_boundary_escape
    tests/test_boundary_escape.cpp
    src/simulation.cpp
)

target_include_directories(
    test_boundary_escape
    PRIVATE include
)

add_test(
    NAME boundary_escape
    COMMAND test_boundary_escape
)

add_executable(
    test_reset_behavior
    tests/test_reset_behavior.cpp
    src/simulation.cpp
)

target_include_directories(
    test_reset_behavior
    PRIVATE include
)

add_test(
    NAME reset_behavior
    COMMAND test_reset_behavior
)


add_executable(
    test_no_collision_population_constant
    tests/test_no_collision_population_constant.cpp
    src/simulation.cpp
)

target_include_directories(
    test_no_collision_population_constant
    PRIVATE include
)

add_test(
    NAME no_collision_population_constant
    COMMAND test_no_collision_population_constant
)

add_executable(
    benchmark_simulation
    benchmarks/benchmark_simulation.cpp
    src/simulation.cpp
)

target_include_directories(
    benchmark_simulation
    PRIVATE include
)

if(MSVC)
    target_compile_options(
        benchmark_simulation
        PRIVATE
        /W4
        /permissive-
    )
else()
    target_compile_options(
        benchmark_simulation
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
