#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <string>

#include <tsh.h>

using namespace std;


// test quit
TEST(ShellTest, Quit) {
  Process p((char *)"quit", 0, 0);
  p.split_string();

  EXPECT_TRUE(isQuit(&p)) << "passing quit should return true" << endl;
}

// test exit
TEST(ShellTest, NotQuit) {
  Process p((char *)"exit", 0, 0);
  p.split_string();

  EXPECT_FALSE(isQuit(&p)) << "passing quit should return true" << endl;
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
