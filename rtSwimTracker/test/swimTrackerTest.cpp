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

#include "postProcessRealTimeTracking.h"

#include "objectCentering.h"

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

//File SwimmerDetector Tests
BOOST_AUTO_TEST_SUITE(SwimmerDetectorTests)
BOOST_AUTO_TEST_CASE(testPrivateVariables) 
{
	float valPass = 0.5f;
	float valFail = 1.1f;

	swimmerDetector testDetector;

	bool confPass = testDetector.setConfThreshold(valPass);
	bool confFail = testDetector.setConfThreshold(valFail);
	float resultConf = testDetector.getConfThreshold();

	bool nmsPass = testDetector.setNmsThreshold(valPass);
	bool nmsFail = testDetector.setNmsThreshold(valFail);
	float resultnms = testDetector.getNmsThreshold();

	BOOST_CHECK_EQUAL(resultConf, valPass);
	BOOST_CHECK_EQUAL(confPass, true);
	BOOST_CHECK_EQUAL(confFail, false);

	BOOST_CHECK_EQUAL(resultnms, valPass);
	BOOST_CHECK_EQUAL(nmsPass, true);
	BOOST_CHECK_EQUAL(nmsFail, false);
}
BOOST_AUTO_TEST_SUITE_END() //End SwimmerDetector Tests

//Tracking Box Tests
BOOST_AUTO_TEST_SUITE(BoxTests)
BOOST_AUTO_TEST_CASE(testTrackingBoxPrivateVariables) 
{
	int testFrameID = 10;
	int testFrameIDTWO = 20;
	int testFrameIDFail = -10;
	int testBoxID = 5;
	int testBoxIDTWO = 20;
	int testBoxIDFail = -2;
	int testClassID = 5;
	int testClassIDFail = 6;
	float testConf = 0.5f;
	float testConfFail = 1.1f;
	cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
	TrackingBox testBox(testFrameID, testBoxID, tBox);

	int m_frame = testBox.get_m_frame();
	bool setFrameTrue = testBox.set_m_frame(testFrameIDTWO);
	bool setFrameFail = testBox.set_m_frame(testFrameIDFail);
	int m_frame_two = testBox.get_m_frame();

	int m_boxID = testBox.get_m_boxID();
	bool m_boxIDTrue = testBox.set_m_boxID(testBoxIDTWO);
	bool m_boxIDFail = testBox.set_m_boxID(testBoxIDFail);
	int m_boxIDTwo = testBox.get_m_boxID();

	bool setClassIDTrue = testBox.set_m_swimmerClass(testClassID);
	bool setClassIDFail = testBox.set_m_swimmerClass(testClassIDFail);
	int m_swimmerClass = testBox.get_m_swimmerClass();

	bool setConfTrue = testBox.set_m_confScore(testConf);
	bool setConfFail = testBox.set_m_confScore(testConfFail);
	float m_confScore = testBox.get_m_confScore();

	BOOST_CHECK_EQUAL(m_frame, testFrameID);
	BOOST_CHECK_EQUAL(setFrameTrue, true);
	BOOST_CHECK_EQUAL(setFrameFail, false);
	BOOST_CHECK_EQUAL(m_frame_two, testFrameIDTWO);

	BOOST_CHECK_EQUAL(m_boxID, testBoxID);
	BOOST_CHECK_EQUAL(m_boxIDTrue, true);
	BOOST_CHECK_EQUAL(m_boxIDFail, false);
	BOOST_CHECK_EQUAL(m_boxIDTwo, testBoxIDTWO);

	BOOST_CHECK_EQUAL(setClassIDTrue, true);
	BOOST_CHECK_EQUAL(setClassIDFail, false);
	BOOST_CHECK_EQUAL(m_swimmerClass, testClassID);

	BOOST_CHECK_EQUAL(setConfTrue, true);
	BOOST_CHECK_EQUAL(setConfFail, false);
	BOOST_CHECK_EQUAL(m_confScore, testConf);
}
BOOST_AUTO_TEST_CASE(testTrackingBoxInStreamFunctionsNEW)
{
	cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
	TrackingBox testBox(10, 5, tBox);

	stringstream ssGround;
	ssGround << testBox.get_m_frame() << "," << testBox.get_m_boxID() << "," << testBox.x << "," << testBox.y << "," << testBox.width << "," << testBox.height << ",1,-1,-1,-1" << std::endl;

	stringstream ss;
	testBox.outputToFile(ss);

	BOOST_CHECK_EQUAL(ssGround.str(), ss.str());
}
BOOST_AUTO_TEST_CASE(testTrackingBoxInStreamFunctions)
{
	cv::Rect_<float> tBox(5.3f, 3.4f, 6.8f, 9.00f);
	TrackingBox testBox(10, 5, tBox);

	stringstream ssGround;
	ssGround << testBox.get_m_frame() << "," << testBox.get_m_boxID() << "," << testBox.x << "," << testBox.y << "," << testBox.width << "," << testBox.height << ",1,-1,-1,-1" << std::endl;

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
	ssTester << groundBox.get_m_frame() << "," << groundBox.get_m_boxID() << "," << groundBox.x << "," << groundBox.y << "," << groundBox.width << "," << groundBox.height << ",1,-1,-1,-1" << std::endl;

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
	ssTester << groundBox.get_m_frame() << "," << groundBox.get_m_boxID() << "," << groundBox.x << "," << groundBox.y << "," << groundBox.width << "," << groundBox.height << ",1,-1,-1,-1" << std::endl;

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
	ssGround << testBox.get_m_frame() << "," << testBox.get_m_boxID();
	ssGround << "," << testBox.x << "," << testBox.y << "," << testBox.width << "," << testBox.height;
	ssGround << "," << testBox.get_m_confScore() << "," << testBox.get_m_swimmerClass() << std::endl;

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
	ssTester << groundBox.get_m_frame() << "," << groundBox.get_m_boxID();
	ssTester << "," << groundBox.x << "," << groundBox.y << "," << groundBox.width << "," << groundBox.height;
	ssTester << "," << groundBox.get_m_confScore() << groundBox.get_m_swimmerClass() << std::endl;

	TrackingBox testBox;

	//ssTester >> testBox;
	testBox.inputFromFileDetection(ssTester);

	BOOST_CHECK_GE(testBox.GetIOU(groundBox), 0.999);
}
BOOST_AUTO_TEST_SUITE_END() //End Box Tests

//Object Centering validation test suite
//TODO how to test finding centre
BOOST_AUTO_TEST_SUITE(ObjectCenteringTestSuite)
BOOST_AUTO_TEST_CASE(FindDifferenceTEST)
{
	objectCentering objCnt;

	//Test the difference in x:
	cv::Point_<float> frameCentre_Test = cv::Point_<float>(100, 100);
	TrackingBox pointSwimmer_Test1 = TrackingBox(1, 1, cv::Rect_<float>(40, 90, 20, 20));
	cv::Point_<float> diff_Test1 = objCnt.findPointDifference(frameCentre_Test, pointSwimmer_Test1);
	cv::Point_<float> resultExpected_Test1 = cv::Point_<float>(50, 0);

	TrackingBox pointSwimmer_Test2 = TrackingBox(1, 1, cv::Rect_<float>(150, 90, 20, 20));
	cv::Point_<float> diff_Test2 = objCnt.findPointDifference(frameCentre_Test, pointSwimmer_Test2);
	cv::Point_<float> resultExpected_Test2 = cv::Point_<float>(-60, 0);

	//Test the difference in y:
	TrackingBox pointSwimmer_Test3 = TrackingBox(1, 1, cv::Rect_<float>(90, 160, 20, 20));
	cv::Point_<float> diff_Test3 = objCnt.findPointDifference(frameCentre_Test, pointSwimmer_Test3);
	cv::Point_<float> resultExpected_Test3 = cv::Point_<float>(0, -70);
	
	TrackingBox pointSwimmer_Test4 = TrackingBox(1, 1, cv::Rect_<float>(90, 60, 20, 20));
	cv::Point_<float> diff_Test4 = objCnt.findPointDifference(frameCentre_Test, pointSwimmer_Test4);
	cv::Point_<float> resultExpected_Test4 = cv::Point_<float>(0, 30);

	BOOST_CHECK_EQUAL(resultExpected_Test1, diff_Test1);
	BOOST_CHECK_EQUAL(resultExpected_Test2, diff_Test2);
	BOOST_CHECK_EQUAL(resultExpected_Test3, diff_Test3);
	BOOST_CHECK_EQUAL(resultExpected_Test4, diff_Test4);
}
BOOST_AUTO_TEST_CASE(moveLeftandRightTEST)
{
	objectCentering objCnt;

	cv::Point_<float> frameCentre_Test = cv::Point_<float>(100, 100);
	
	//Test move left
	TrackingBox pointSwimmer_Test1 = TrackingBox(1, 1, cv::Rect_<float>(40, 90, 20, 20));
	tiltPanCommand result_Test1 = objCnt.findCommand(pointSwimmer_Test1, frameCentre_Test);
	tiltPanCommand resultExpected_Test1;
	resultExpected_Test1.moveDown = false;
	resultExpected_Test1.moveLeft = true;
	resultExpected_Test1.moveRight = false;
	resultExpected_Test1.moveUp = false;

	//Test move right
	TrackingBox pointSwimmer_Test2 = TrackingBox(1, 1, cv::Rect_<float>(150, 90, 20, 20));
	tiltPanCommand result_Test2 = objCnt.findCommand(pointSwimmer_Test2, frameCentre_Test);
	tiltPanCommand resultExpected_Test2;
	resultExpected_Test2.moveDown = false;
	resultExpected_Test2.moveLeft = false;
	resultExpected_Test2.moveRight = true;
	resultExpected_Test2.moveUp = false;

	//Test move neither left nor right
	TrackingBox pointSwimmer_Test3 = TrackingBox(1, 1, cv::Rect_<float>(90, 90, 20, 20));
	tiltPanCommand result_Test3 = objCnt.findCommand(pointSwimmer_Test3, frameCentre_Test);
	tiltPanCommand resultExpected_Test3;
	resultExpected_Test3.moveDown = false;
	resultExpected_Test3.moveLeft = false;
	resultExpected_Test3.moveRight = false;
	resultExpected_Test3.moveUp = false;

	BOOST_CHECK_EQUAL(result_Test1.moveDown, resultExpected_Test1.moveDown);
	BOOST_CHECK_EQUAL(result_Test1.moveUp, resultExpected_Test1.moveUp);
	BOOST_CHECK_EQUAL(result_Test1.moveLeft, resultExpected_Test1.moveLeft);
	BOOST_CHECK_EQUAL(result_Test1.moveRight, resultExpected_Test1.moveRight);

	BOOST_CHECK_EQUAL(result_Test2.moveDown, resultExpected_Test2.moveDown);
	BOOST_CHECK_EQUAL(result_Test2.moveUp, resultExpected_Test2.moveUp);
	BOOST_CHECK_EQUAL(result_Test2.moveLeft, resultExpected_Test2.moveLeft);
	BOOST_CHECK_EQUAL(result_Test2.moveRight, resultExpected_Test2.moveRight);

	BOOST_CHECK_EQUAL(result_Test3.moveDown, resultExpected_Test3.moveDown);
	BOOST_CHECK_EQUAL(result_Test3.moveUp, resultExpected_Test3.moveUp);
	BOOST_CHECK_EQUAL(result_Test3.moveLeft, resultExpected_Test3.moveLeft);
	BOOST_CHECK_EQUAL(result_Test3.moveRight, resultExpected_Test3.moveRight);
}
BOOST_AUTO_TEST_SUITE_END() //End Object Centering validation tests suite


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

//postProcessRTTrackingTestSuite test suite
BOOST_AUTO_TEST_SUITE(postProcessRTTrackingTestSuite)
BOOST_AUTO_TEST_CASE(postProcessgetCentrevalidationTEST) 
{
	int tpx = 0;
	int tpy = 0;
	int tpw = 50;
	int tph = 20;
	postProcessRealTimeTracking postProcessRTobj;
	cv::Point_<float> result;
	cv::Point_<float> truthPoint = Point_<float>((tpx+ tpw)/2, (tpy+ tph)/2);

	result = postProcessRTobj.getCentre(Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph)));
	
	BOOST_CHECK_EQUAL(truthPoint.x, result.x);
	BOOST_CHECK_EQUAL(truthPoint.y, result.y);
}
BOOST_AUTO_TEST_CASE(postProcessOneItemVectorTEST) 
{
	int tpx = 0;
	int tpy = 0;
	int tpw = 50;
	int tph = 20;
	cv::Point_<float> mouse_Test = cv::Point_<float>(2, 4);
	int IDtest = 1;
	postProcessRealTimeTracking postProcessRTobj;
	int result = 0;
	vector<TrackingBox> testVector;

	testVector.push_back(TrackingBox(1, IDtest, Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph))));

	result = postProcessRTobj.trajectoryMatcher(mouse_Test, testVector);

	BOOST_CHECK_EQUAL(result, IDtest);

}
BOOST_AUTO_TEST_CASE(postProcessOneItemVectorOutsideTEST)
{
	int tpx = 0;
	int tpy = 0;
	int tpw = 50;
	int tph = 20;
	cv::Point_<float> mouse_Test = cv::Point_<float>(54, 40);
	int IDtest = 1;
	postProcessRealTimeTracking postProcessRTobj;
	int result = 0;
	vector<TrackingBox> testVector;

	testVector.push_back(TrackingBox(1, IDtest, Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph))));

	result = postProcessRTobj.trajectoryMatcher(mouse_Test, testVector);

	BOOST_CHECK_EQUAL(result, IDtest);

}
BOOST_AUTO_TEST_CASE(postProcessTwoItemVectorTEST)
{
	int tpx = 0;
	int tpy = 0;
	int tpx2 = 100;
	int tpy2 = 40;
	int tpw = 50;
	int tph = 20;
	cv::Point_<float> mouse_Test = cv::Point_<float>(150, 41);
	postProcessRealTimeTracking postProcessRTobj;
	int result = 0;
	vector<TrackingBox> testVector;

	testVector.push_back(TrackingBox(1, 1, Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph))));
	testVector.push_back(TrackingBox(1, 2, Rect_<float>(Point_<float>(tpx2, tpy2), Point_<float>(tpx2 + tpw, tpy2 + tph))));

	result = postProcessRTobj.trajectoryMatcher(mouse_Test, testVector);

	BOOST_CHECK_EQUAL(result, 2);
}
BOOST_AUTO_TEST_CASE(postProcessTwoItemVectorTWOTEST)
{
	int tpx = 0;
	int tpy = 0;
	int tpx2 = 40;
	int tpy2 = 10;
	int tpw = 50;
	int tph = 20;
	cv::Point_<float> mouse_Test = cv::Point_<float>(70, 21);
	postProcessRealTimeTracking postProcessRTobj;
	int result = 0;
	vector<TrackingBox> testVector;

	testVector.push_back(TrackingBox(1, 1, Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph))));
	testVector.push_back(TrackingBox(1, 2, Rect_<float>(Point_<float>(tpx2, tpy2), Point_<float>(tpx2 + tpw, tpy2 + tph))));

	result = postProcessRTobj.trajectoryMatcher(mouse_Test, testVector);

	BOOST_CHECK_EQUAL(result, 2);

}
BOOST_AUTO_TEST_CASE(postProcessFourItemVectorTEST)
{
	Point_<float> p1 = Point_<float>(10, 10);
	Point_<float> p1_end = Point_<float>(10+10, 10+20);
	Point_<float> p2 = Point_<float>(18, 15);
	Point_<float> p2_end = Point_<float>(18 + 22, 15 + 15);
	Point_<float> p3 = Point_<float>(45, 35);
	Point_<float> p3_end = Point_<float>(45 + 25, 35 + 15);
	Point_<float> p4 = Point_<float>(50, 55);
	Point_<float> p4_end = Point_<float>(50 + 20, 55 + 15);

	cv::Point_<float> mouse_Test_1 = cv::Point_<float>(75, 75);
	cv::Point_<float> mouse_Test_2 = cv::Point_<float>(70, 57);
	cv::Point_<float> mouse_Test_3 = cv::Point_<float>(45, 20);
	cv::Point_<float> mouse_Test_4 = cv::Point_<float>(50, 40);

	int result1 = 0;
	int result2 = 0;
	int result3 = 0;
	int result4 = 0;

	postProcessRealTimeTracking postProcessRTobj;
	vector<TrackingBox> testVector;

	testVector.push_back(TrackingBox(1, 1, Rect_<float>(p1, p1_end)));
	testVector.push_back(TrackingBox(1, 2, Rect_<float>(p2, p2_end)));
	testVector.push_back(TrackingBox(1, 3, Rect_<float>(p3, p3_end)));
	testVector.push_back(TrackingBox(1, 4, Rect_<float>(p4, p4_end)));

	result1 = postProcessRTobj.trajectoryMatcher(mouse_Test_1, testVector);
	result2 = postProcessRTobj.trajectoryMatcher(mouse_Test_2, testVector);
	result3 = postProcessRTobj.trajectoryMatcher(mouse_Test_3, testVector);
	result4 = postProcessRTobj.trajectoryMatcher(mouse_Test_4, testVector);

	BOOST_CHECK_EQUAL(result1, 4);
	BOOST_CHECK_EQUAL(result2, 4);
	BOOST_CHECK_EQUAL(result3, 2);
	BOOST_CHECK_EQUAL(result4, 3);
}
BOOST_AUTO_TEST_SUITE_END() //End postProcessRTTrackingTestSuite test suite