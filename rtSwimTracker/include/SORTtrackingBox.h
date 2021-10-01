#ifndef SORTTRACKINGBOX_H
#define SORTTRACKINGBOX_H

#include <iostream>
#include <opencv2/opencv.hpp>

class TrackingBox
{

public:

	TrackingBox() {};

	int frame;
	int id;
	cv::Rect_<float> box;

	friend std::ostream& operator<< (std::ostream& out, const TrackingBox& box);
	friend std::istream& operator>> (std::istream& in, TrackingBox& box);

};


#endif // !SORTTRACKINGBOX_H

