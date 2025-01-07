cmake_minimum_required(VERSION 3.10)
project(SheriffOfNottinghamServer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Find ENet
# find_library(ENET_LIBRARY enet)
# include_directories(/usr/local/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    src/main.cpp
    src/Server.cpp
    src/Game.cpp
    src/Player.cpp
    src/Card.cpp
)

# Add executable
add_executable(SheriffOfNottinghamServer ${SOURCES})

# Link ENet library
# target_link_libraries(SheriffOfNottinghamServer ${ENET_LIBRARY})