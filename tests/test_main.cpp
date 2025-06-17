/**
 * @file test_main.cpp
 * @brief Basic test runner for Octa-Core C++ library
 * 
 * This file provides the main entry point for Google Test.
 * Individual test files will be added in Phase P1.
 * 
 * Phase P0: Basic test infrastructure
 */

#include <gtest/gtest.h>

/**
 * @brief Basic placeholder test to verify test infrastructure works
 */
TEST(PhaseP0Tests, TestInfrastructure) {
    // Basic test to verify Google Test is working
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
}

/**
 * @brief Test runner main function
 * 
 * This function initializes Google Test and runs all tests.
 * Individual component tests will be added in subsequent phases.
 */
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 