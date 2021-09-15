#define BOOST_TEST_MODULE Swim Tracker Tests
#include <boost/test/included/unit_test.hpp>


#include "addAnotherFile.h"

//Add --detect_memory_leak=0 to debug command to remove memory leak output

/*
BOOST_AUTO_TEST_SUITE(sortSubFunctions)

BOOST_AUTO_TEST_CASE(subFunc1) 
{

}

BOOST_AUTO_TEST_SUITE_END()
*/


BOOST_AUTO_TEST_CASE(SORTvalidationTEST)
{

  //run sortTracker(string seqName, double iou)

  // 1) open produced file
  // 2) open gt file

  // compare 
  BOOST_TEST(true/* some way to compare files*/);

}