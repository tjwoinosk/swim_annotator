#include "sort_tracker.h"
#include <boost/chrono/thread_clock.hpp>

sort_tracker::sort_tracker() {

}

// Computes IOU between two bounding boxes
double sort_tracker::GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt)
{
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}



//Implementation used from 
//https://github.com/mcximing/sort-cpp
void sort_tracker::sortTracker(string seqName, double iou)
{
	cout << "Processing " << seqName << "..." << endl;

	// 1. read detection file
	ifstream detectionFile;
	string detFileName = seqName;
	detectionFile.open(detFileName);

	if (!detectionFile.is_open())
	{
		cerr << "Error: can not find file " << detFileName << endl;
		return;
	}

	string detLine;
	istringstream ss;
	vector<TrackingBoxOld> detData;
	char ch;
	float tpx, tpy, tpw, tph;

	while (getline(detectionFile, detLine))
	{
		TrackingBoxOld tb;

		ss.str(detLine);
		ss >> tb.frame >> ch >> tb.id >> ch;
		ss >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;
		ss.str("");

		tb.box = Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph));
		detData.push_back(tb);
	}
	detectionFile.close();

	// 2. group detData by frame
	int maxFrame = 0;
	for (auto tb : detData) // find max frame number
	{
		if (maxFrame < tb.frame)
			maxFrame = tb.frame;
	}

	vector<vector<TrackingBoxOld>> detFrameData;
	vector<TrackingBoxOld> tempVec;
	for (int fi = 0; fi < maxFrame; fi++)
	{
		for (auto tb : detData)
			if (tb.frame == fi + 1) // frame num starts from 1
				tempVec.push_back(tb);
		detFrameData.push_back(tempVec);
		tempVec.clear();
	}

	// 3. update across frames
	int frame_count = 0;
	int max_age = 1;//var for killing a tracker should be changed, in paper max_age = 1;
	int min_hits = 3;//min hits is min number of hits for it to be an object originaly min_hits = 3;
	double iouThreshold = iou;//Orignaly this value was 0.30
	vector<KalmanTracker> trackers;
	KalmanTracker::kf_count = 0; // tracking id relies on this, so we have to reset it in each seq.


	// variables used in the for-loop
	vector<Rect_<float>> predictedBoxes;
	vector<vector<double>> iouMatrix;
	vector<int> assignment;
	set<int> unmatchedDetections;
	set<int> unmatchedTrajectories;
	set<int> allItems;
	set<int> matchedItems;
	vector<cv::Point> matchedPairs;
	vector<TrackingBoxOld> frameTrackingResult;
	unsigned int trkNum = 0;
	unsigned int detNum = 0;

	double cycle_time = 0.0;
	int64 start_time = 0;

	// prepare result file.
	ofstream resultsFile;
	string resFileName = seqName;
	resFileName.replace(resFileName.end() - 4, resFileName.end(), "_det.txt");

	resultsFile.open(resFileName);

	if (!resultsFile.is_open())
	{
		cerr << "Error: can not create file " << resFileName << endl;
		return;
	}


	//////////////////////////////////////////////
	// main loop
	for (int fi = 0; fi < maxFrame; fi++)
	{
		total_frames++;
		frame_count++;
		//cout << frame_count << endl;

		// I used to count running time using clock(), but found it seems to conflict with cv::cvWaitkey(),
		// when they both exists, clock() can not get right result. Now I use cv::getTickCount() instead.
		start_time = getTickCount();

		if (trackers.size() == 0) // the first frame met
		{
			// initialize kalman trackers using first detections.
			for (unsigned int i = 0; i < detFrameData[fi].size(); i++)
			{
				//KalmanTracker trk = KalmanTracker(detFrameData[fi][i].box, process_mat, obser_mat);
				KalmanTracker trk = KalmanTracker(detFrameData[fi][i].box);
				trackers.push_back(trk);
			}
			// output the first frame detections
			for (unsigned int id = 0; id < detFrameData[fi].size(); id++)
			{
				TrackingBoxOld tb = detFrameData[fi][id];
				resultsFile << tb.frame << "," << id + 1 << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
				//Save to results in swimmer_tracking.h

			}
			continue;
		}

		///////////////////////////////////////
		// 3.1. get predicted locations from existing trackers.
		predictedBoxes.clear();

		for (auto it = trackers.begin(); it != trackers.end();)
		{
			Rect_<float> pBox = (*it).predict();
			if (pBox.x >= 0 && pBox.y >= 0)
			{
				predictedBoxes.push_back(pBox);
				it++;
			}
			else
			{
				it = trackers.erase(it);
				//cerr << "Box invalid at frame: " << frame_count << endl;
			}
		}

		///////////////////////////////////////
		// 3.2. associate detections to tracked object (both represented as bounding boxes)
		// dets : detFrameData[fi]
		trkNum = predictedBoxes.size();
		detNum = detFrameData[fi].size();

		iouMatrix.clear();
		iouMatrix.resize(trkNum, vector<double>(detNum, 0));

		for (unsigned int i = 0; i < trkNum; i++) // compute iou matrix as a distance matrix
		{
			for (unsigned int j = 0; j < detNum; j++)
			{
				// use 1-iou because the hungarian algorithm computes a minimum-cost assignment.
				iouMatrix[i][j] = 1 - GetIOU(predictedBoxes[i], detFrameData[fi][j].box);
			}
		}

		// solve the assignment problem using hungarian algorithm.
		// the resulting assignment is [track(prediction) : detection], with len=preNum
		HungarianAlgorithm HungAlgo;
		assignment.clear();
		HungAlgo.Solve(iouMatrix, assignment);

		// find matches, unmatched_detections and unmatched_predictions
		unmatchedTrajectories.clear();
		unmatchedDetections.clear();
		allItems.clear();
		matchedItems.clear();

		if (detNum > trkNum) //	there are unmatched detections
		{
			for (unsigned int n = 0; n < detNum; n++)
				allItems.insert(n);

			for (unsigned int i = 0; i < trkNum; ++i)
				matchedItems.insert(assignment[i]);

			set_difference(allItems.begin(), allItems.end(),
				matchedItems.begin(), matchedItems.end(),
				insert_iterator<set<int>>(unmatchedDetections, unmatchedDetections.begin()));
		}
		else if (detNum < trkNum) // there are unmatched trajectory/predictions
		{
			for (unsigned int i = 0; i < trkNum; ++i)
				if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
					unmatchedTrajectories.insert(i);
		}
		else
			;

		// filter out matched with low IOU
		matchedPairs.clear();
		for (unsigned int i = 0; i < trkNum; ++i)
		{
			if (assignment[i] == -1) // pass over invalid values
				continue;
			if (1 - iouMatrix[i][assignment[i]] < iouThreshold)
			{
				unmatchedTrajectories.insert(i);
				unmatchedDetections.insert(assignment[i]);
			}
			else
				matchedPairs.push_back(cv::Point(i, assignment[i]));
		}

		///////////////////////////////////////
		// 3.3. updating trackers

		// update matched trackers with assigned detections.
		// each prediction is corresponding to a tracker
		int detIdx, trkIdx;
		for (unsigned int i = 0; i < matchedPairs.size(); i++)
		{
			trkIdx = matchedPairs[i].x;
			detIdx = matchedPairs[i].y;
			trackers[trkIdx].update(detFrameData[fi][detIdx].box);
		}

		// create and initialise new trackers for unmatched detections
		for (auto umd : unmatchedDetections)
		{
			//KalmanTracker tracker = KalmanTracker(detFrameData[fi][umd].box, process_mat, obser_mat);
			KalmanTracker tracker = KalmanTracker(detFrameData[fi][umd].box);
			trackers.push_back(tracker);
		}

		// get trackers' output
		frameTrackingResult.clear();
		for (auto it = trackers.begin(); it != trackers.end();)
		{
			if (((*it).m_time_since_update < max_age) &&
				((*it).m_hit_streak >= min_hits || frame_count <= min_hits))
			{
				TrackingBoxOld res;
				res.box = (*it).get_state();
				res.id = (*it).m_id + 1;
				res.frame = frame_count;
				frameTrackingResult.push_back(res);
				it++;
			}
			else
				it++;

			// remove dead tracklet
			if (it != trackers.end() && (*it).m_time_since_update > max_age)
				it = trackers.erase(it);
		}

		cycle_time = (double)(getTickCount() - start_time);
		total_time += cycle_time / getTickFrequency();

		for (auto tb : frameTrackingResult) {
			resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
			//Save to results in swimmer_tracking.h

		}
	}

	resultsFile.close();

}



//Break sortTracker into various funtions so that we can then use them for pipelining
//Implementation used from 
//https://github.com/mcximing/sort-cpp

void sort_tracker::getDataFromDetectionFile(string detFileName, vector<TrackingBoxOld>& detData)
{
	/*
	The purpose of this function is to read in the file whose name is speciied by the input detFileName
	and put the information of this file into a vector of TrackingBoxes, which is the argument detData
	*/
	string detLine;
	istringstream ss;
	char ch;
	float tpx, tpy, tpw, tph;
	ifstream detectionFile;

	detectionFile.open(detFileName);

	if (!detectionFile.is_open())
	{
		cerr << "Error: can not find file " << detFileName << endl;
		return;
	}



	while (getline(detectionFile, detLine))
	{
		TrackingBoxOld tb;

		ss.str(detLine);
		ss >> tb.frame >> ch >> tb.id >> ch;
		ss >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;
		ss.str("");

		tb.box = Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph));
		detData.push_back(tb);
	}
	detectionFile.close();

	return;
}

int sort_tracker::groupingDetectionData(vector<TrackingBoxOld> detData, vector<vector<TrackingBoxOld>>& detFrameData)
{
	/*
	This function takes an input detData that is all the detection data stored in a vector of TrackingBox
	and then grouping TrackingBoxes for a single frame into a vector, and storing this vector into another 
	vector which is the input detFrameData.
	The result is a 2D array like vector where each element points to a vector of TrackingBoxes of the same frame.
	*/

	int maxFrame = 0;
	vector<TrackingBoxOld> tempVec;

	for (auto tb : detData) // find max frame number
	{
		if (maxFrame < tb.frame)
			maxFrame = tb.frame;
	}


	for (int fi = 0; fi < maxFrame; fi++)
	{
		for (auto tb : detData)
			if (tb.frame == fi + 1) // frame num starts from 1
				tempVec.push_back(tb);
		detFrameData.push_back(tempVec);
		tempVec.clear();
	}
	return maxFrame;
}

void sort_tracker::trackingForSingleFrame(vector<KalmanTracker>& trackers, vector<TrackingBoxOld> detFrameData, ofstream& resultsFile, double iou, int max_age, int min_hits, int frame_count)
{
	/*
	This function will take trackers and data for a single frame (frame number fi) and produce predictions
	for that frame, as well as adjust the trackers accordingly
	*/

	int64 start_time = 0;
	double cycle_time = 0.0;
	unsigned int trkNum = 0;
	unsigned int detNum = 0;
	vector<vector<double>> iouMatrix;
	vector<int> assignment;
	set<int> unmatchedDetections;
	set<int> unmatchedTrajectories;
	set<int> allItems;
	set<int> matchedItems;
	vector<cv::Point> matchedPairs;
	vector<TrackingBoxOld> frameTrackingResult;
	vector<Rect_<float>> predictedBoxes;

	double iouThreshold = iou;//Orignaly this value was 0.30


	// I used to count running time using clock(), but found it seems to conflict with cv::cvWaitkey(),
		// when they both exists, clock() can not get right result. Now I use cv::getTickCount() instead.
	start_time = getTickCount();

	//If this is the first frame we want to initialize everything and that is all.
	if (trackers.size() == 0) // the first frame met
	{
		// initialize kalman trackers using first detections.
		for (unsigned int i = 0; i < detFrameData.size(); i++)
		{
			//KalmanTracker trk = KalmanTracker(detFrameData[fi][i].box, process_mat, obser_mat);
			KalmanTracker trk = KalmanTracker(detFrameData[i].box);
			trackers.push_back(trk);
		}
		// output the first frame detections
		for (unsigned int id = 0; id < detFrameData.size(); id++)
		{
			TrackingBoxOld tb = detFrameData[id];
			resultsFile << tb.frame << "," << id + 1 << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
			//Save to results in swimmer_tracking.h

		}
		return;
	}


	///////////////////////////////////////
		// 3.1. get predicted locations from existing trackers.

	predictedBoxes.clear();

	for (auto it = trackers.begin(); it != trackers.end();)
	{
		Rect_<float> pBox = (*it).predict();
		if (pBox.x >= 0 && pBox.y >= 0)
		{
			predictedBoxes.push_back(pBox);
			it++;
		}
		else
		{
			it = trackers.erase(it);
			//cerr << "Box invalid at frame: " << frame_count << endl;
		}
	}



	///////////////////////////////////////
	// 3.2. associate detections to tracked object (both represented as bounding boxes)
	// dets : detFrameData[fi]
	trkNum = predictedBoxes.size();
	detNum = detFrameData.size();

	iouMatrix.clear();
	iouMatrix.resize(trkNum, vector<double>(detNum, 0));

	for (unsigned int i = 0; i < trkNum; i++) // compute iou matrix as a distance matrix
	{
		for (unsigned int j = 0; j < detNum; j++)
		{
			// use 1-iou because the hungarian algorithm computes a minimum-cost assignment.
			iouMatrix[i][j] = 1 - GetIOU(predictedBoxes[i], detFrameData[j].box);
		}
	}

	// solve the assignment problem using hungarian algorithm.
	// the resulting assignment is [track(prediction) : detection], with len=preNum
	HungarianAlgorithm HungAlgo;
	assignment.clear();
	HungAlgo.Solve(iouMatrix, assignment);

	// find matches, unmatched_detections and unmatched_predictions
	unmatchedTrajectories.clear();
	unmatchedDetections.clear();
	allItems.clear();
	matchedItems.clear();

	if (detNum > trkNum) //	there are unmatched detections
	{
		for (unsigned int n = 0; n < detNum; n++)
			allItems.insert(n);

		for (unsigned int i = 0; i < trkNum; ++i)
			matchedItems.insert(assignment[i]);

		set_difference(allItems.begin(), allItems.end(),
			matchedItems.begin(), matchedItems.end(),
			insert_iterator<set<int>>(unmatchedDetections, unmatchedDetections.begin()));
	}
	else if (detNum < trkNum) // there are unmatched trajectory/predictions
	{
		for (unsigned int i = 0; i < trkNum; ++i)
			if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
				unmatchedTrajectories.insert(i);
	}
	else
		;

	// filter out matched with low IOU
	matchedPairs.clear();
	for (unsigned int i = 0; i < trkNum; ++i)
	{
		if (assignment[i] == -1) // pass over invalid values
			continue;
		if (1 - iouMatrix[i][assignment[i]] < iouThreshold)
		{
			unmatchedTrajectories.insert(i);
			unmatchedDetections.insert(assignment[i]);
		}
		else
			matchedPairs.push_back(cv::Point(i, assignment[i]));
	}


	///////////////////////////////////////
	// 3.3. updating trackers

	// update matched trackers with assigned detections.
	// each prediction is corresponding to a tracker
	int detIdx, trkIdx;
	for (unsigned int i = 0; i < matchedPairs.size(); i++)
	{
		trkIdx = matchedPairs[i].x;
		detIdx = matchedPairs[i].y;
		trackers[trkIdx].update(detFrameData[detIdx].box);
	}

	// create and initialise new trackers for unmatched detections
	for (auto umd : unmatchedDetections)
	{
		//KalmanTracker tracker = KalmanTracker(detFrameData[fi][umd].box, process_mat, obser_mat);
		KalmanTracker tracker = KalmanTracker(detFrameData[umd].box);
		trackers.push_back(tracker);
	}

	// get trackers' output
	frameTrackingResult.clear();
	for (auto it = trackers.begin(); it != trackers.end();)
	{
		if (((*it).m_time_since_update < max_age) &&
			((*it).m_hit_streak >= min_hits || frame_count <= min_hits))
		{
			TrackingBoxOld res;
			res.box = (*it).get_state();
			res.id = (*it).m_id + 1;
			res.frame = frame_count;
			frameTrackingResult.push_back(res);
			it++;
		}
		else
			it++;

		// remove dead tracklet
		if (it != trackers.end() && (*it).m_time_since_update > max_age)
			it = trackers.erase(it);
	}

	cycle_time = (double)(getTickCount() - start_time);
	total_time += cycle_time / getTickFrequency();

	for (auto tb : frameTrackingResult) {
		resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
		//Save to results in swimmer_tracking.h

	}
}

void sort_tracker::sortTrackerUsingFunctions(string seqName, double iou)
{
	cout << "Processing " << seqName << "..." << endl;

	vector<TrackingBoxOld> detData;
	vector<vector<TrackingBoxOld>> detFrameData;
	
	int maxFrame = 0;

	getDataFromDetectionFile(seqName, detData);

	maxFrame = groupingDetectionData(detData, detFrameData);


	// 3. update across frames
	int frame_count = 0;
	int max_age = 1;//var for killing a tracker should be changed, in paper max_age = 1;
	int min_hits = 3;//min hits is min number of hits for it to be an object originaly min_hits = 3;
	//double iouThreshold = iou;//Orignaly this value was 0.30
	vector<KalmanTracker> trackers;
	KalmanTracker::kf_count = 0; // tracking id relies on this, so we have to reset it in each seq.

	// prepare result file.
	string resFileName = seqName;
	resFileName.replace(resFileName.end() - 4, resFileName.end(), "_det.txt");
	ofstream resultsFile;
	resultsFile.open(resFileName);

	if (!resultsFile.is_open())
	{
		cerr << "Error: can not create file " << resFileName << endl;
		return;
	}

	//////////////////////////////////////////////
	// main loop
	for (int fi = 0; fi < maxFrame; fi++)
	{
		total_frames++;
		frame_count++;
		//cout << frame_count << endl;

		//call pipeline function
		trackingForSingleFrame(trackers, detFrameData[fi], resultsFile, iou, max_age, min_hits, frame_count);
	}

	resultsFile.close();
}

vector<TrackingBoxOld> sort_tracker::trackingForSingleFrame(vector<KalmanTracker>& trackers, vector<TrackingBoxOld> detFrameData, double iou, int max_age, int min_hits, int frame_count)
{
	/*
This function will take trackers and data for a single frame (frame number fi) and produce predictions
for that frame, as well as adjust the trackers accordingly
*/

	int64 start_time = 0;
	double cycle_time = 0.0;
	unsigned int trkNum = 0;
	unsigned int detNum = 0;
	vector<vector<double>> iouMatrix;
	vector<int> assignment;
	set<int> unmatchedDetections;
	set<int> unmatchedTrajectories;
	set<int> allItems;
	set<int> matchedItems;
	vector<cv::Point> matchedPairs;
	vector<TrackingBoxOld> frameTrackingResult;
	vector<Rect_<float>> predictedBoxes;

	double iouThreshold = iou;//Orignaly this value was 0.30


	// I used to count running time using clock(), but found it seems to conflict with cv::cvWaitkey(),
		// when they both exists, clock() can not get right result. Now I use cv::getTickCount() instead.
	start_time = getTickCount();

	frameTrackingResult.clear();

	//If this is the first frame we want to initialize everything and that is all.
	if (trackers.size() == 0) // the first frame met
	{
		// initialize kalman trackers using first detections.
		cout << "first time" << endl;
		for (unsigned int i = 0; i < detFrameData.size(); i++)
		{
			//KalmanTracker trk = KalmanTracker(detFrameData[fi][i].box, process_mat, obser_mat);
			KalmanTracker trk = KalmanTracker(detFrameData[i].box);
			trackers.push_back(trk);
		}
		// output the first frame detections
		for (unsigned int id = 0; id < detFrameData.size(); id++)
		{
			TrackingBoxOld tb = detFrameData[id];
			//resultsFile << tb.frame << "," << id + 1 << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
			//Save to results in swimmer_tracking.h
			frameTrackingResult.push_back(tb);
			//return frameTrackingResult;
		}
		cout << "trackers size = " << trackers.size() << endl;
		return frameTrackingResult; 
	}


	///////////////////////////////////////
		// 3.1. get predicted locations from existing trackers.

	predictedBoxes.clear();

	for (auto it = trackers.begin(); it != trackers.end();)
	{
		Rect_<float> pBox = (*it).predict();
		if (pBox.x >= 0 && pBox.y >= 0)
		{
			predictedBoxes.push_back(pBox);
			it++;
		}
		else
		{
			it = trackers.erase(it);
			//cerr << "Box invalid at frame: " << frame_count << endl;
		}
	}



	///////////////////////////////////////
	// 3.2. associate detections to tracked object (both represented as bounding boxes)
	// dets : detFrameData[fi]
	trkNum = predictedBoxes.size();
	detNum = detFrameData.size();

	iouMatrix.clear();
	iouMatrix.resize(trkNum, vector<double>(detNum, 0));

	for (unsigned int i = 0; i < trkNum; i++) // compute iou matrix as a distance matrix
	{
		for (unsigned int j = 0; j < detNum; j++)
		{
			// use 1-iou because the hungarian algorithm computes a minimum-cost assignment.
			iouMatrix[i][j] = 1 - GetIOU(predictedBoxes[i], detFrameData[j].box);
		}
	}

	// solve the assignment problem using hungarian algorithm.
	// the resulting assignment is [track(prediction) : detection], with len=preNum
	HungarianAlgorithm HungAlgo;
	assignment.clear();
	HungAlgo.Solve(iouMatrix, assignment);

	// find matches, unmatched_detections and unmatched_predictions
	unmatchedTrajectories.clear();
	unmatchedDetections.clear();
	allItems.clear();
	matchedItems.clear();

	if (detNum > trkNum) //	there are unmatched detections
	{
		for (unsigned int n = 0; n < detNum; n++)
			allItems.insert(n);

		for (unsigned int i = 0; i < trkNum; ++i)
			matchedItems.insert(assignment[i]);

		set_difference(allItems.begin(), allItems.end(),
			matchedItems.begin(), matchedItems.end(),
			insert_iterator<set<int>>(unmatchedDetections, unmatchedDetections.begin()));
	}
	else if (detNum < trkNum) // there are unmatched trajectory/predictions
	{
		for (unsigned int i = 0; i < trkNum; ++i)
			if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
				unmatchedTrajectories.insert(i);
	}
	else
		;

	// filter out matched with low IOU
	matchedPairs.clear();
	for (unsigned int i = 0; i < trkNum; ++i)
	{
		if (assignment[i] == -1) // pass over invalid values
			continue;
		if (1 - iouMatrix[i][assignment[i]] < iouThreshold)
		{
			unmatchedTrajectories.insert(i);
			unmatchedDetections.insert(assignment[i]);
		}
		else
			matchedPairs.push_back(cv::Point(i, assignment[i]));
	}


	///////////////////////////////////////
	// 3.3. updating trackers

	// update matched trackers with assigned detections.
	// each prediction is corresponding to a tracker
	int detIdx, trkIdx;
	for (unsigned int i = 0; i < matchedPairs.size(); i++)
	{
		trkIdx = matchedPairs[i].x;
		detIdx = matchedPairs[i].y;
		trackers[trkIdx].update(detFrameData[detIdx].box);
	}

	// create and initialise new trackers for unmatched detections
	for (auto umd : unmatchedDetections)
	{
		//KalmanTracker tracker = KalmanTracker(detFrameData[fi][umd].box, process_mat, obser_mat);
		KalmanTracker tracker = KalmanTracker(detFrameData[umd].box);
		trackers.push_back(tracker);
	}

	// get trackers' output
	frameTrackingResult.clear();
	for (auto it = trackers.begin(); it != trackers.end();)
	{
		if (((*it).m_time_since_update < max_age) &&
			((*it).m_hit_streak >= min_hits || frame_count <= min_hits))
		{
			TrackingBoxOld res;
			res.box = (*it).get_state();
			res.id = (*it).m_id + 1;
			res.frame = frame_count;
			frameTrackingResult.push_back(res);
			it++;
		}
		else
			it++;

		// remove dead tracklet
		if (it != trackers.end() && (*it).m_time_since_update > max_age)
			it = trackers.erase(it);
	}

	cycle_time = (double)(getTickCount() - start_time);
	total_time += cycle_time / getTickFrequency();
	/*
	for (auto tb : frameTrackingResult) {
		resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
		//Save to results in swimmer_tracking.h

	}
	*/
	return frameTrackingResult;

	//return vector<vector<TrackingBox>>();
}

void sort_tracker::sortTrackerPipelined(string outputFileName, double iou, vector<TrackingBoxOld> detData)
{
	/*
	This function will perform SORT on a single frame, whose detections are present in detData
	Assumption: detData contains only the information of a single frame, and contains all the information for that frame

	*/

	// 3. update across frames
	int frame_count = 0;
	int max_age = 1;//var for killing a tracker should be changed, in paper max_age = 1;
	int min_hits = 3;//min hits is min number of hits for it to be an object originaly min_hits = 3;
	//double iouThreshold = iou;//Orignaly this value was 0.30
	vector<KalmanTracker> trackers;
	KalmanTracker::kf_count = 0; // tracking id relies on this, so we have to reset it in each seq.

	// prepare result file.
	string resFileName = outputFileName;
	resFileName.replace(resFileName.end() - 4, resFileName.end(), "_det.txt");
	ofstream resultsFile;
	resultsFile.open(resFileName);

	if (!resultsFile.is_open())
	{
		cerr << "Error: can not create file " << resFileName << endl;
		return;
	}

	//total_frames++;
		//cout << frame_count << endl;

	//call pipeline function
	trackingForSingleFrame(trackers, detData, resultsFile, iou, max_age, min_hits, frame_count);

	resultsFile.close();

}

vector<TrackingBoxOld> sort_tracker::sortTrackerPipelined(double iou, vector<TrackingBoxOld> detData, vector<KalmanTracker>& trackers)
{
	/*
	This function will perform SORT on a single frame, whose detections are present in detData
	Assumption: detData contains only the information of a single frame, and contains all the information for that frame
	This function will not output to a file, and instead return the tracking in a vector.
	*/

	int frame_count = 0;
	int max_age = 1;//var for killing a tracker should be changed, in paper max_age = 1;
	int min_hits = 3;//min hits is min number of hits for it to be an object originaly min_hits = 3;
	//double iouThreshold = iou;//Orignaly this value was 0.30
	//vector<KalmanTracker> trackers; //TODO is there a better way
	KalmanTracker::kf_count = 0; // tracking id relies on this, so we have to reset it in each seq.

	vector<TrackingBoxOld> trackedObjects; //This will hold the results
	trackedObjects.clear();

	//call pipeline function
	trackedObjects = trackingForSingleFrame(trackers, detData, iou, max_age, min_hits, frame_count);

	return trackedObjects;
}

void sort_tracker::sortWithFunctionsTest(string seqName, double iou)
{
	cout << "Processing " << seqName << "..." << endl;

	vector<TrackingBoxOld> detData;
	vector<vector<TrackingBoxOld>> detFrameData;

	int maxFrame = 0;

	getDataFromDetectionFile(seqName, detData);

	maxFrame = groupingDetectionData(detData, detFrameData);


	// 3. update across frames
	int frame_count = 0;
	int max_age = 1;//var for killing a tracker should be changed, in paper max_age = 1;
	int min_hits = 3;//min hits is min number of hits for it to be an object originaly min_hits = 3;
	//double iouThreshold = iou;//Orignaly this value was 0.30
	vector<KalmanTracker> trackers;
	KalmanTracker::kf_count = 0; // tracking id relies on this, so we have to reset it in each seq.

	// prepare result file.
	string resFileName = seqName;
	resFileName.replace(resFileName.end() - 4, resFileName.end(), "_det.txt");
	ofstream resultsFile;
	resultsFile.open(resFileName);

	if (!resultsFile.is_open())
	{
		cerr << "Error: can not create file " << resFileName << endl;
		return;
	}

	//////////////////////////////////////////////
	// main loop
	vector<TrackingBoxOld> resultsTracking;
	resultsTracking.clear();

	for (int fi = 0; fi < maxFrame; fi++)
	{
		total_frames++;
		frame_count++;
		//cout << frame_count << endl;

		//call pipeline function
		//trackingForSingleFrame(trackers, detFrameData[fi], resultsFile, iou, max_age, min_hits, frame_count);
		
		
		//resultsTracking = (sortTrackerPipelined(iou, detFrameData[fi], trackers));

		//for (auto tb : resultsTracking) {
		//	resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
			//Save to results in swimmer_tracking.h
		//}
		//resultsTracking.clear();
		vector<TrackingBoxOld> temp = (sortTrackerPipelined(iou, detFrameData[fi], trackers));
		resultsTracking.insert(resultsTracking.end(), temp.begin(), temp.end());
	}

	for (auto tb : resultsTracking) {
			resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
			//Save to results in swimmer_tracking.h
	}

	resultsFile.close();
}

void sort_tracker::sortOnFrame(string seqName, double iou)
{
	cout << "Processing " << seqName << "..." << endl;

	vector<TrackingBoxOld> detData;
	vector<vector<TrackingBoxOld>> detFrameData;

	int maxFrame = 0;

	getDataFromDetectionFile(seqName, detData);

	maxFrame = groupingDetectionData(detData, detFrameData);


	// 3. update across frames
	int frame_count = 0;
	int max_age = 1;//var for killing a tracker should be changed, in paper max_age = 1;
	int min_hits = 3;//min hits is min number of hits for it to be an object originaly min_hits = 3;
	//double iouThreshold = iou;//Orignaly this value was 0.30
	vector<KalmanTracker> trackers;
	KalmanTracker::kf_count = 0; // tracking id relies on this, so we have to reset it in each seq.

	// prepare result file.
	string resFileName = seqName;
	resFileName.replace(resFileName.end() - 4, resFileName.end(), "_det.txt");
	ofstream resultsFile;
	resultsFile.open(resFileName);

	if (!resultsFile.is_open())
	{
		cerr << "Error: can not create file " << resFileName << endl;
		return;
	}

	vector<TrackingBoxOld> tempResults;
	tempResults.clear();
	//////////////////////////////////////////////
	// main loop
	for (int fi = 0; fi < maxFrame; fi++)
	{
		total_frames++;
		frame_count++;
		tempResults.clear();
		//cout << frame_count << endl;

		//call pipeline function
		tempResults = singleFrameSORT(trackers, detFrameData[fi], iou, max_age, min_hits, frame_count);

		for (auto tb : tempResults) {
			resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
		}
		//for (auto tb : frameTrackingResult) {
//	resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
	//Save to results in swimmer_tracking.h

//}


					//resultsFile << tb.frame << "," << id + 1 << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;

	}

	resultsFile.close();
}

vector<TrackingBoxOld> sort_tracker::singleFrameSORT(vector<KalmanTracker>& trackers, vector<TrackingBoxOld> detFrameData, double iou, int max_age, int min_hits, int frame_count)
{
	/*
This function will take trackers and data for a single frame (frame number fi) and produce predictions
for that frame, as well as adjust the trackers accordingly
*/

	int64 start_time = 0;
	double cycle_time = 0.0;
	unsigned int trkNum = 0;
	unsigned int detNum = 0;
	vector<vector<double>> iouMatrix;
	vector<int> assignment;
	set<int> unmatchedDetections;
	set<int> unmatchedTrajectories;
	set<int> allItems;
	set<int> matchedItems;
	vector<cv::Point> matchedPairs;
	vector<TrackingBoxOld> frameTrackingResult;
	vector<Rect_<float>> predictedBoxes;

	double iouThreshold = iou;//Orignaly this value was 0.30

	frameTrackingResult.clear();

	// I used to count running time using clock(), but found it seems to conflict with cv::cvWaitkey(),
		// when they both exists, clock() can not get right result. Now I use cv::getTickCount() instead.
	start_time = getTickCount();

	//If this is the first frame we want to initialize everything and that is all.
	if (trackers.size() == 0) // the first frame met
	{
		// initialize kalman trackers using first detections.
		for (unsigned int i = 0; i < detFrameData.size(); i++)
		{
			//KalmanTracker trk = KalmanTracker(detFrameData[fi][i].box, process_mat, obser_mat);
			KalmanTracker trk = KalmanTracker(detFrameData[i].box);
			trackers.push_back(trk);
		}
		// output the first frame detections
		for (unsigned int id = 0; id < detFrameData.size(); id++)
		{
			TrackingBoxOld tb = detFrameData[id];
			//resultsFile << tb.frame << "," << id + 1 << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
			//Save to results in swimmer_tracking.h

			tb.id = id + 1;
			frameTrackingResult.push_back(tb);
		}
		return frameTrackingResult;
	}


	///////////////////////////////////////
		// 3.1. get predicted locations from existing trackers.

	predictedBoxes.clear();

	for (auto it = trackers.begin(); it != trackers.end();)
	{
		Rect_<float> pBox = (*it).predict();
		if (pBox.x >= 0 && pBox.y >= 0)
		{
			predictedBoxes.push_back(pBox);
			it++;
		}
		else
		{
			it = trackers.erase(it);
			//cerr << "Box invalid at frame: " << frame_count << endl;
		}
	}



	///////////////////////////////////////
	// 3.2. associate detections to tracked object (both represented as bounding boxes)
	// dets : detFrameData[fi]
	trkNum = predictedBoxes.size();
	detNum = detFrameData.size();

	iouMatrix.clear();
	iouMatrix.resize(trkNum, vector<double>(detNum, 0));

	for (unsigned int i = 0; i < trkNum; i++) // compute iou matrix as a distance matrix
	{
		for (unsigned int j = 0; j < detNum; j++)
		{
			// use 1-iou because the hungarian algorithm computes a minimum-cost assignment.
			iouMatrix[i][j] = 1 - GetIOU(predictedBoxes[i], detFrameData[j].box);
		}
	}

	// solve the assignment problem using hungarian algorithm.
	// the resulting assignment is [track(prediction) : detection], with len=preNum
	HungarianAlgorithm HungAlgo;
	assignment.clear();
	HungAlgo.Solve(iouMatrix, assignment);

	// find matches, unmatched_detections and unmatched_predictions
	unmatchedTrajectories.clear();
	unmatchedDetections.clear();
	allItems.clear();
	matchedItems.clear();

	if (detNum > trkNum) //	there are unmatched detections
	{
		for (unsigned int n = 0; n < detNum; n++)
			allItems.insert(n);

		for (unsigned int i = 0; i < trkNum; ++i)
			matchedItems.insert(assignment[i]);

		set_difference(allItems.begin(), allItems.end(),
			matchedItems.begin(), matchedItems.end(),
			insert_iterator<set<int>>(unmatchedDetections, unmatchedDetections.begin()));
	}
	else if (detNum < trkNum) // there are unmatched trajectory/predictions
	{
		for (unsigned int i = 0; i < trkNum; ++i)
			if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
				unmatchedTrajectories.insert(i);
	}
	else
		;

	// filter out matched with low IOU
	matchedPairs.clear();
	for (unsigned int i = 0; i < trkNum; ++i)
	{
		if (assignment[i] == -1) // pass over invalid values
			continue;
		if (1 - iouMatrix[i][assignment[i]] < iouThreshold)
		{
			unmatchedTrajectories.insert(i);
			unmatchedDetections.insert(assignment[i]);
		}
		else
			matchedPairs.push_back(cv::Point(i, assignment[i]));
	}


	///////////////////////////////////////
	// 3.3. updating trackers

	// update matched trackers with assigned detections.
	// each prediction is corresponding to a tracker
	int detIdx, trkIdx;
	for (unsigned int i = 0; i < matchedPairs.size(); i++)
	{
		trkIdx = matchedPairs[i].x;
		detIdx = matchedPairs[i].y;
		trackers[trkIdx].update(detFrameData[detIdx].box);
	}

	// create and initialise new trackers for unmatched detections
	for (auto umd : unmatchedDetections)
	{
		//KalmanTracker tracker = KalmanTracker(detFrameData[fi][umd].box, process_mat, obser_mat);
		KalmanTracker tracker = KalmanTracker(detFrameData[umd].box);
		trackers.push_back(tracker);
	}

	// get trackers' output
	frameTrackingResult.clear();
	for (auto it = trackers.begin(); it != trackers.end();)
	{
		if (((*it).m_time_since_update < max_age) &&
			((*it).m_hit_streak >= min_hits || frame_count <= min_hits))
		{
			TrackingBoxOld res;
			res.box = (*it).get_state();
			res.id = (*it).m_id + 1;
			res.frame = frame_count;
			frameTrackingResult.push_back(res);
			it++;
		}
		else
			it++;

		// remove dead tracklet
		if (it != trackers.end() && (*it).m_time_since_update > max_age)
			it = trackers.erase(it);
	}

	cycle_time = (double)(getTickCount() - start_time);
	total_time += cycle_time / getTickFrequency();

	//for (auto tb : frameTrackingResult) {
	//	resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
		//Save to results in swimmer_tracking.h

	//}
	return frameTrackingResult;
	//return vector<TrackingBox>();
}


