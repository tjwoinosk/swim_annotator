#ifndef FRAMEANALYSIS_H
#define FRAMEANALYSIS_H

#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <boost/timer/timer.hpp>

#include "SORTtrackingBox.h"
#include "sortTrackerPipelined.h";

class frameAnalysis
{
private:

	// global variables 

public:

	void sortOnFrame(std::string seqName);

	void getDataFromDetectionFile(std::string detFileName, std::vector<TrackingBox>& detData);
	int groupingDetectionData(std::vector<TrackingBox> detData, std::vector<std::vector<TrackingBox>>& detFrameData);

	frameAnalysis();

	void analyzeVideo(std::string videoToAnalyzeName);

};


#endif // !FRAMEANALYSIS_H
