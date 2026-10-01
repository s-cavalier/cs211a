cmake_minimum_required(VERSION 3.23)

project(cs211a LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(POET_BUILD_TESTS OFF CACHE BOOL "Build POET tests")
FetchContent_Declare(poet
    GIT_REPOSITORY https://github.com/flatironinstitute/poet.git
    GIT_TAG 19185a17c49bde771880840214fe86844cec2d01
    UPDATE_DISCONNECTED TRUE
)
FetchContent_MakeAvailable(poet)

add_library(cs211a INTERFACE)
target_compile_features(cs211a INTERFACE cxx_std_23)
target_include_directories(cs211a INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cs211a INTERFACE poet::poet)
target_sources(cs211a INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
    FILES src/Matrix.hpp src/Rational.hpp
)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE cs211a)
