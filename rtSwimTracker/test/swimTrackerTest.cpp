#define BOOST_TEST_MODULE Swim Tracker Tests
#include <boost/test/included/unit_test.hpp>
//Add --detect_memory_leak=0 to debug command to remove memory leak output

#include "addAnotherFile.h"
#include "sort_tracker.h"

#include "frameAnalysis.h"
#include "sortTrackerPipelined.h"

#include "TrackingBox.h"
#include "fileFinder.h"

#include "swimmerDetector.h"

#include <fstream>
#include <iterator>

BOOST_AUTO_TEST_SUITE(sortSubFunctions)

/*
BOOST_AUTO_TEST_CASE(testVectorized) 
{
  
	string videoName = "0_01_sub_video.avi";
	frameAnalysis mainfuncTest;
	mainfuncTest.analyzeVideo(videoName); //TODO make a proper test case from this. maybe function returns bool?
}
*/

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FrameAnalysisSubFunctions)

BOOST_AUTO_TEST_CASE(testFrameAnalysisMain)
{

}

BOOST_AUTO_TEST_SUITE_END() //End blank Tests


//Tracking Box Tests
BOOST_AUTO_TEST_SUITE(fileFinderTests)

BOOST_AUTO_TEST_CASE(testOutStreamFunctions)
{
  fileFinder find;
  std::string absolutePath;

  absolutePath = find.absolutePath("PipeTest.txt");
  BOOST_CHECK(!absolutePath.empty());

  absolutePath = find.absolutePath("notAFile.txt");
  BOOST_CHECK(absolutePath.empty());
}

BOOST_AUTO_TEST_SUITE_END() //End file Finder Tests


//Tracking Box Tests
BOOST_AUTO_TEST_SUITE(SORTtrackingBoxTests)
BOOST_AUTO_TEST_CASE(testInStreamFunctions)
{
  cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
  TrackingBox testBox(10,5,tBox);

  stringstream ssGround;
  ssGround << testBox.m_frame << "," << testBox.m_boxID << "," << testBox.x << "," << testBox.y << "," << testBox.width << "," << testBox.height << ",1,-1,-1,-1" << std::endl;

  stringstream ss;
  ss << testBox;

  BOOST_CHECK_EQUAL(ssGround.str(), ss.str());
}

BOOST_AUTO_TEST_CASE(testequalOperator)
{
  cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
  cv::Rect_<float> t2Box(4.3f, 3.4f, 6.98f, 9.00f);

  TrackingBox testBox(10, 5, tBox);
  TrackingBox test2Box(3, 7, t2Box);
  TrackingBox test3Box(10, 5, t2Box);
  TrackingBox test4Box(10, 5, tBox);

  BOOST_CHECK(testBox != test2Box);
  BOOST_CHECK(test2Box != test3Box);
  BOOST_CHECK(test3Box == test4Box);
  BOOST_CHECK(testBox == test4Box);
}

BOOST_AUTO_TEST_CASE(testOutStreamFunctions)
{
  cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
  TrackingBox groundBox(10, 5, tBox);

  stringstream ssTester;
  ssTester << groundBox.m_frame << "," << groundBox.m_boxID << "," << groundBox.x << "," << groundBox.y << "," << groundBox.width << "," << groundBox.height << ",1,-1,-1,-1" << std::endl;

  TrackingBox testBox;

  ssTester >> testBox;

  BOOST_CHECK(testBox == groundBox);
  BOOST_CHECK_GE(testBox.GetIOU(groundBox), 0.999);

}
BOOST_AUTO_TEST_SUITE_END() //End Tracking Box Tests

//Detection validation test suite
BOOST_AUTO_TEST_SUITE(SORTvalidationTestSuite)

BOOST_AUTO_TEST_CASE(DetectionValidationTEST)
{

  swimmerDetector test;



}

BOOST_AUTO_TEST_SUITE_END() //End Detection validation tests suite



//SORT validation test suite
BOOST_AUTO_TEST_SUITE(SORTvalidationTestSuite)

BOOST_AUTO_TEST_CASE(SORTvalidationTEST)
{
  frameAnalysis testSORTTWO;
  fileFinder find;
  string seqName = "PipeTest.txt";

  testSORTTWO.sortOnFrame(find.absolutePath(seqName));

  //results file
  string outputName = seqName;
  outputName.replace(outputName.end() - 4, outputName.end(), "_det.txt");

  //ground truth file
  string gtFile = "gt" + outputName;

  std::ifstream ifs1(find.absolutePath(outputName));
  std::ifstream ifs2(find.absolutePath(gtFile));

  std::istream_iterator<char> b1(ifs1), e1;
  std::istream_iterator<char> b2(ifs2), e2;

  // compare 
  BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);

}
BOOST_AUTO_TEST_SUITE_END() //End SORT validation tests suite