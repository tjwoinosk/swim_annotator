#define BOOST_TEST_MODULE Swim Tracker Speed Tests
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

#include <iostream>
#include <fstream>
#include <iterator>

//added...
#include <boost/timer/timer.hpp>
#include <boost/date_time.hpp>


//Detection validation test suite
///*
BOOST_AUTO_TEST_SUITE(DetectionTestSuite)
BOOST_AUTO_TEST_CASE(DetectionSpeedTEST)
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

BOOST_AUTO_TEST_CASE(SORTspeedTEST)
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

/*

std::string testSORTTrackingSpeed();

namespace bpt = boost::posix_time;

int main()
{
	std::cout << "Running Speed Tests..." << std::endl << std::endl;
	fileFinder find;

	std::fstream updateFile;
	std::string logSpeed;

	bpt::ptime now = bpt::second_clock::local_time();

	const std::string SORTLog = "SORTSpeedLog.txt";

	try 
	{
		updateFile.open(find.absolutePath(SORTLog), std::ios::app);
		logSpeed = testSORTTrackingSpeed();
		updateFile << now << " -> " << logSpeed;
		updateFile.close();
	}
	catch(const std::exception & e)
	{
		std::cout << "Could not open SORT Speed Log: " << e.what() << std::endl;
	}


}

std::string testSORTTrackingSpeed()
{
	std::cout << "Start SORT Speed Test" << std::endl;

	boost::timer::cpu_timer measureSORT;
	fileFinder find;

	frameAnalysis getData;
	sortTrackerPiplelined SORTprocessor;

	std::vector<TrackingBox> detData;
	std::vector<std::vector<TrackingBox>> detFrameData;
	std::string seqName = "PipeTest.txt";

	int maxFrame = 0;
	getData.getDataFromDetectionFile(find.absolutePath(seqName), detData);
	maxFrame = getData.groupingDetectionData(detData, detFrameData);

	std::vector<TrackingBox> tempResults;
	tempResults.clear();
	measureSORT.start();
	for (int fi = 0; fi < maxFrame; fi++)
	{
		tempResults.clear();

		measureSORT.resume();
		tempResults = SORTprocessor.singleFrameSORT(detFrameData[fi]);
		measureSORT.stop();
	}

	boost::timer::nanosecond_type divTerm = static_cast<boost::timer::nanosecond_type>(maxFrame);
	boost::timer::cpu_times res = measureSORT.elapsed();
	res.system /= divTerm;
	res.user /= divTerm;
	res.wall /= divTerm;

	return "Single Frame SORT speed (AVG): " + boost::timer::format(res);

}

*/