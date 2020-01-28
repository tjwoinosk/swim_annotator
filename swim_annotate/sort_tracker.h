#pragma once
#include "swimmer_tracking.h"

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

	// Computes IOU between two bounding boxes
	double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt);

public:

	sort_tracker();
	void TestSORT(string seqName, double iou);

};

