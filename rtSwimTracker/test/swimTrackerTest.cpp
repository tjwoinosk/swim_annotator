#define BOOST_TEST_MODULE Swim Tracker Tests
#include <boost/test/included/unit_test.hpp>
//Add --detect_memory_leak=0 to debug command to remove memory leak output

#include "addAnotherFile.h"
#include "sort_tracker.h"
#include "frameAnalysis.h"


#include <fstream>
#include <iterator>

BOOST_AUTO_TEST_SUITE(sortSubFunctions)

BOOST_AUTO_TEST_CASE(testVectorized) 
{
	string videoName = "0_01_sub_video.avi";
	frameAnalysis mainfuncTest;
	mainfuncTest.analyzeVideo(videoName); //TODO make a proper test case from this. maybe function returns bool?
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FrameAnalysisSubFunctions)

BOOST_AUTO_TEST_CASE(testFrameAnalysisMain)
{

}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_CASE(SORTvalidationTEST)
{
  sort_tracker testSORT;
  string seqName = "pipeTest.txt";

  //testSORT.sortTracker(seqName, 0.05);
  //testSORT.sortTrackerUsingFunctions(seqName, 0.05);
  //testSORT.sortWithFunctionsTest(seqName, 0.05);
  testSORT.sortOnFrame(seqName, 0.05);


  //results file
  string outputName = seqName;
  outputName.replace(outputName.end() - 4, outputName.end(), "_det.txt");

  //ground truth file
  string gtFile = "gt" + outputName;

  std::ifstream ifs1(outputName);
  std::ifstream ifs2(gtFile);

  std::istream_iterator<char> b1(ifs1), e1;
  std::istream_iterator<char> b2(ifs2), e2;

  // compare 
  BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);

}