cmake_minimum_required(VERSION 3.10)
project(SheriffOfNottinghamServer)

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIRS
    include
    include/model
    include/controller
    include/utils
)

set(SERVER_SOURCES
    src/model/Server.cpp
    src/controller/Game.cpp
    src/model/Player.cpp
    src/model/Card.cpp
    src/utils/Utils.cpp
)

file(GLOB TEST_SOURCES test/*.cpp)

find_package(Threads REQUIRED)

set(LIBS pthread jsoncpp)

add_executable(SheriffOfNottinghamServer
    src/main.cpp
    ${SERVER_SOURCES}
)
target_include_directories(SheriffOfNottinghamServer PRIVATE ${INCLUDE_DIRS})
target_link_libraries(SheriffOfNottinghamServer PRIVATE ${LIBS})

enable_testing()

add_executable(UnitTests
    test/main_test.cpp
    ${SERVER_SOURCES}
    ${TEST_SOURCES}
)

add_subdirectory(googletest)

target_include_directories(UnitTests PRIVATE ${INCLUDE_DIRS})
target_link_libraries(UnitTests PRIVATE gtest gtest_main Threads::Threads ${LIBS})

target_compile_options(UnitTests PRIVATE -Wall -Wextra -g)

include(GoogleTest)
gtest_discover_tests(UnitTests TEST_PREFIX "SheriffOfNottingham.")