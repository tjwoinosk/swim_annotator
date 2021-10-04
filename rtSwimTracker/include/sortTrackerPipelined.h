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

	static vector<KalmanTracker>& vectorsOfTrackers() { static vector<KalmanTracker> vectorsOfTrackers; return vectorsOfTrackers; }

	void fillFrameTrackingResultsWith(const vector<TrackingBox>& swimmerDetections);
	void initializeTrackerUsing(const vector<TrackingBox>& swimmerDetections);
	void makeKalmanPredictions();
	void associatePredictionsWith(const vector<TrackingBox>& swimmerDetections);
	void collectResultsWhileKillingTrackers();
	double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt);

	vector<vector<double>> constructIOUmat(const vector<TrackingBox>& detFrameData);
	void fillMatchedPairs(const vector<int>& assignment, const vector<vector<double>>& iouCostMatrix);
	void fillUnmatchedDetections(vector<int> assignments);
	void fillUnmatchedTrajectories(vector<int> assignments);

	void updateKalmanTrackers(const vector<TrackingBox>& detFrameData);
	void createNewKalmanTrackers(const vector<TrackingBox>& detFrameData);

	set<int> m_unmatchedDetections;
	set<int> m_unmatchedTrajectories;
	vector<cv::Point> m_matchedPairs;
	vector<TrackingBox> m_frameTrackingResult;
	vector<Rect_<float>> m_trajectoryPredictions;
	int m_numTrajectories;
	int m_numDetections;

	unsigned int m_numberFramesProcessed;
	const double m_iou = 0.05; //Orignaly this value was 0.30
	const int m_maxAgeInFrames = 1;
	const int m_minHitsInFrames = 3;
};


#endif // !SORTTRACKERPIPELINED_H

