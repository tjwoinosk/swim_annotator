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
	int indexSelectedSwimmer; 

	swimmerDetector detectSwimmersInVideo;
	sortTrackerPiplelined trackSORTprocessorInVideo;

	std::vector<TrackingBox> resultsSingleSwimmer; //Holds results of single swimmer from all tracked frames
	std::vector<TrackingBox> currentResults; //Holds tracking of all swimmers for the most recent frame

public:

	std::string sortOnFrame(SpeedReporter* report = NULL);

	std::string runDetectorOnFrames(SpeedReporter* report = NULL);

	void getDataFromDetectionFile(std::string detFileName, std::vector<TrackingBox>& detData);
	int groupingDetectionData(std::vector<TrackingBox> detData, std::vector<std::vector<TrackingBox>>& detFrameData);

	frameAnalysis();

	//void analyzeVideo(std::string videoToAnalyzeName);
	TrackingBox analyzeVideo(cv::Mat frameToAnalyze, bool saveResults);

	void setAnalyzeSwimmer(bool valSetTo);
	bool setIDSelectedSwimmer(int valSetTo);
	bool getAnalyzeSwimmer();
	int getIDSelectedSwimmer();
	bool setindexSelectedSwimmer(int valSetTo);
	int getindexSelectedSwimmer();
	int findindexSelectedSwimmer(int idSwimmer, std::vector<TrackingBox> allSwimmers);
	std::vector<TrackingBox> getCurrentResults();

	void writeToFile();

	void resizeBoxes(float scaleX, float scaleY, std::vector<TrackingBox>& dataToResize); 
	float findFrameScale(int newFrameSize, int currentFrameSize);
	TrackingBox resizeBox(float scaleX, float scaleY, TrackingBox boxToResize);
};


#endif // !FRAMEANALYSIS_H