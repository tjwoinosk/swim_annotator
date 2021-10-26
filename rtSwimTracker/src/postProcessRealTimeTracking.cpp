#include "postProcessRealTimeTracking.h"

int postProcessRealTimeTracking::trajectoryMatcher(cv::Point_<float> mouseClick, std::vector<TrackingBox> trajectories)
{
	cv::Point_<float> centreBox;
	int euclidianDistance = 0;
	int smallestEuclidianDistance = 0;
	int IDsmallestDistance = 0;
	int tmp1 = 0;
	int tmp2 = 0;

	for (int i = 0; i < trajectories.size(); i++) {
		centreBox = getCentre(trajectories[i]);
		tmp1 = pow((centreBox.x - mouseClick.x), 2);
		tmp2 = pow((centreBox.y - mouseClick.y), 2);
		euclidianDistance = sqrt(tmp1 + tmp2);

		if (i == 0) {
			smallestEuclidianDistance = euclidianDistance;
			IDsmallestDistance = trajectories[i].get_m_boxID();
			continue;
		}

		if (euclidianDistance < smallestEuclidianDistance) {
			smallestEuclidianDistance = euclidianDistance;
			IDsmallestDistance = trajectories[i].get_m_boxID();
		}
	}

	return IDsmallestDistance;
}

cv::Point_<float> postProcessRealTimeTracking::getCentre(cv::Rect_<float> boxSwimmer)
{
	int x2 = 0;
	int y2 = 0;
	x2 = boxSwimmer.x + boxSwimmer.width / 2;
	y2 = boxSwimmer.y + boxSwimmer.height / 2;
	return cv::Point_<float>(x2, y2);
}
