#include <iostream>
#include <fstream>

#include <boost/timer/timer.hpp>
#include <boost/date_time.hpp>

#include "TrackingBox.h"
#include "sortTrackerPipelined.h";
#include "frameAnalysis.h"
#include "fileFinder.h"

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
		updateFile.open(find.retrunAbsolutePath(SORTLog), std::ios::app);
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
	getData.getDataFromDetectionFile(find.retrunAbsolutePath(seqName), detData);
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