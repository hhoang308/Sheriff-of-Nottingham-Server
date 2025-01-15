cmake_minimum_required(VERSION 3.10)
project(SheriffOfNottinghamServer)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include
    include/model
    include/controller 
)

set(SOURCES
    src/model/Server.cpp
    src/controller/Game.cpp
    src/model/Player.cpp
    src/model/Card.cpp
)


add_executable(SheriffOfNottinghamServer 
    src/main.cpp
    ${SOURCES}
)

target_link_libraries(SheriffOfNottinghamServer PRIVATE pthread)

########For Unit Test########

find_package(Threads REQUIRED)

enable_testing()

add_executable(UnitTests 
    test/test_createAndSuffleDeck.cpp
    ${SOURCES}
)

add_subdirectory(googletest)

target_link_libraries(UnitTests gtest gtest_main Threads::Threads)

# Use GoogleTest's CTest integration
include(GoogleTest)
gtest_discover_tests(UnitTests)