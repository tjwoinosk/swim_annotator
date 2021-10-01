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
private:
	// global variables for counting
	int frame_count;

	//variables for SORT Tracking parameters
	const double iou = 0.05; //Orignaly this value was 0.30
	const int max_age = 1;
	const int min_hits = 3;

	//https://stackoverflow.com/questions/18860895/how-to-initialize-static-members-in-the-header

	static vector<KalmanTracker>& vectorsOfTrackers() {static vector<KalmanTracker> vectorsOfTrackers; return vectorsOfTrackers;}
	
public:

	sortTrackerPiplelined();
	double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt);

	vector<TrackingBox> singleFrameSORT(vector<TrackingBox> detFrameData);

private:

	void initializeTracker(const vector<TrackingBox>& detFrameData);
	void getPredictions();
	void assoicateDetections(const vector<TrackingBox>& detFrameData);
	void updateTrackers(const vector<TrackingBox>& detFrameData);

	unsigned int numberFramesProcessed;
	unsigned int trkNum = 0;
	unsigned int detNum = 0;
	vector<vector<double>> iouMatrix;
	vector<int> assignment;
	set<int> unmatchedDetections;
	set<int> unmatchedTrajectories;
	set<int> allItems;
	set<int> matchedItems;
	vector<cv::Point> matchedPairs;
	vector<TrackingBox> frameTrackingResult;
	vector<Rect_<float>> predictedBoxes;

};


#endif // !SORTTRACKERPIPELINED_H

