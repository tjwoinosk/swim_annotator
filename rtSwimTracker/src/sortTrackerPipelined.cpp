#include "sortTrackerPipelined.h";

sortTrackerPiplelined::sortTrackerPiplelined()
{
	frameCounter() = 0;
	vectorsOfTrackers().clear();
}

void sortTrackerPiplelined::sortOnFrame(string seqName)
{
	cout << "Processing " << seqName << "..." << endl;

	vector<TrackingBox> detData;
	vector<vector<TrackingBox>> detFrameData;

	int maxFrame = 0;
	getDataFromDetectionFile(seqName, detData);
	maxFrame = groupingDetectionData(detData, detFrameData);


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

	vector<TrackingBox> tempResults;
	tempResults.clear();

	for (int fi = 0; fi < maxFrame; fi++)
	{
		total_frames++;
		frameCounter()++;
		tempResults.clear();

		tempResults = singleFrameSORT(detFrameData[fi]);

		for (auto tb : tempResults) 
		{
			resultsFile << tb;
		}
	}

	resultsFile.close();
}


/*
This function will take trackers and data for a single frame (frame number fi) and produce predictions
for that frame, as well as adjust the trackers accordingly
*/
vector<TrackingBox> sortTrackerPiplelined::singleFrameSORT(vector<TrackingBox> detFrameData)
{
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

	double iouThreshold = iou;//Orignaly this value was 0.30

	frameTrackingResult.clear();


	//If this is the first frame we want to initialize everything and that is all.
	if (vectorsOfTrackers().size() == 0) // the first frame met
	{
		// initialize kalman trackers using first detections.
		for (unsigned int i = 0; i < detFrameData.size(); i++)
		{
			//KalmanTracker trk = KalmanTracker(detFrameData[fi][i].box, process_mat, obser_mat);
			KalmanTracker trk = KalmanTracker(detFrameData[i].box);
			vectorsOfTrackers().push_back(trk);
		}
		// output the first frame detections
		for (unsigned int id = 0; id < detFrameData.size(); id++)
		{
			TrackingBox tb = detFrameData[id];
			//resultsFile << tb.frame << "," << id + 1 << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;
			//Save to results in swimmer_tracking.h

			tb.id = id + 1; //TODO should this be dealt in another way?
			frameTrackingResult.push_back(tb);
		}
		return frameTrackingResult;
	}


	///////////////////////////////////////
		// 3.1. get predicted locations from existing trackers.

	predictedBoxes.clear();

	for (auto it = vectorsOfTrackers().begin(); it != vectorsOfTrackers().end();)
	{
		Rect_<float> pBox = (*it).predict();
		if (pBox.x >= 0 && pBox.y >= 0)
		{
			predictedBoxes.push_back(pBox);
			it++;
		}
		else
		{
			it = vectorsOfTrackers().erase(it);
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
		vectorsOfTrackers()[trkIdx].update(detFrameData[detIdx].box);
	}

	// create and initialise new trackers for unmatched detections
	for (auto umd : unmatchedDetections)
	{
		//KalmanTracker tracker = KalmanTracker(detFrameData[fi][umd].box, process_mat, obser_mat);
		KalmanTracker tracker = KalmanTracker(detFrameData[umd].box);
		vectorsOfTrackers().push_back(tracker);
	}

	// get trackers' output
	frameTrackingResult.clear();
	for (auto it = vectorsOfTrackers().begin(); it != vectorsOfTrackers().end();)
	{
		if (((*it).m_time_since_update < max_age) &&
			((*it).m_hit_streak >= min_hits || frameCounter() <= min_hits))
		{
			TrackingBox res;
			res.box = (*it).get_state();
			res.id = (*it).m_id + 1;
			res.frame = frameCounter();
			frameTrackingResult.push_back(res);
			it++;
		}
		else
			it++;

		// remove dead tracklet
		if (it != vectorsOfTrackers().end() && (*it).m_time_since_update > max_age)
			it = vectorsOfTrackers().erase(it);
	}

	return frameTrackingResult;
}


/*
The purpose of this function is to read in the file whose name is speciied by the input detFileName
and put the information of this file into a vector of TrackingBoxes, which is the argument detData
*/
void sortTrackerPiplelined::getDataFromDetectionFile(string detFileName, vector<TrackingBox>& detData)
{
	string detLine;
	istringstream ss;
	ifstream detectionFile;

	detectionFile.open(detFileName);
	if (!detectionFile.is_open())
	{
		cerr << "Error: can not find file " << detFileName << endl;
		return;
	}

	TrackingBox tb;
	while (getline(detectionFile, detLine))
	{
		ss.str(detLine);
		ss >> tb;
		detData.push_back(tb);
	}
	detectionFile.close();

	return;
}


/*
This function takes an input detData that is all the detection data stored in a vector of TrackingBox
and then grouping TrackingBoxes for a single frame into a vector, and storing this vector into another
vector which is the input detFrameData.
The result is a 2D array like vector where each element points to a vector of TrackingBoxes of the same frame.
*/
int sortTrackerPiplelined::groupingDetectionData(vector<TrackingBox> detData, vector<vector<TrackingBox>>& detFrameData)
{
	int maxFrame = 0;
	vector<TrackingBox> tempVec;

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


double sortTrackerPiplelined::GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt)
{
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}