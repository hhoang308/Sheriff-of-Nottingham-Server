cmake_minimum_required(VERSION 3.10)
project(SheriffOfNottinghamServer)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SERVER "Build the server executable" ON)
option(BUILD_TESTS "Build unit tests" OFF)

set(INCLUDE_DIRS
    include
    include/model
    include/state
    include/utils
    include/interface
    include/state
    external
)

set(SERVER_SOURCES
    src/model/Server.cpp
    src/model/Game.cpp
    src/model/Player.cpp
    src/model/Card.cpp
    src/utils/Utils.cpp
    src/utils/Log.cpp
    src/state/GameEndedState.cpp
    src/state/GameStartedState.cpp
    src/state/MerchantTurnState.cpp
    src/state/RoundStartedState.cpp
    src/state/SheriffTurnState.cpp
    src/state/WaitingForPlayersState.cpp
    external/json/jsoncpp.cpp
)

file(GLOB TEST_SOURCES test/*.cpp)

find_package(Threads REQUIRED)

set(LIBS pthread)

if(BUILD_SERVER)
    add_executable(SheriffOfNottinghamServer
        src/main.cpp
        ${SERVER_SOURCES}
    )
    target_include_directories(SheriffOfNottinghamServer PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(SheriffOfNottinghamServer PRIVATE ${LIBS})
    target_compile_options(SheriffOfNottinghamServer PRIVATE -Wall -Wextra )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(SheriffOfNottinghamServer PRIVATE ENABLE_DEBUG)
    endif()

endif()

if(BUILD_TESTS)
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
    target_compile_definitions(UnitTests PRIVATE ENABLE_DEBUG)

    include(GoogleTest)
    gtest_discover_tests(UnitTests TEST_PREFIX "SoN.")
endif()