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

class frameAnalysis
{
private:

	// global variables 

public:

	void sortOnFrame(std::string seqName);

	std::string runDetectorOnFrames();

	void getDataFromDetectionFile(std::string detFileName, std::vector<TrackingBox>& detData);
	int groupingDetectionData(std::vector<TrackingBox> detData, std::vector<std::vector<TrackingBox>>& detFrameData);

	frameAnalysis();

	void analyzeVideo(std::string videoToAnalyzeName);

};


#endif // !FRAMEANALYSIS_H
