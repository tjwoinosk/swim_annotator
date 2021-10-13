#include "sortTrackerPipelined.h"
#include "HungarianAlgorithm.h"

sortTrackerPiplelined::sortTrackerPiplelined()
{
	KalmanTracker::kf_count = 0; // tracking id relies on this, so we have to reset it in each seq.
	m_numberFramesProcessed = 0;
}

/*
This function will take trackers and data for a single frame (frame number fi) and produce predictions
for that frame, as well as adjust the trackers accordingly
*/
std::vector<TrackingBox> sortTrackerPiplelined::singleFrameSORT(std::vector<TrackingBox> frameDetections)
{
	inputDetectionData(frameDetections);

	m_frameTrackingResults.clear();

	processFrame();
	
	frameDetections.clear();
	for(int ii = 0; ii < m_frameTrackingResults.size(); ii++)
		frameDetections.push_back(m_frameTrackingResults[ii]);

	return frameDetections;
}

std::vector<TrackingBox> sortTrackerPiplelined::singleFrameSORT(std::vector<DetectionBox> frameDetections)
{
	std::vector<TrackingBox> resultsFrameData;

	inputDetectionData(frameDetections);

	m_frameTrackingResults.clear();

	processFrame();

	resultsFrameData.clear();

	for (int ii = 0; ii < m_frameTrackingResults.size(); ii++)
		resultsFrameData.push_back(m_frameTrackingResults[ii]);

	return resultsFrameData;
}

void sortTrackerPiplelined::processFrame()
{
	std::vector<std::vector<double>> iouCostMatrix;
	std::vector<cv::Rect_<float>> trajectoryPredictions;
	std::vector<cv::Point> pairs;

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

void sortTrackerPiplelined::initializeTrackersUsing(const std::vector<TrackingBox>& trackingBoxData)
{
	TrackingBox tb;
	KalmanTracker trk;
	for (int i = 0; i < trackingBoxData.size(); i++)
	{
		trk = KalmanTracker(trackingBoxData[i]);
		m_vectorOfTrackers.push_back(trk);

		tb.updateBox(trk.get_state());
		tb.m_boxID = trk.m_id + 1;
		tb.m_frame = m_numberFramesProcessed;
		m_frameTrackingResults.push_back(tb);
	}
}

std::vector<cv::Rect_<float>>& sortTrackerPiplelined::createTrajecotoryPredictions(std::vector<cv::Rect_<float>>& initializedValue)
{
	for (auto it = m_vectorOfTrackers.begin(); it != m_vectorOfTrackers.end();)
	{
		cv::Rect_<float> pBox = (*it).predict();
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
	m_numTrajectories = static_cast<int>(initializedValue.size());

	return initializedValue;
}

std::vector<std::vector<double>>& sortTrackerPiplelined::constructIOUCostMat(const std::vector<cv::Rect_<float>>& trajectoryPredictions, std::vector<std::vector<double>>& iouCostMatrix)
{
	iouCostMatrix.resize(m_numTrajectories, std::vector<double>(m_numDetections, 0));

	for (int i = 0; i < m_numTrajectories; i++)
	{
		for (int j = 0; j < m_numDetections; j++)
		{
			// use 1-iou because the Hungarian algorithm computes a minimum-cost assignment.
			iouCostMatrix[i][j] = 1 - GetIOU(trajectoryPredictions[i], m_frameData[j]);
		}
	}
	return iouCostMatrix;
}

std::vector<cv::Point>& sortTrackerPiplelined::matchDetectionsToTrajectories(const std::vector<std::vector<double>>& iouCostMatrix, std::vector<cv::Point>& pairs)
{
	std::vector<int> assignments;
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

void sortTrackerPiplelined::fillUnmatchedDetections(const std::vector<int>& assignments)
{
	std::set<int> allItems;
	std::set<int> matchedItems;

	m_unmatchedDetections.clear();

	if (m_numDetections > m_numTrajectories)
	{
		for (int n = 0; n < m_numDetections; n++)
			allItems.insert(n);

		for (int i = 0; i < m_numTrajectories; ++i)
			matchedItems.insert(assignments[i]);

		set_difference(allItems.begin(), allItems.end(),
			matchedItems.begin(), matchedItems.end(),
			std::insert_iterator<std::set<int>>(m_unmatchedDetections, m_unmatchedDetections.begin()));
	}
}

void sortTrackerPiplelined::fillUnmatchedTrajectories(const std::vector<int>& assignments)
{
	if (m_numDetections < m_numTrajectories)
	{
		for (int i = 0; i < m_numTrajectories; ++i)
			if (assignments[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
				m_unmatchedTrajectories.insert(i);
	}
}

void sortTrackerPiplelined::updateTrackers(const std::vector<cv::Point>& pairs)
{
	int detIdx, trkIdx;
	for (int i = 0; i < pairs.size(); i++)
	{
		trkIdx = pairs[i].x;
		detIdx = pairs[i].y;
		m_vectorOfTrackers[trkIdx].update(m_frameData[detIdx]);
	}
}

void sortTrackerPiplelined::createNewTrackersWithLeftoverDetections()
{
	for (auto umd : m_unmatchedDetections)
	{
		KalmanTracker tracker = KalmanTracker(m_frameData[umd]);
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
			res.updateBox(it->get_state());
			res.m_boxID = it->m_id + 1;
			res.m_frame = m_numberFramesProcessed;
			m_frameTrackingResults.push_back(res);
			it++;
		}
		else
			it++;
	}
}

double sortTrackerPiplelined::GetIOU(cv::Rect_<float> bb_test, cv::Rect_<float> bb_gt)
{
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}

void sortTrackerPiplelined::inputDetectionData(const std::vector<DetectionBox>& detFrameData) {
	// https://stackoverflow.com/questions/5966698/error-use-of-deleted-function
	//https://stackoverflow.com/questions/9365318/c-can-i-cast-a-vector-derived-class-to-a-vector-base-class-during-a-funct

	m_frameData.clear(); 

	for (int i = 0; i < detFrameData.size(); i++) {
		TrackingBox temp = (DetectionBox)detFrameData[i];
		m_frameData.push_back(temp);
	}
	m_numDetections = static_cast<int>(detFrameData.size());

}