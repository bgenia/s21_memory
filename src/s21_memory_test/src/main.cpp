#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}
