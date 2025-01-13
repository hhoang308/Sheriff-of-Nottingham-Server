cmake_minimum_required(VERSION 3.10)
project(SheriffOfNottinghamServer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include project source directories
include_directories(
    include
    include/model
    include/controller 
)

# Add source files for the main project
set(SOURCES
    src/main.cpp
    src/model/Server.cpp
    src/controller/Game.cpp
    src/model/Player.cpp
    src/model/Card.cpp
)

# # Link the main executable with pthread and GoogleTest libraries
# target_link_libraries(SheriffOfNottinghamServer PRIVATE GTest::GTest GTest::Main Threads::Threads)

# Create the main executable (SheriffOfNottinghamServer)
add_executable(SheriffOfNottinghamServer ${SOURCES})

# Link ENet library
# target_link_libraries(SheriffOfNottinghamServer ${ENET_LIBRARY})