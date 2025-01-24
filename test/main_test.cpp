#include "gtest/gtest.h"

/**
 * @brief Main function to initialize Google Test and run all tests in the project.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return Returns 0 if all tests pass, otherwise returns a non-zero value.
 */
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
