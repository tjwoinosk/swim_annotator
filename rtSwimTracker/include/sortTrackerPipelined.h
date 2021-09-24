#ifndef SORTTRACKERPIPELINED_H
#define SORTTRACKERPIPELINED_H


#include <iostream>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
#include <io.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include <set>

#include "HungarianAlgorithm.h"
#include "KalmanTracker.h"
#include "sort_tracker.h" //TODO do we need this? or create new trackerBox struct

#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp> //displaying video
#include <opencv2/opencv.hpp> //displaying video

using namespace std;
using namespace cv;


//Implementation used from 
//https://github.com/mcximing/sort-cpp


//typedef struct TrackingBox
//{
//	int frame;
//	int id;
//	Rect_<float> box;
//}TrackingBox;

#define CNUM 20


class sortTrackerPiplelined
{
private:
	// global variables for counting
	int total_frames = 0;
	double total_time = 0.0;
	int frame_count;

	//variables for SORT Tracking parameters
	const double iou = 0.05;
	const int max_age = 1;
	const int min_hits = 3;


	static int& frameCounter() { static int frameCounter; return frameCounter; }
	//https://stackoverflow.com/questions/18860895/how-to-initialize-static-members-in-the-header
	
public:

	sortTrackerPiplelined();
	double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt);

	void sortOnFrame(string seqName);
	vector<TrackingBox> singleFrameSORT(vector<KalmanTracker>& trackers, vector<TrackingBox> detFrameData);

	void getDataFromDetectionFile(string detFileName, vector<TrackingBox>& detData);
	int groupingDetectionData(vector<TrackingBox> detData, vector<vector<TrackingBox>>& detFrameData);
};


#endif // !SORTTRACKERPIPELINED_H

