#include "objectCentering.h"

objectCentering::objectCentering()
{
	deltaX = 10;
	deltaY = 10;
	centrePoint_Frame = cv::Point_<float>(0, 0);
}

objectCentering::~objectCentering()
{
}

tiltPanCommand objectCentering::findCommand(TrackingBox swimmerFollowed, cv::Point_<float> pointCentre)
{
	cv::Point_<float> diffFromCentre = findPointDifference(pointCentre, swimmerFollowed);
	return findCommand(diffFromCentre);
}

tiltPanCommand objectCentering::findCommand(TrackingBox swimmerFollowed)
{
	if (centrePoint_Frame.x == 0 && centrePoint_Frame.y == 0) {
		std::cout << " ERROR: Centre point of frame not set." << std::endl;
		return tiltPanCommand();
	}
	cv::Point_<float> diffFromCentre = findPointDifference(centrePoint_Frame, swimmerFollowed);
	return findCommand(diffFromCentre);
}

tiltPanCommand objectCentering::findCommand(char c)
{
	tiltPanCommand returnCommands;
	returnCommands.moveDown = false;
	returnCommands.moveUp = false;
	returnCommands.moveLeft = false;
	returnCommands.moveRight = false;

	if (c == 97) { //Pressed a
		returnCommands.moveLeft = true;
	}
	if (c == 100) { //Pressed d
		returnCommands.moveRight = true;
	}
	if (c == 119) { //Pressed w
		returnCommands.moveUp = true;
	}
	if (c == 115) { //Pressed s
		returnCommands.moveDown = true;
	}

	return returnCommands;
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

bool objectCentering::setDelta(float percent_X, float percent_Y, cv::Mat frame)
{
	if(percent_X > 1 || percent_X < 0)
		return false;
	if (percent_Y > 1 || percent_Y < 0)
		return false;

	float frameHeight = static_cast<float>(frame.rows);
	float frameWidth = static_cast<float>(frame.cols);

	deltaX = frameWidth * percent_X;
	deltaY = frameHeight * percent_Y;

	return true;
}

bool objectCentering::setDelta(float percent_X, float percent_Y, cv::Point_<float> frameCorner)
{
	if (percent_X > 1 || percent_X < 0)
		return false;
	if (percent_Y > 1 || percent_Y < 0)
		return false;
	if(frameCorner.x < 0 || frameCorner.y < 0)
		return false;

	std::cout << " frameCorder (x, y) = " << frameCorner.x << " , " << frameCorner.y << std::endl;
	std::cout << " (percent x, percent y) = " << percent_X << " , " << percent_Y << std::endl;

	deltaX = frameCorner.x * percent_X;
	deltaY = frameCorner.y * percent_Y;

	return true;
}

float objectCentering::getDeltaX()
{
	return deltaX;
}

float objectCentering::getDeltaY()
{
	return deltaY;
}

void objectCentering::setCentrePointFrame(cv::Mat frame)
{
	centrePoint_Frame = findCentreOfFrame(frame);
}

void objectCentering::outputToFile(std::ostream& out, tiltPanCommand box)
{
	std::string goRight;
	std::string goLeft;
	std::string goUp;
	std::string goDown;

	if (box.moveDown)
		goDown = "Down = YES";
	else
		goDown = "Down = NO";

	if (box.moveUp)
		goUp = "Up = YES";
	else
		goUp = "Up = NO";

	if (box.moveLeft)
		goLeft = "Left = YES";
	else
		goLeft = "Left = NO";

	if (box.moveRight)
		goRight = "Right = YES";
	else
		goRight = "Right = NO";

	out << goRight << "," << goLeft << "," << goUp << "," << goDown << std::endl;
	return;
}

void objectCentering::outputToScreen(tiltPanCommand box)
{
	std::string goRight;
	std::string goLeft;
	std::string goUp;
	std::string goDown;

	if (box.moveDown)
		std::cout << "Down = YES, ";

	if (box.moveUp)
		std::cout << "Up = YES, ";

	if (box.moveLeft)
		std::cout << "Left = YES, ";

	if (box.moveRight)
		std::cout << "Right = YES, ";

	if(box.moveDown || box.moveUp || box.moveLeft || box.moveRight)
		std::cout  << std::endl;

	return;
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

	if (abs(diffOfPoints.x) > deltaX) {
		if (diffOfPoints.x > 0) {
			returnCommands.moveLeft = true;
		}
		else if (diffOfPoints.x < 0) {
			returnCommands.moveRight = true;
		}
	}

	if (abs(diffOfPoints.y) > deltaY) {
		if (diffOfPoints.y > 0) { //In the image, y = 0 is at the top, and increases as you go down
			returnCommands.moveUp = true;
		}
		else if (diffOfPoints.y < 0) {
			returnCommands.moveDown = true;
		}
	}

	return returnCommands;
}

std::ostream& operator<<(std::ostream& out, const tiltPanCommand& box)
{
	std::string goRight;
	std::string goLeft;
	std::string goUp;
	std::string goDown;

	if (box.moveDown)
		goDown = "Down = YES";
	else
		goDown = "Down = NO";

	if (box.moveUp)
		goUp = "Up = YES";
	else
		goUp = "Up = NO";

	if (box.moveLeft)
		goLeft = "Left = YES";
	else
		goLeft = "Left = NO";

	if (box.moveRight)
		goRight = "Right = YES";
	else
		goRight = "Right = NO";

	out << goRight << "," << goLeft << "," << goUp << "," << goDown  << std::endl;
	return out;
}