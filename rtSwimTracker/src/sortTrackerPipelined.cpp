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
	m_numDetections = swimmerDetections.size();
	m_frameTrackingResult.clear();

	//If this is the first frame we want to initialize everything and that is all.
	if (vectorsOfTrackers().size() == 0) // the first frame met
	{
		initializeTrackerUsing(swimmerDetections);
		fillFrameTrackingResultsWith(swimmerDetections);
	}
	else 
	{
		makeKalmanPredictions();
		associatePredictionsWith(swimmerDetections);
		updateKalmanTrackers(swimmerDetections);
		createNewKalmanTrackers(swimmerDetections);
		collectResultsWhileKillingTrackers();
	}

	return m_frameTrackingResult;
}


void sortTrackerPiplelined::initializeTrackerUsing(const vector<TrackingBox>& detFrameData)
{
	for (unsigned int i = 0; i < detFrameData.size(); i++)
	{
		KalmanTracker trk = KalmanTracker(detFrameData[i].box);
		vectorsOfTrackers().push_back(trk);
	}
}


void sortTrackerPiplelined::fillFrameTrackingResultsWith(const vector<TrackingBox>& detFrameData)
{
	for (unsigned int id = 0; id < detFrameData.size(); id++)
	{
		TrackingBox tb = detFrameData[id];
		tb.id = id + 1;
		m_frameTrackingResult.push_back(tb);
	}
}


void sortTrackerPiplelined::makeKalmanPredictions()
{
	m_trajectoryPredictions.clear();

	for (auto it = vectorsOfTrackers().begin(); it != vectorsOfTrackers().end();)
	{
		Rect_<float> pBox = (*it).predict();
		if (pBox.x >= 0 && pBox.y >= 0)
		{
			m_trajectoryPredictions.push_back(pBox);
			it++;
		}
		else
		{
			it = vectorsOfTrackers().erase(it);
		}
	}
	m_numTrajectories = m_trajectoryPredictions.size();
}


void sortTrackerPiplelined::associatePredictionsWith(const vector<TrackingBox>& detFrameData)
{
	vector<vector<double>> iouCostMatrix;
	vector<int> assignments;
	HungarianAlgorithm HungAlgo;

	iouCostMatrix = constructIOUmat(detFrameData);
	HungAlgo.Solve(iouCostMatrix, assignments);

	fillUnmatchedDetections(assignments);
	fillUnmatchedTrajectories(assignments);
	fillMatchedPairs(assignments, iouCostMatrix);
}

void sortTrackerPiplelined::collectResultsWhileKillingTrackers()
{
	bool missedTooManyTimes, isOldEnough, lessThanMinHitsHasBeenProcessed;
	lessThanMinHitsHasBeenProcessed = m_numberFramesProcessed <= m_minHitsInFrames;

	for (auto it = vectorsOfTrackers().begin(); it != vectorsOfTrackers().end();)
	{
		missedTooManyTimes = it->m_time_since_update > m_maxAgeInFrames;
		isOldEnough = it->m_hit_streak >= m_minHitsInFrames;

		if (missedTooManyTimes)
		{
			it = vectorsOfTrackers().erase(it);
		}
		else if (isOldEnough || lessThanMinHitsHasBeenProcessed)
		{
			TrackingBox res;
			res.box = it->get_state();
			res.id = it->m_id + 1;
			res.frame = m_numberFramesProcessed;
			m_frameTrackingResult.push_back(res);
			it++;
		}
		else
			it++;
	}

	/*old version of the algorithm... does not seem right

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
	*/
}

void sortTrackerPiplelined::updateKalmanTrackers(const vector<TrackingBox>& detFrameData)
{
	int detIdx, trkIdx;
	for (unsigned int i = 0; i < m_matchedPairs.size(); i++)
	{
		trkIdx = m_matchedPairs[i].x;
		detIdx = m_matchedPairs[i].y;
		vectorsOfTrackers()[trkIdx].update(detFrameData[detIdx].box);
	}
}

void sortTrackerPiplelined::createNewKalmanTrackers(const vector<TrackingBox>& detFrameData)
{
	for (auto umd : m_unmatchedDetections)
	{
		KalmanTracker tracker = KalmanTracker(detFrameData[umd].box);
		vectorsOfTrackers().push_back(tracker);
	}
}

vector<vector<double>> sortTrackerPiplelined::constructIOUmat(const vector<TrackingBox>& detFrameData)
{
	unsigned int numTrajectories = m_trajectoryPredictions.size();
	unsigned int numDetections = detFrameData.size();
	vector<vector<double>> iouCostMatrix;

	iouCostMatrix.resize(numTrajectories, vector<double>(numDetections, 0));

	for (unsigned int i = 0; i < numTrajectories; i++)
	{
		for (unsigned int j = 0; j < numDetections; j++)
		{
			// use 1-iou because the Hungarian algorithm computes a minimum-cost assignment.
			iouCostMatrix[i][j] = 1 - GetIOU(m_trajectoryPredictions[i], detFrameData[j].box);
		}
	}

	return iouCostMatrix;
}

void sortTrackerPiplelined::fillUnmatchedDetections(vector<int> assignments)
{
	set<int> allItems;
	set<int> matchedItems;

	m_unmatchedDetections.clear();

	if (m_numDetections > m_numTrajectories)
	{
		for (unsigned int n = 0; n < m_numDetections; n++)
			allItems.insert(n);

		for (unsigned int i = 0; i < m_numTrajectories; ++i)
			matchedItems.insert(assignments[i]);

		set_difference(allItems.begin(), allItems.end(),
			matchedItems.begin(), matchedItems.end(),
			insert_iterator<set<int>>(m_unmatchedDetections, m_unmatchedDetections.begin()));
	}
}

void sortTrackerPiplelined::fillUnmatchedTrajectories(vector<int> assignments)
{
	if (m_numDetections < m_numTrajectories)
	{
		for (unsigned int i = 0; i < m_numTrajectories; ++i)
			if (assignments[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
				m_unmatchedTrajectories.insert(i);
	}
}

void sortTrackerPiplelined::fillMatchedPairs(const vector<int>& assignment, const vector<vector<double>>& iouCostMatrix)
{
	// filter out matched with low IOU
	m_matchedPairs.clear();

	for (unsigned int i = 0; i < m_numTrajectories; ++i)
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


double sortTrackerPiplelined::GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt)
{
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}