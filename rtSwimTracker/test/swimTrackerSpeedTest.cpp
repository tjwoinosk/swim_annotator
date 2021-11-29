#define BOOST_TEST_MODULE Swim Tracker Speed Tests
#include <boost/test/included/unit_test.hpp>
//Add --detect_memory_leak=0 to debug command to remove memory leak output
//--catch_system_error=no
//--log_level=test_suite in command for more info https://www.boost.org/doc/libs/1_75_0/libs/test/doc/html/boost_test/utf_reference/rt_param_reference/log_level.html

/*
References: [2]
*/

#include "frameAnalysis.h"
#include "sortTrackerPipelined.h"
#include "swimmerDetector.h"

#include "TrackingBox.h"
#include "fileFinder.h"
#include "SpeedReporter.h"

#include <iostream>
#include <fstream>
#include <iterator>


BOOST_AUTO_TEST_SUITE(speedTestSuite)

BOOST_AUTO_TEST_CASE(speedTEST)
{
	frameAnalysis testDetector;
	std::string resAbsPath = "";
	DetectionSpeedReporter reportDetSpeed;
	reportDetSpeed.startReporter();

	if (reportDetSpeed.isReady())
	{
		resAbsPath = testDetector.runDetectorOnFrames(&reportDetSpeed);

		//results file
		std::string resAbsPathGT = resAbsPath;
		resAbsPathGT.replace(resAbsPathGT.end() - 4, resAbsPathGT.end(), "GT.txt");

		std::ifstream det_ifs1(resAbsPath);
		std::ifstream det_ifs2(resAbsPathGT);

		std::istream_iterator<char> det_b1(det_ifs1), det_e1;
		std::istream_iterator<char> det_b2(det_ifs2), det_e2;

		// compare 
		BOOST_CHECK_EQUAL_COLLECTIONS(det_b1, det_e1, det_b2, det_e2);
	}

	///////////////////////////////////////////////////////////////////////////
	frameAnalysis testSORTTWO;
	std::string gtPath = "";
	std::string outputName = "";
	TrackingSpeedReporter reportSpeed;

	reportSpeed.startReporter();

	if (reportSpeed.isReady())
	{
		outputName = testSORTTWO.sortOnFrame(&reportSpeed);
		gtPath = outputName;

		//results file
		gtPath.replace(gtPath.end() - 4, gtPath.end(), "GT.txt");

		std::ifstream ifs1(outputName);
		std::ifstream ifs2(gtPath);

		std::istream_iterator<char> b1(ifs1), e1;
		std::istream_iterator<char> b2(ifs2), e2;

		BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
	}
}

BOOST_AUTO_TEST_SUITE_END() //End speed tests suite