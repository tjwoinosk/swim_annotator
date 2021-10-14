#define BOOST_TEST_MODULE Swim Tracker Tests
#include <boost/test/included/unit_test.hpp>
//Add --detect_memory_leak=0 to debug command to remove memory leak output
//--log_level=test_suite in command for more info https://www.boost.org/doc/libs/1_75_0/libs/test/doc/html/boost_test/utf_reference/rt_param_reference/log_level.html

#include "addAnotherFile.h"
#include "sort_tracker.h"

#include "frameAnalysis.h"
#include "sortTrackerPipelined.h"

#include "TrackingBox.h"
#include "fileFinder.h"

#include "swimmerDetector.h"

#include <fstream>
#include <iterator>


BOOST_AUTO_TEST_SUITE(FrameAnalysisSubFunctions)
BOOST_AUTO_TEST_CASE(testFrameAnalysisMain)
{
	string videoName = "0_01_sub_video.avi";
	frameAnalysis mainfuncTest;
	//mainfuncTest.analyzeVideo(videoName); //TODO make a proper test case from this. maybe function returns bool?
}

BOOST_AUTO_TEST_SUITE_END() //End blank Tests


//File FinderTests Tests
BOOST_AUTO_TEST_SUITE(fileFinderTests)
BOOST_AUTO_TEST_CASE(testOutStreamFunctions)
{
	fileFinder find;
	std::string absolutePath;

	absolutePath = find.absolutePath("PipeTest.txt");
	BOOST_CHECK(!absolutePath.empty());

	absolutePath = find.absolutePath("notAFile.txt");
	BOOST_CHECK(absolutePath.empty());

	absolutePath = find.returnDataLocation();
	BOOST_CHECK(!absolutePath.empty());

	absolutePath = find.returnSpeedTestLocation();
	BOOST_CHECK(!absolutePath.empty());
}
BOOST_AUTO_TEST_SUITE_END() //End file Finder Tests


//Tracking Box Tests
BOOST_AUTO_TEST_SUITE(BoxTests)
BOOST_AUTO_TEST_CASE(testTrackingBoxInStreamFunctionsNEW)
{
	cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
	TrackingBox testBox(10, 5, tBox);

	stringstream ssGround;
	ssGround << testBox.m_frame << "," << testBox.m_boxID << "," << testBox.x << "," << testBox.y << "," << testBox.width << "," << testBox.height << ",1,-1,-1,-1" << std::endl;

	stringstream ss;
	testBox.outputToFile(ss);

	BOOST_CHECK_EQUAL(ssGround.str(), ss.str());
}
BOOST_AUTO_TEST_CASE(testTrackingBoxInStreamFunctions)
{
	cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
	TrackingBox testBox(10, 5, tBox);

	stringstream ssGround;
	ssGround << testBox.m_frame << "," << testBox.m_boxID << "," << testBox.x << "," << testBox.y << "," << testBox.width << "," << testBox.height << ",1,-1,-1,-1" << std::endl;

	stringstream ss;
	ss << testBox;

	BOOST_CHECK_EQUAL(ssGround.str(), ss.str());
}
BOOST_AUTO_TEST_CASE(testTrackingBoxEqualOperator)
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
BOOST_AUTO_TEST_CASE(testTrackingBoxOutStreamFunctions)
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
BOOST_AUTO_TEST_CASE(testTrackingBoxOutStreamFunctionsNew)
{
	cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
	TrackingBox groundBox(10, 5, tBox);

	stringstream ssTester;
	ssTester << groundBox.m_frame << "," << groundBox.m_boxID << "," << groundBox.x << "," << groundBox.y << "," << groundBox.width << "," << groundBox.height << ",1,-1,-1,-1" << std::endl;

	TrackingBox testBox;

	//ssTester >> testBox;
	testBox.inputFromFile(ssTester);

	BOOST_CHECK(testBox == groundBox);
	BOOST_CHECK_GE(testBox.GetIOU(groundBox), 0.999);
}
BOOST_AUTO_TEST_CASE(testDetectionBoxInStreamFunctionsNEW)
{
	cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
	TrackingBox testBox(1, 0.78665f, 10, 5, tBox);

	stringstream ssGround;
	ssGround << testBox.m_frame << "," << testBox.m_boxID;
	ssGround << "," << testBox.x << "," << testBox.y << "," << testBox.width << "," << testBox.height;
	ssGround << "," << testBox.m_confScore << "," << testBox.m_swimmerClass << std::endl;

	stringstream ss;
	//ss << testBox;
	testBox.outputToFileDetection(ss);

	BOOST_CHECK_EQUAL(ssGround.str(), ss.str());
}
BOOST_AUTO_TEST_CASE(testDetectionBoxOutStreamFunctionsNew)
{
	cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
	TrackingBox groundBox(1, 0.78665f, 10, 5, tBox);

	std::stringstream ssTester;
	ssTester << groundBox.m_frame << "," << groundBox.m_boxID;
	ssTester << "," << groundBox.x << "," << groundBox.y << "," << groundBox.width << "," << groundBox.height;
	ssTester << "," << groundBox.m_confScore << groundBox.m_swimmerClass << std::endl;

	TrackingBox testBox;

	//ssTester >> testBox;
	testBox.inputFromFileDetection(ssTester);

	BOOST_CHECK_GE(testBox.GetIOU(groundBox), 0.999);
}
BOOST_AUTO_TEST_SUITE_END() //End Box Tests


//Detection validation test suite
///*
BOOST_AUTO_TEST_SUITE(DetectionTestSuite)
BOOST_AUTO_TEST_CASE(DetectionValidationTEST)
{
	frameAnalysis testDetector;
	std::string resAbsPath = "";

	resAbsPath = testDetector.runDetectorOnFrames();

	//results file
	std::string resAbsPathGT = resAbsPath;
	resAbsPathGT.replace(resAbsPathGT.end() - 4, resAbsPathGT.end(), "GT.txt");

	std::ifstream ifs1(resAbsPath);
	std::ifstream ifs2(resAbsPathGT);

	std::istream_iterator<char> b1(ifs1), e1;
	std::istream_iterator<char> b2(ifs2), e2;

	// compare 
	BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}
BOOST_AUTO_TEST_SUITE_END() //End Detection validation tests suite
//*/


//SORT validation test suite
BOOST_AUTO_TEST_SUITE(SORTvalidationTestSuite)
BOOST_AUTO_TEST_CASE(SORTvalidationTEST)
{
	frameAnalysis testSORTTWO;
	string gtPath = "";
	string outputName = "";

	outputName = testSORTTWO.sortOnFrame();
	gtPath = outputName;

	//results file
	gtPath.replace(gtPath.end() - 4, gtPath.end(), "GT.txt");

	std::ifstream ifs1(outputName);
	std::ifstream ifs2(gtPath);

	std::istream_iterator<char> b1(ifs1), e1;
	std::istream_iterator<char> b2(ifs2), e2;

	BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}
BOOST_AUTO_TEST_SUITE_END() //End SORT validation tests suite