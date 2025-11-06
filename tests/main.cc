#include "pch.hpp"

unsigned int test_counter = 0;

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
