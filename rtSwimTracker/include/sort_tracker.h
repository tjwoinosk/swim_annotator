#ifndef SORT_TRACKER_H
#define SORT_TRACKER_H

#include <iostream>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
#include <io.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include <set>

#include "HungarianAlgorithm.h"
#include "KalmanTracker.h"

#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp> //displaying video
#include <opencv2/opencv.hpp> //displaying video

using namespace std;
using namespace cv;


//Implementation used from 
//https://github.com/mcximing/sort-cpp

typedef struct TrackingBox
{
	int frame;
	int id;
	Rect_<float> box;
}TrackingBox;

#define CNUM 20

class sort_tracker
{
private:

	// global variables for counting
	int total_frames = 0;
	double total_time = 0.0;

public:

	// Computes IOU between two bounding boxes
	double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt);

	sort_tracker();
	void sortTracker(string seqName, double iou);

	//Added on functions for use in pipelining sortTracker
	void getDataFromDetectionFile(string detFileName, vector<TrackingBox>& detData);
	int groupingDetectionData(vector<TrackingBox> detData, vector<vector<TrackingBox>>& detFrameData);
	void trackingForSingleFrame(vector<KalmanTracker>& trackers, vector<TrackingBox> detFrameData, ofstream& resultsFile, double iou, int max_age, int min_hits, int frame_count);
	void sortTrackerUsingFunctions(string seqName, double iou);

	//Added on functions to implement pipelining sortTracker
	void sortTrackerPipelined(string outputFileName, double iou, vector<TrackingBox> detData);
	void sortLoopingPipelined(String outputFileName, double iou);
};


#endif // !SORT_TRACKER_H