#ifndef FRAMEANALYSIS_H
#define FRAMEANALYSIS_H

#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "SORTtrackingBox.h"
#include "sortTrackerPipelined.h";

using namespace std;
using namespace cv;

class frameAnalysis
{
private:

	// global variables 

public:

	void sortOnFrame(string seqName);

	void getDataFromDetectionFile(string detFileName, vector<TrackingBox>& detData);
	int groupingDetectionData(vector<TrackingBox> detData, vector<vector<TrackingBox>>& detFrameData);

	frameAnalysis();

	void analyzeVideo(string videoToAnalyzeName);

};


#endif // !FRAMEANALYSIS_H
