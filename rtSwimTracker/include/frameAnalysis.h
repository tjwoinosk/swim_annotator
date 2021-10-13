#ifndef FRAMEANALYSIS_H
#define FRAMEANALYSIS_H

#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "fileFinder.h"

#include "TrackingBox.h"
#include "DetectionBox.h"
#include "sortTrackerPipelined.h"
#include "swimmerDetector.h"
#include "SpeedReporter.h"

class frameAnalysis
{
private:

	// global variables 

public:

	std::string sortOnFrame(SpeedReporter* report = NULL);

	std::string sortOnFrameDet();

	std::string runDetectorOnFrames(SpeedReporter* report = NULL);

	void getDataFromDetectionFile(std::string detFileName, std::vector<TrackingBox>& detData);
	int groupingDetectionData(std::vector<TrackingBox> detData, std::vector<std::vector<TrackingBox>>& detFrameData);

	void getDataFromDetectionFile(std::string detFileName, std::vector<DetectionBox>& detData);
	int groupingDetectionData(std::vector<DetectionBox> detData, std::vector<std::vector<DetectionBox>>& detFrameData);

	frameAnalysis();

	void analyzeVideo(std::string videoToAnalyzeName);
	void analyzeVideo(cv::Mat frameToAnalyze);
};


#endif // !FRAMEANALYSIS_H