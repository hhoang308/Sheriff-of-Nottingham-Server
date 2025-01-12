## Precondition
### Install dependencies
```
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config
sudo apt install libgtest-dev
```
## How to build the project and Unit Test
```
# Clone the project
git clone https://github.com/your-repo/SheriffOfNottinghamServer.git
cd SheriffOfNottinghamServer
# Clone googletest
git clone https://github.com/google/googletest.git
# Configure the project
mkdir build
cd build
# Configure and build project
cmake ..
make
# Build both the project and unit tests
cmake --build .

# Run the server
./SheriffOfNottinghamServer

# Execute Unit Test
./UnitTests
```

