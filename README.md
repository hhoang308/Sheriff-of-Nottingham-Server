## Precondition
### Install dependencies
```
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config libgtest-dev
```
## How to build the project
```
# Clone the project
git clone https://github.com/hhoang308/Sheriff-of-Nottingham-Server.git
cd SheriffOfNottinghamServer
# [Optional] Clone googletest for unitest
git clone https://github.com/google/googletest.git
# Configure the project
mkdir build
cd build
# Build the project
cmake ..
make
# Run the server
./SheriffOfNottinghamServer
# [Optional] Build both the project and unit tests
cmake --build
# [Optional] Execute unit tests
./UnitTests
```