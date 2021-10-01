#ifndef SORTTRACKINGBOX_H
#define SORTTRACKINGBOX_H

#include <iostream>
#include <opencv2/opencv.hpp>

class TrackingBox
{

public:

	TrackingBox() {};
	TrackingBox(int inFrame, int inId, cv::Rect_<float> inBox) { frame = inFrame; id = inId; box = inBox; }

	int frame;
	int id;
	cv::Rect_<float> box;

	friend std::ostream& operator<< (std::ostream& out, const TrackingBox& box);
	friend std::istream& operator>> (std::istream& in, TrackingBox& box);

	//Two boxes are equal when they are the same frame and ID
	friend bool operator== (const TrackingBox& c1, const TrackingBox& c2);
	friend bool operator!= (const TrackingBox& c1, const TrackingBox& c2);

	float GetIOU(const TrackingBox& bb_gt);

};


#endif // !SORTTRACKINGBOX_H

