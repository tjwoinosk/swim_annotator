#include "sortTrackerPipelined.h";

sortTrackerPiplelined::sortTrackerPiplelined()
{
	numberFramesProcessed = 0;
	vectorsOfTrackers().clear();
}


/*
This function will take trackers and data for a single frame (frame number fi) and produce predictions
for that frame, as well as adjust the trackers accordingly
*/
vector<TrackingBox> sortTrackerPiplelined::singleFrameSORT(vector<TrackingBox> detFrameData)
{
	numberFramesProcessed++;

	frameTrackingResult.clear();
	//If this is the first frame we want to initialize everything and that is all.
	if (vectorsOfTrackers().size() == 0) // the first frame met
	{
		initializeTracker(detFrameData);
		return frameTrackingResult;
	}

	getPredictions();
	assoicateDetections(detFrameData);
	updateTrackers(detFrameData);

	return frameTrackingResult;
}

void sortTrackerPiplelined::initializeTracker(const vector<TrackingBox>& detFrameData)
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
}


void sortTrackerPiplelined::getPredictions()
{
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

}

void sortTrackerPiplelined::assoicateDetections(const vector<TrackingBox>& detFrameData)
{
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
		if (1 - iouMatrix[i][assignment[i]] < iou)
		{
			unmatchedTrajectories.insert(i);
			unmatchedDetections.insert(assignment[i]);
		}
		else
			matchedPairs.push_back(cv::Point(i, assignment[i]));
	}

}

void sortTrackerPiplelined::updateTrackers(const vector<TrackingBox>& detFrameData) 
{
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
			((*it).m_hit_streak >= min_hits || numberFramesProcessed <= min_hits))
		{
			TrackingBox res;
			res.box = (*it).get_state();
			res.id = (*it).m_id + 1;
			res.frame = numberFramesProcessed;
			frameTrackingResult.push_back(res);
			it++;
		}
		else
			it++;

		// remove dead tracklet
		if (it != vectorsOfTrackers().end() && (*it).m_time_since_update > max_age)
			it = vectorsOfTrackers().erase(it);
	}
}





double sortTrackerPiplelined::GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt)
{
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}