#include "sortTrackerPipelined.h";

sortTrackerPiplelined::sortTrackerPiplelined()
{
	m_numberFramesProcessed = 0;
	vectorsOfTrackers().clear();
}


/*
This function will take trackers and data for a single frame (frame number fi) and produce predictions
for that frame, as well as adjust the trackers accordingly
*/
vector<TrackingBox> sortTrackerPiplelined::singleFrameSORT(const vector<TrackingBox>& swimmerDetections)
{
	m_numberFramesProcessed++;

	m_frameTrackingResult.clear();
	//If this is the first frame we want to initialize everything and that is all.
	if (vectorsOfTrackers().size() == 0) // the first frame met
	{
		initializeTracker(swimmerDetections);
		fillFrameTrackingResultsWith(swimmerDetections);
	}
	else 
	{
		makeKalmanPredictions();
		associatePredictionsWith(swimmerDetections);
		updateTrackers(swimmerDetections);
	}

	return m_frameTrackingResult;
}


void sortTrackerPiplelined::initializeTracker(const vector<TrackingBox>& detFrameData)
{
	// initialize kalman trackers using first detections.
	for (unsigned int i = 0; i < detFrameData.size(); i++)
	{
		KalmanTracker trk = KalmanTracker(detFrameData[i].box);
		vectorsOfTrackers().push_back(trk);
	}
}


void sortTrackerPiplelined::fillFrameTrackingResultsWith(const vector<TrackingBox>& detFrameData)
{
	// output the first frame detections
	for (unsigned int id = 0; id < detFrameData.size(); id++)
	{
		TrackingBox tb = detFrameData[id];
		tb.id = id + 1;
		m_frameTrackingResult.push_back(tb);
	}
}


void sortTrackerPiplelined::makeKalmanPredictions()
{
	m_predictedBoxes.clear();

	for (auto it = vectorsOfTrackers().begin(); it != vectorsOfTrackers().end();)
	{
		Rect_<float> pBox = (*it).predict();
		if (pBox.x >= 0 && pBox.y >= 0)
		{
			m_predictedBoxes.push_back(pBox);
			it++;
		}
		else
		{
			it = vectorsOfTrackers().erase(it);
		}
	}

}


void sortTrackerPiplelined::associatePredictionsWith(const vector<TrackingBox>& detFrameData)
{
	vector<vector<double>> iouCostMatrix;
	constructIOUmat(iouCostMatrix, detFrameData);
	solveIOUassignmentProblem(iouCostMatrix);
}

void sortTrackerPiplelined::constructIOUmat(vector<vector<double>>& blankMat, const vector<TrackingBox>& detFrameData)
{
	unsigned int trkNum = m_predictedBoxes.size();
	unsigned int detNum = detFrameData.size();
	vector<vector<double>> iouCostMatrix;

	iouCostMatrix.resize(trkNum, vector<double>(detNum, 0));

	for (unsigned int i = 0; i < trkNum; i++)
	{
		for (unsigned int j = 0; j < detNum; j++)
		{
			// use 1-iou because the Hungarian algorithm computes a minimum-cost assignment.
			iouCostMatrix[i][j] = 1 - GetIOU(m_predictedBoxes[i], detFrameData[j].box);
		}
	}

	blankMat = iouCostMatrix;
}

// solve the assignment problem using hungarian algorithm.
// the resulting assignment is [track(prediction) : detection], with len=preNum
void sortTrackerPiplelined::solveIOUassignmentProblem(vector<vector<double>> iouCostMatrix)
{
	if (iouCostMatrix.size() < 1) return;

	unsigned int trkNum = iouCostMatrix.size();
	unsigned int detNum = iouCostMatrix[0].size();
	set<int> allItems; //local
	set<int> matchedItems; //local
	HungarianAlgorithm HungAlgo;

	vector<int> assignment;
	HungAlgo.Solve(iouCostMatrix, assignment);

	// find matches, unmatched_detections and unmatched_predictions

	m_unmatchedTrajectories.clear();
	m_unmatchedDetections.clear();

	if (detNum > trkNum) //	there are unmatched detections
	{
		for (unsigned int n = 0; n < detNum; n++)
			allItems.insert(n);

		for (unsigned int i = 0; i < trkNum; ++i)
			matchedItems.insert(assignment[i]);

		set_difference(allItems.begin(), allItems.end(),
			matchedItems.begin(), matchedItems.end(),
			insert_iterator<set<int>>(m_unmatchedDetections, m_unmatchedDetections.begin()));
	}
	else if (detNum < trkNum) // there are unmatched trajectory/predictions
	{
		for (unsigned int i = 0; i < trkNum; ++i)
			if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
				m_unmatchedTrajectories.insert(i);
	}
	else
		;

	// filter out matched with low IOU
	m_matchedPairs.clear();
	for (unsigned int i = 0; i < trkNum; ++i)
	{
		if (assignment[i] == -1) // pass over invalid values
			continue;
		if (1 - iouCostMatrix[i][assignment[i]] < m_iou)
		{
			m_unmatchedTrajectories.insert(i);
			m_unmatchedDetections.insert(assignment[i]);
		}
		else
			m_matchedPairs.push_back(cv::Point(i, assignment[i]));
	}

}



void sortTrackerPiplelined::updateTrackers(const vector<TrackingBox>& detFrameData) 
{
	///////////////////////////////////////
// 3.3. updating trackers

// update matched trackers with assigned detections.
// each prediction is corresponding to a tracker
	int detIdx, trkIdx;
	for (unsigned int i = 0; i < m_matchedPairs.size(); i++)
	{
		trkIdx = m_matchedPairs[i].x;
		detIdx = m_matchedPairs[i].y;
		vectorsOfTrackers()[trkIdx].update(detFrameData[detIdx].box);
	}

	// create and initialise new trackers for unmatched detections
	for (auto umd : m_unmatchedDetections)
	{
		//KalmanTracker tracker = KalmanTracker(detFrameData[fi][umd].box, process_mat, obser_mat);
		KalmanTracker tracker = KalmanTracker(detFrameData[umd].box);
		vectorsOfTrackers().push_back(tracker);
	}

	// get trackers' output
	m_frameTrackingResult.clear();
	for (auto it = vectorsOfTrackers().begin(); it != vectorsOfTrackers().end();)
	{
		if (((*it).m_time_since_update < m_maxAge) &&
			((*it).m_hit_streak >= m_minHits || m_numberFramesProcessed <= m_minHits))
		{
			TrackingBox res;
			res.box = (*it).get_state();
			res.id = (*it).m_id + 1;
			res.frame = m_numberFramesProcessed;
			m_frameTrackingResult.push_back(res);
			it++;
		}
		else
			it++;

		// remove dead tracklet
		if (it != vectorsOfTrackers().end() && (*it).m_time_since_update > m_maxAge)
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