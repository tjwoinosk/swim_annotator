#include "sortTrackerPipelined.h";

sortTrackerPiplelined::sortTrackerPiplelined()
{
	m_numberFramesProcessed = 0;
}


/*
This function will take trackers and data for a single frame (frame number fi) and produce predictions
for that frame, as well as adjust the trackers accordingly
*/
vector<TrackingBox> sortTrackerPiplelined::singleFrameSORT(const vector<TrackingBox>& swimmerDetections)
{
	m_frameTrackingResult.clear();
	inputDetectionData(swimmerDetections);

	processFrame();
	
	return m_frameTrackingResult;
}


void sortTrackerPiplelined::initializeTrackersUsing(const vector<TrackingBox>& detFrameData)
{
	for (unsigned int i = 0; i < detFrameData.size(); i++)
	{
		KalmanTracker trk = KalmanTracker(detFrameData[i].box);
		m_vectorsOfTrackers.push_back(trk);
	}
}


void sortTrackerPiplelined::fillResultsWithDetections()
{
	for (unsigned int id = 0; id < m_frameData.size(); id++)
	{
		TrackingBox tb = m_frameData[id];
		tb.id = id + 1;
		m_frameTrackingResult.push_back(tb);
	}
}

void sortTrackerPiplelined::processFrame()
{
	vector<vector<double>> iouCostMatrix;
	vector<Rect_<float>> trajectoryPredictions;
	vector<cv::Point> pairs;

	m_numberFramesProcessed++;
	if (m_numberFramesProcessed == 0 || m_vectorsOfTrackers.size() == 0)
	{
		initializeTrackersUsing(m_frameData);
		fillResultsWithDetections();
	}
	else
	{
		trajectoryPredictions = createTrajecotoryPredictions();
		iouCostMatrix = constructIOUmat(trajectoryPredictions);
		pairs = matchDetectionsToTrajectories(iouCostMatrix);
		updateTrackers(pairs);
		createNewTrackersWithLeftoverDetections();
		collectResultsWhileKillingTrackers();
	}
}

vector<Rect_<float>> sortTrackerPiplelined::createTrajecotoryPredictions()
{
	vector<Rect_<float>> trajectoryPredictions;

	for (auto it = m_vectorsOfTrackers.begin(); it != m_vectorsOfTrackers.end();)
	{
		Rect_<float> pBox = (*it).predict();
		if (pBox.x >= 0 && pBox.y >= 0)
		{
			trajectoryPredictions.push_back(pBox);
			it++;
		}
		else
		{
			it = m_vectorsOfTrackers.erase(it);
		}
	}
	m_numTrajectories = trajectoryPredictions.size();

	return trajectoryPredictions;
}

void sortTrackerPiplelined::collectResultsWhileKillingTrackers()
{
	bool missedTooManyTimes, trackerHasMatchedEnough, lessThanMinHitsHasBeenProcessed;
	lessThanMinHitsHasBeenProcessed = m_numberFramesProcessed <= m_minHitsInFrames;

	for (auto it = m_vectorsOfTrackers.begin(); it != m_vectorsOfTrackers.end();)
	{
		missedTooManyTimes = it->m_time_since_update > m_maxUpdateAllowance;
		trackerHasMatchedEnough = it->m_hit_streak >= m_minHitsInFrames;

		if (missedTooManyTimes)
		{
			it = m_vectorsOfTrackers.erase(it);
		}
		else if (trackerHasMatchedEnough || lessThanMinHitsHasBeenProcessed)
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
}

void sortTrackerPiplelined::updateTrackers(const vector<cv::Point>& pairs)
{
	int detIdx, trkIdx;
	for (unsigned int i = 0; i < pairs.size(); i++)
	{
		trkIdx = pairs[i].x;
		detIdx = pairs[i].y;
		m_vectorsOfTrackers[trkIdx].update(m_frameData[detIdx].box);
	}
}

void sortTrackerPiplelined::createNewTrackersWithLeftoverDetections()
{
	for (auto umd : m_unmatchedDetections)
	{
		KalmanTracker tracker = KalmanTracker(m_frameData[umd].box);
		m_vectorsOfTrackers.push_back(tracker);
	}
}

vector<vector<double>> sortTrackerPiplelined::constructIOUmat(const vector<Rect_<float>>& trajectoryPredictions)
{
	vector<vector<double>> iouCostMatrix;

	iouCostMatrix.resize(m_numTrajectories, vector<double>(m_numDetections, 0));

	for (unsigned int i = 0; i < m_numTrajectories; i++)
	{
		for (unsigned int j = 0; j < m_numDetections; j++)
		{
			// use 1-iou because the Hungarian algorithm computes a minimum-cost assignment.
			iouCostMatrix[i][j] = 1 - GetIOU(trajectoryPredictions[i], m_frameData[j].box);
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

vector<cv::Point> sortTrackerPiplelined::matchDetectionsToTrajectories(const vector<vector<double>>& iouCostMatrix)
{
	vector<int> assignments;
	HungarianAlgorithm HungAlgo;
	HungAlgo.Solve(iouCostMatrix, assignments);

	fillUnmatchedDetections(assignments);
	fillUnmatchedTrajectories(assignments);

	// filter out matched with low IOU
	vector<cv::Point> pairs;

	for (int i = 0; i < m_numTrajectories; ++i)
	{
		if (assignments[i] == -1) // pass over invalid values
			continue;
		if (1 - iouCostMatrix[i][assignments[i]] < m_iou)
		{
			m_unmatchedTrajectories.insert(i);
			m_unmatchedDetections.insert(assignments[i]);
		}
		else
			pairs.push_back(cv::Point(i, assignments[i]));
	}

	return pairs;
}


double sortTrackerPiplelined::GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt)
{
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}