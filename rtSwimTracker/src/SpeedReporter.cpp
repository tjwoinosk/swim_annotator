#include "SpeedReporter.h"

void SpeedReporter::produceMeasurementAVG()
{
	boost::timer::nanosecond_type divTerm = static_cast<boost::timer::nanosecond_type>(m_numberIterations);
	m_measurement.system /= divTerm;
	m_measurement.user /= divTerm;
	m_measurement.wall /= divTerm;
}

void TrackingSpeedReporter::startReporter()
{
	fileFinder find;
	std::string SORTLog = "SORTSpeedLog.txt";
	SORTLog = find.returnSpeedTestLocation() + SORTLog;

	try
	{
		m_updateFile.open(SORTLog, std::ios::app);
	}
	catch (const std::exception & e)
	{
		std::cout << "Could not open SORT speed Log: " << e.what() << std::endl;
	}
}

void TrackingSpeedReporter::reportSpeed()
{
	if (this->isReady())
	{
		std::string logSpeed;
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		logSpeed = boost::timer::format(m_measurement);

		m_updateFile << now << " -> ";
		m_updateFile << "Single Frame SORT speed (AVG): " << logSpeed;
	}
	else
	{
		std::cout << "Could not report tracking speed, reporting file not open" << std::endl;
	}
}

void DetectionSpeedReporter::startReporter()
{
	fileFinder find;
	std::string detectionLog = "DetectionSpeedLog.txt";
	detectionLog = find.returnSpeedTestLocation() + detectionLog;

	try
	{
		m_updateFile.open(detectionLog, std::ios::app);
	}
	catch (const std::exception & e)
	{
		std::cout << "Could not open detection speed Log: " << e.what() << std::endl;
	}
}

void DetectionSpeedReporter::reportSpeed()
{
	if (this->isReady())
	{
		std::string logSpeed;
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		logSpeed = boost::timer::format(m_measurement);

		m_updateFile << now << " -> ";
		m_updateFile << "Single Frame detection speed (AVG): " << logSpeed;
	}
	else
	{
		std::cout << "Could not report detection speed, reporting file not open" << std::endl;
	}
}


/*

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



	return "Single Frame SORT speed (AVG): " + boost::timer::format(res);

}

*/