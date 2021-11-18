#include "objectCentering.h"

objectCentering::objectCentering()
{
}

objectCentering::~objectCentering()
{
}

tiltPanCommand objectCentering::findCommand(TrackingBox swimmerFollowed, cv::Point_<float> pointCentre)
{
	cv::Point_<float> diffFromCentre = findPointDifference(pointCentre, swimmerFollowed);
	return findCommand(diffFromCentre);
}

tiltPanCommand objectCentering::findCommand(TrackingBox swimmerFollowed, cv::Mat frame)
{
	cv::Point_<float> centrePoint = findCentreOfFrame(frame);
	cv::Point_<float> diffFromCentre = findPointDifference(centrePoint, swimmerFollowed);
	return findCommand(diffFromCentre);
}

cv::Point_<float> objectCentering::findCentreOfFrame(cv::Mat frame)
{
	//Find the center point of the frame to use for finding the command
	float frameHeight = static_cast<float>(frame.rows);
	float frameWidth = static_cast<float>(frame.cols);

	return cv::Point_<float>(frameWidth / 2, frameHeight / 2);
}

cv::Point_<float> objectCentering::findPointDifference(cv::Point_<float> pointCentre, TrackingBox pointSwimmer)
{
	//Find the difference from the centre point to the centre of the box representing the swimmer
	float swimmerCentre_x = pointSwimmer.x + (pointSwimmer.width) / 2;
	float swimmerCentre_y = pointSwimmer.y + (pointSwimmer.height) / 2;

	float x_diff = pointCentre.x - swimmerCentre_x;
	float y_diff = pointCentre.y - swimmerCentre_y;

	return cv::Point_<float>(x_diff, y_diff);
}

tiltPanCommand objectCentering::findCommand(cv::Point_<float> diffOfPoints)
{
	/*
* Find where the swimmer box is relative to the centre of the image.
From that, find the direction we need to go to be closer to the centre

If we consider centre, then
(a) a swimmer to the left of center is at x > 0. Move left.
(b) a swimmer to the right of the center is at x < 0. Move right.
(c) a swimmer above the center is at y < 0. Move up.
(d) a swimmer below the center is at y > 0. Mode down.

*/
	tiltPanCommand returnCommands;
	returnCommands.moveDown = false;
	returnCommands.moveUp = false;
	returnCommands.moveLeft = false;
	returnCommands.moveRight = false;

	if (diffOfPoints.x > 0) {
		returnCommands.moveLeft = true;
	}
	else if (diffOfPoints.x < 0) {
		returnCommands.moveRight = true;
	}

	if (diffOfPoints.y < 0) {
		returnCommands.moveUp = true;
	}
	else if (diffOfPoints.y > 0) {
		returnCommands.moveDown = true;
	}

	return returnCommands;
}

