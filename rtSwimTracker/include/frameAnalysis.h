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
#include "objectCentering.h"

class frameAnalysis
{
private:

	// global variables 
	bool analyzeSwimmer;
	int idSelectedSwimmer;
	int indexSelectedSwimmer; 
	int statusSelected; //1=Tracking, 2=Not tracking but selected swimmer, 3=Not tracking
	int currentFrameNum;

	bool usePredefinedDetections; //TODO for use on CPU to speed up testing
	std::vector<std::vector<TrackingBox>> predefinedDetections; //TODO for use on CPU to speed up testing

	objectCentering centeringObj;

	swimmerDetector detectSwimmersInVideo;
	sortTrackerPiplelined trackSORTprocessorInVideo;

	std::vector<TrackingBox> resultsSingleSwimmer; //Holds results of single swimmer from all tracked frames
	std::vector<TrackingBox> currentResults; //Holds tracking of all swimmers for the most recent frame
	std::vector<tiltPanCommand> commandResults; //Holds command results for the single swimmer that is being followed (not necessarily tracked)
	std::vector<tiltPanCommand> commandResults_KeyBoard; //Holds command results for the single swimmer that is being followed (not necessarily tracked)

public:

	std::string sortOnFrame(SpeedReporter* report = NULL);

	std::string runDetectorOnFrames(SpeedReporter* report = NULL);

	void getDataFromDetectionFile(std::string detFileName, std::vector<TrackingBox>& detData);
	int groupingDetectionData(std::vector<TrackingBox> detData, std::vector<std::vector<TrackingBox>>& detFrameData);

	frameAnalysis();

	//void analyzeVideo(std::string videoToAnalyzeName);
	TrackingBox analyzeVideo(cv::Mat frameToAnalyze);

	void setAnalyzeSwimmer(bool valSetTo);
	bool setIDSelectedSwimmer(int valSetTo);
	bool getAnalyzeSwimmer();
	int getIDSelectedSwimmer();
	bool setindexSelectedSwimmer(int valSetTo);
	int getindexSelectedSwimmer();
	int findindexSelectedSwimmer(int idSwimmer);

	std::vector<TrackingBox> getCurrentResults();
	int getStatus();
	void setStatus(bool tracking, int selectedSwimmer);
	bool isTracking();
	bool isFollowing();
	std::vector<TrackingBox> getSingleSwimmerResults();
	int getCurrentFrameNum();
	bool setCurrentFrameNum(int val);

	void setVideoData(cv::Mat frame, float deltaX, float deltaY);
	cv::Point_<float> getVideoDataInfo();

	void buttonClicked(char c);

	void writeToFile();

	void resizeBoxes(float scaleX, float scaleY, std::vector<TrackingBox>& dataToResize); 
	float findFrameScale(int newFrameSize, int currentFrameSize);
	TrackingBox resizeBox(float scaleX, float scaleY, TrackingBox boxToResize);

	void setUseDetetionFile(std::string fileName); //TODO for use on CPU to speed up testing
	void setDontUseDetectionFile();
};


#endif // !FRAMEANALYSIS_H