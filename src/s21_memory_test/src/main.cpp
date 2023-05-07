#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std::string_literals;
using namespace testing;

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  InitGoogleMock(&argc, argv);
}
