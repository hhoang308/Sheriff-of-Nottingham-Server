cmake_minimum_required(VERSION 3.10)
project(SheriffOfNottinghamServer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Enable testing
enable_testing()

# Find GoogleTest
# find_package(GTest REQUIRED)
# include_directories(${GTEST_INCLUDE_DIRS})

# Include directories
include_directories(include)

# Find Threads
find_package(Threads REQUIRED)

# Find ENet
# find_library(ENET_LIBRARY enet)
# include_directories(/usr/local/include)
# include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    test/test_createAndSuffleDeck.cpp
    src/main.cpp
    src/Server.cpp
    src/Game.cpp
    src/Player.cpp
    src/Card.cpp
)

# Add executable
add_executable(SheriffOfNottinghamServer ${SOURCES})

# Link GoogleTest and pthread to your test executable
add_subdirectory(googletest)
target_link_libraries(SheriffOfNottinghamServer gtest gtest_main Threads::Threads)

# Add unit tests
add_executable(UnitTests
    test/test_createAndSuffleDeck.cpp
    src/Server.cpp
    src/Game.cpp
    src/Player.cpp
    src/Card.cpp
    src/main.cpp 
)

# Link GoogleTest and pthread to the unit test executable
target_link_libraries(UnitTests gtest gtest_main Threads::Threads)

# Link ENet library
# target_link_libraries(SheriffOfNottinghamServer ${ENET_LIBRARY})

add_test(NAME UnitTests COMMAND UnitTests)