#ifndef TRACKINGBOX_H
#define TRACKINGBOX_H

#include <iostream>
#include <opencv2/core.hpp>

class TrackingBox : public cv::Rect_<float>
{

public:

	TrackingBox() {};
	TrackingBox(int inFrame, int inId, cv::Rect_<float> inBox) 
		: Rect_<float>(inBox)
	{ 
		frame = inFrame;
		id = inId; 
	}

	void updateBox(const cv::Rect_<float>& input);

	int frame;
	int id;

	friend std::ostream& operator<< (std::ostream& out, const TrackingBox& box);
	friend std::istream& operator>> (std::istream& in, TrackingBox& box);

	//Two boxes are equal when they are the same frame and ID
	friend bool operator== (const TrackingBox& c1, const TrackingBox& c2);
	friend bool operator!= (const TrackingBox& c1, const TrackingBox& c2);

	double GetIOU(const TrackingBox& bb_gt);

};

#endif // !TRACKINGBOX_H

