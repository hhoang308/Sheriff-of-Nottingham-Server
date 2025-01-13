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

########For Unit Test########

find_package(Threads REQUIRED)

enable_testing()

add_executable(UnitTests 
    test/test_createAndSuffleDeck.cpp
    ${SOURCES}
)

add_subdirectory(googletest)

# TODO: is it necessary to link these library?
target_link_libraries(SheriffOfNottinghamServer gtest gtest_main Threads::Threads)

target_link_libraries(UnitTests gtest gtest_main Threads::Threads)

add_test(NAME UnitTests COMMAND UnitTests)