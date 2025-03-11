#!/bin/bash

BUILD_DIR="build"
BUILD_TYPE="Debug"
BUILD_SERVER="ON"
BUILD_TESTS="OFF"
RUN_AFTER_BUILD="OFF"

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --debug) BUILD_TYPE="Debug" ;;
        --release) BUILD_TYPE="Release" ;;
        --server) BUILD_SERVER=ON; BUILD_TESTS=OFF ;;
        --tests) BUILD_SERVER=OFF; BUILD_TESTS=ON ;;
        --all) BUILD_SERVER=ON; BUILD_TESTS=ON ;;
        --clean) rm -rf "$BUILD_DIR"; echo "Cleaned build directory."; exit 0 ;;
        --run) RUN_AFTER_BUILD=ON ;;  # Thêm tùy chọn chạy chương trình sau khi build
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
    shift
done

echo "Build Type: $BUILD_TYPE"
echo "Build Server: $BUILD_SERVER"
echo "Build Tests: $BUILD_TESTS"
echo "Run After Build: $RUN_AFTER_BUILD"

mkdir -p "$BUILD_DIR" || { echo "Failed to create build directory"; exit 1; }

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DBUILD_SERVER=$BUILD_SERVER -DBUILD_TESTS=$BUILD_TESTS

if cmake --build "$BUILD_DIR"; then
    echo "Build successful!"
    if [[ "$RUN_AFTER_BUILD" == "ON" && "$BUILD_SERVER" == "ON" ]]; then
        EXECUTABLE="$BUILD_DIR/SheriffOfNottinghamServer"
        if [[ -f "$EXECUTABLE" ]]; then
            echo "Running SheriffOfNottinghamServer..."
            cd "$BUILD_DIR" && ./SheriffOfNottinghamServer
        else
            echo "Error: Executable $EXECUTABLE not found!"
            exit 1
        fi
    fi
    # if [[ "$BUILD_TESTS" == "ON" ]]; then
    #     EXECUTABLE="$BUILD_DIR/UnitTests"
    #     if [[ -f "$EXECUTABLE" ]]; then
    #         echo "Running Unit Test..."
    #         cd "$BUILD_DIR" && ./UnitTests
    #     else
    #         echo "Error: Executable $EXECUTABLE not found!"
    #         exit 1
    #     fi
    # fi
else
    echo "Build failed!"
    exit 1
fi