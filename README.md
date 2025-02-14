## Precondition
### Install dependencies
```
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config libgtest-dev
```
## Commit message rule
```
<action> <subject> <optional extra information>
```
## How to build the project
```
# Clone the project
git clone https://github.com/hhoang308/Sheriff-of-Nottingham-Server.git
cd SheriffOfNottinghamServer
# Install jsoncpp (in Ubuntu)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install jsoncpp
# [Optional] Clone googletest for unitest
git clone https://github.com/google/googletest.git
# Build the project
./build.sh
Options:
default is --debug --server
--debug : Build Sheriff of Nottingham with full logs
--release : Build Sheriff of Nottingham without debug logs
--run : Build Sheriff of Nottingham and run it (if it success)
--server : Build Sheriff of Nottingham
--tests : Build unit tests of Sheriff of Nottingham
--all : Build unit tests and Sheriff of Nottingham
--clean : Clean build directory
# Run the server
./SheriffOfNottinghamServer
# [Optional] Build both the project and unit tests
cmake --build
# [Optional] Execute unit tests
./UnitTests
or
ctest
```