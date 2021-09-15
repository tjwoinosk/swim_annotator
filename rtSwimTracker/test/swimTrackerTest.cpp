#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "addAnotherFile.h"


BOOST_AUTO_TEST_CASE(myTestCase)
{
  testFunc();
  BOOST_CHECK(1 == 1);
  BOOST_CHECK(1 == 2);
  BOOST_CHECK(true);
}