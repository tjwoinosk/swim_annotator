#ifndef OBJECTCENTERING_H
#define OBJECTCENTERING_H

#include "TrackingBox.h"
#include <math.h>
//TODO takes the one tracking box on the swimmer we are analyzing and return the datatype
//that will define where the camera needs to move
//camera moves by the following switches: switch to go up, left, right, down, not go left or down, not go right or left
// we can only have one switch at a moment activated
//9 different options - think of as two servos: left, right, nothing; down, up, nothing
//sending those signals from this object, we can control this camera
//maybe every frame or every other frame we can update (will decide later)
//would be in the frame analysis main loop, where everytime the frame goes in, you find the swimmer tracking
// you put the box you finished tracking into this object, and then it goes left or right

struct tiltPanCommand
{
	bool moveRight; // if both are true or false, then don't move
	bool moveLeft;
	bool moveDown;
	bool moveUp;
};

class objectCentering
{
public:
	objectCentering();
	~objectCentering();

	void calculate() {};

	tiltPanCommand findCommand(TrackingBox swimmerFollowed, cv::Mat frame); //TODO should it be pass in frame or centre?
	tiltPanCommand findCommand(TrackingBox swimmerFollowed, cv::Point_<float> pointCentre);
	cv::Point_<float> findCentreOfFrame(cv::Mat frame);
	cv::Point_<float> findPointDifference(cv::Point_<float> pointCentre, TrackingBox pointSwimmer);

	bool setDelta(float percent_X, float percent_Y, cv::Mat frame); //TODO should it be pass in frame or centre?
	bool setDelta(float percent_X, float percent_Y, cv::Point_<float> frameCorner);
	float getDeltaX();
	float getDeltaY();

private:
	float deltaX;
	float deltaY;

	tiltPanCommand findCommand(cv::Point_<float> diffOfPoints);
};


#endif // !OBJECTCENTERING_H

