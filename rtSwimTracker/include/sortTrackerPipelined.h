#ifndef SORTTRACKERPIPELINED_H
#define SORTTRACKERPIPELINED_H


#include <iostream>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
#include <io.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include <set>

#include "HungarianAlgorithm.h"
#include "KalmanTracker.h"
//#include "sort_tracker.h" //TODO do we need this? or create new trackerBox struct
#include "SORTtrackingBox.h"

#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp> //displaying video
#include <opencv2/opencv.hpp> //displaying video

using namespace std;
using namespace cv;


//Implementation used from 
//https://github.com/mcximing/sort-cpp

class sortTrackerPiplelined
{
public:

	sortTrackerPiplelined();
	vector<TrackingBox> singleFrameSORT(const vector<TrackingBox>& swimmerDetections);

private:
	void inputDetectionData(const vector<TrackingBox>& detFrameData) 
	{ 
		m_frameData = detFrameData; 
		m_numDetections = detFrameData.size();
	}

	void initializeTrackersUsing(const vector<TrackingBox>& detFrameData);
	void processFrame();
	vector<Rect_<float>>& createTrajecotoryPredictions(vector<Rect_<float>>& initializedValue);
	vector<vector<double>>& constructIOUCostMat(const vector<Rect_<float>>& trajectoryPredictions, vector<vector<double>>& iouCostMatrix);
	vector<cv::Point>& matchDetectionsToTrajectories(const vector<vector<double>>& iouCostMatrix, vector<cv::Point>& pairs);
	void updateTrackers(const vector<cv::Point>& pairs);
	void createNewTrackersWithLeftoverDetections();
	void collectResultsWhileKillingTrackers();
	void fillUnmatchedDetections(const vector<int>& assignments);
	void fillUnmatchedTrajectories(const vector<int>& assignments);
	double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt);

	vector<KalmanTracker> m_vectorOfTrackers;
	vector<TrackingBox> m_frameData;
	vector<TrackingBox> m_frameTrackingResults;
	set<int> m_unmatchedDetections;
	set<int> m_unmatchedTrajectories;
	int m_numTrajectories;
	int m_numDetections;
	unsigned int m_numberFramesProcessed;

	const double m_iou = 0.05; //Orignaly this value was 0.30
	const int m_maxUpdateAllowance = 1;
	const int m_minHitsInFrames = 3;
};


#endif // !SORTTRACKERPIPELINED_H

