cmake_minimum_required(VERSION 3.10) # Set minimum CMake version
project(MyCMakeproject VERSION 0.1.0 LANGUAGES C CXX)               # Name of your project

include(CTest)
enable_testing()

# Set C++ standard (recommended for Windows/MSVC)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# ThreadSanitizer
# ---------------------------------------------------------
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")

# ---------------------------------------------------------
# FetchContent
# ---------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

# Avoid MSVC runtime override
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------
# add_executable
# ---------------------------------------------------------
add_executable(eVTOL_Simulation
    main.cpp
    sources/Aircraft.cpp
    sources/Charger.cpp    
    # add any other .cpp files here
)

target_include_directories(eVTOL_Simulation
    PRIVATE
        ${CMAKE_SOURCE_DIR}/includes
        ${CMAKE_SOURCE_DIR}/sources
)

# ---------------------------------------------------------
# add_executable test
# ---------------------------------------------------------
add_executable(eVTOL_Simulation_Tests
    tests/TestAircraft.cpp
    tests/TestCharger.cpp
    tests/UnitTestsMain.cpp
    sources/Aircraft.cpp
    sources/Charger.cpp
)

target_include_directories(eVTOL_Simulation_Tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/includes
        ${CMAKE_SOURCE_DIR}/sources    
        ${CMAKE_SOURCE_DIR}/tests
)

target_link_libraries(eVTOL_Simulation_Tests
    gtest_main
    gtest
)