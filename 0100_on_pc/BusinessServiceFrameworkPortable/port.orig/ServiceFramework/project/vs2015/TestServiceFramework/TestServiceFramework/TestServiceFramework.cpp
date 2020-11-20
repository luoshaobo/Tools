#include <gtest/gtest.h>
#include "tut/tinyunittest.hpp"

int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);

  return Ntut::mainImpl(__FILE__, __LINE__, "Test", "OPTNATIVE: +d.");
}