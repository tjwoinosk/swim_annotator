#ifndef POSTPROCESSREALTIMETRACKING_H
#define POSTPROCESSREALTIMETRACKING_H

#include "TrackingBox.h"
#include <math.h>

class postProcessRealTimeTracking
{

public:
	postProcessRealTimeTracking() {};

	int trajectoryMatcher(int xMouse, int yMouse, std::vector<TrackingBox> trajectories);
	cv::Point_<float> getCentre(cv::Rect_<float> boxSwimmer); //TODO maybe should be private? For testing habe publix


private:

};

#endif // !ADD_ANOTHER_FILE_H