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
#include "sortTrackerPipelined.h"
#include "swimmerDetector.h"
#include "SpeedReporter.h"

class frameAnalysis
{
private:

	// global variables 
	bool analyzeSwimmer;
	int idSelectedSwimmer;

	//TODO below is to test
	int frameCount;

	swimmerDetector detectSwimmersInVideo;
	sortTrackerPiplelined trackSORTprocessorInVideo;

public:

	std::string sortOnFrame(SpeedReporter* report = NULL);

	std::string runDetectorOnFrames(SpeedReporter* report = NULL);

	void getDataFromDetectionFile(std::string detFileName, std::vector<TrackingBox>& detData);
	int groupingDetectionData(std::vector<TrackingBox> detData, std::vector<std::vector<TrackingBox>>& detFrameData);

	frameAnalysis();

	void analyzeVideo(std::string videoToAnalyzeName);
	std::vector<TrackingBox> analyzeVideo(cv::Mat frameToAnalyze);

	void setAnalyzeSwimmer(bool valSetTo);
	bool setIDSelectedSwimmer(int valSetTo);
	bool getAnalyzeSwimmer();
	int getIDSelectedSwimmer();
};


#endif // !FRAMEANALYSIS_H