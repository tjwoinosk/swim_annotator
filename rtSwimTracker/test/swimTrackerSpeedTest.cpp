#include <iostream>

#include <boost/timer/timer.hpp>

#include "SORTtrackingBox.h"
#include "sortTrackerPipelined.h";
#include "frameAnalysis.h"

void testSORTTrackingSpeed();


int main()
{
	std::cout << "Running Speed Tests..." << std::endl << std::endl;

	testSORTTrackingSpeed();


}

void testSORTTrackingSpeed()
{
	std::cout << "Start SORT Speed Test" << std::endl;

	boost::timer::cpu_timer measureSORT;

	frameAnalysis getData;
	sortTrackerPiplelined SORTprocessor;

	std::vector<TrackingBox> detData;
	std::vector<std::vector<TrackingBox>> detFrameData;
	std::string seqName = "test\\testData\\PipeTest.txt";

	int maxFrame = 0;
	getData.getDataFromDetectionFile(seqName, detData);
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

	std::cout << "\tSingle Frame SORT speed (AVG): " << boost::timer::format(res) << std::endl;

}