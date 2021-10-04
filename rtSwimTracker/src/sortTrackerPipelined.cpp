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
	m_frameTrackingResults.clear();
	inputDetectionData(swimmerDetections);

	processFrame();
	
	return m_frameTrackingResults;
}


void sortTrackerPiplelined::processFrame()
{
	vector<vector<double>> iouCostMatrix;
	vector<Rect_<float>> trajectoryPredictions;
	vector<cv::Point> pairs;

	m_numberFramesProcessed++;
	if (m_vectorOfTrackers.size() == 0)
	{
		initializeTrackersUsing(m_frameData);
	}
	else
	{
		trajectoryPredictions = createTrajecotoryPredictions(trajectoryPredictions);
		iouCostMatrix = constructIOUCostMat(trajectoryPredictions, iouCostMatrix);
		pairs = matchDetectionsToTrajectories(iouCostMatrix, pairs);
		updateTrackers(pairs);
		createNewTrackersWithLeftoverDetections();
		collectResultsWhileKillingTrackers();
	}
}


void sortTrackerPiplelined::initializeTrackersUsing(const vector<TrackingBox>& trackingBoxData)
{
	TrackingBox tb;
	KalmanTracker trk;
	for (int i = 0; i < trackingBoxData.size(); i++)
	{
		trk = KalmanTracker(trackingBoxData[i].box);
		m_vectorOfTrackers.push_back(trk);

		tb.box = trk.get_state();
		tb.id = trk.m_id + 1;
		tb.frame = m_numberFramesProcessed;
		m_frameTrackingResults.push_back(tb);
	}
}


vector<Rect_<float>>& sortTrackerPiplelined::createTrajecotoryPredictions(vector<Rect_<float>>& initializedValue)
{
	for (auto it = m_vectorOfTrackers.begin(); it != m_vectorOfTrackers.end();)
	{
		Rect_<float> pBox = (*it).predict();
		if (pBox.x >= 0 && pBox.y >= 0)
		{
			initializedValue.push_back(pBox);
			it++;
		}
		else
		{
			it = m_vectorOfTrackers.erase(it);
		}
	}
	m_numTrajectories = initializedValue.size();

	return initializedValue;
}


vector<vector<double>>& sortTrackerPiplelined::constructIOUCostMat(const vector<Rect_<float>>& trajectoryPredictions, vector<vector<double>>& iouCostMatrix)
{
	iouCostMatrix.resize(m_numTrajectories, vector<double>(m_numDetections, 0));

	for (int i = 0; i < m_numTrajectories; i++)
	{
		for (int j = 0; j < m_numDetections; j++)
		{
			// use 1-iou because the Hungarian algorithm computes a minimum-cost assignment.
			iouCostMatrix[i][j] = 1 - GetIOU(trajectoryPredictions[i], m_frameData[j].box);
		}
	}
	return iouCostMatrix;
}


vector<cv::Point>& sortTrackerPiplelined::matchDetectionsToTrajectories(const vector<vector<double>>& iouCostMatrix, vector<cv::Point>& pairs)
{
	vector<int> assignments;
	HungarianAlgorithm HungAlgo;
	HungAlgo.Solve(iouCostMatrix, assignments);

	fillUnmatchedDetections(assignments);
	fillUnmatchedTrajectories(assignments);

	// filter out matched with low IOU

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


void sortTrackerPiplelined::fillUnmatchedDetections(const vector<int>& assignments)
{
	set<int> allItems;
	set<int> matchedItems;

	m_unmatchedDetections.clear();

	if (m_numDetections > m_numTrajectories)
	{
		for (int n = 0; n < m_numDetections; n++)
			allItems.insert(n);

		for (int i = 0; i < m_numTrajectories; ++i)
			matchedItems.insert(assignments[i]);

		set_difference(allItems.begin(), allItems.end(),
			matchedItems.begin(), matchedItems.end(),
			insert_iterator<set<int>>(m_unmatchedDetections, m_unmatchedDetections.begin()));
	}
}

void sortTrackerPiplelined::fillUnmatchedTrajectories(const vector<int>& assignments)
{
	if (m_numDetections < m_numTrajectories)
	{
		for (int i = 0; i < m_numTrajectories; ++i)
			if (assignments[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
				m_unmatchedTrajectories.insert(i);
	}
}


void sortTrackerPiplelined::updateTrackers(const vector<cv::Point>& pairs)
{
	int detIdx, trkIdx;
	for (int i = 0; i < pairs.size(); i++)
	{
		trkIdx = pairs[i].x;
		detIdx = pairs[i].y;
		m_vectorOfTrackers[trkIdx].update(m_frameData[detIdx].box);
	}
}


void sortTrackerPiplelined::createNewTrackersWithLeftoverDetections()
{
	for (auto umd : m_unmatchedDetections)
	{
		KalmanTracker tracker = KalmanTracker(m_frameData[umd].box);
		m_vectorOfTrackers.push_back(tracker);
	}
}


void sortTrackerPiplelined::collectResultsWhileKillingTrackers()
{
	bool missedTooManyTimes, trackerHasMatchedEnough, lessThanMinHitsHasBeenProcessed;
	lessThanMinHitsHasBeenProcessed = m_numberFramesProcessed <= m_minHitsInFrames;

	for (auto it = m_vectorOfTrackers.begin(); it != m_vectorOfTrackers.end();)
	{
		missedTooManyTimes = it->m_time_since_update > m_maxUpdateAllowance;
		trackerHasMatchedEnough = it->m_hit_streak >= m_minHitsInFrames;

		if (missedTooManyTimes)
		{
			it = m_vectorOfTrackers.erase(it);
		}
		else if (trackerHasMatchedEnough || lessThanMinHitsHasBeenProcessed)
		{
			TrackingBox res;
			res.box = it->get_state();
			res.id = it->m_id + 1;
			res.frame = m_numberFramesProcessed;
			m_frameTrackingResults.push_back(res);
			it++;
		}
		else
			it++;
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