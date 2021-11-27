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
		m_frame = inFrame;
		m_boxID = inId; 
	}
	TrackingBox(int inSwimClass, float inConf, int inFrameNumber, int inIDNum, Rect_<float> inBox) 
		: Rect_<float>(inBox) 
	{
		m_frame = inFrameNumber;
		m_boxID = inIDNum;
		m_swimmerClass = inSwimClass;
		m_confScore = inConf;
	}

	void updateBox(const cv::Rect_<float>& input);

	friend std::ostream& operator<< (std::ostream& out, const TrackingBox& box);
	friend std::istream& operator>> (std::istream& in, TrackingBox& box);

	//Two boxes are equal when they are the same frame and ID
	friend bool operator== (const TrackingBox& c1, const TrackingBox& c2);
	friend bool operator!= (const TrackingBox& c1, const TrackingBox& c2);

	virtual double GetIOU(const TrackingBox& bb_gt);

	void outputToFile(std::ostream& out); // Similar to overloaded operator<< (can use either)
	void inputFromFile(std::istream& in); //Similar to overloaded operator>> (can use either)
	void outputToFileDetection(std::ostream& out);
	void inputFromFileDetection(std::istream& in);

	bool set_m_frame(int setVal);
	int get_m_frame();
	bool set_m_boxID(int setVal);
	int get_m_boxID();
	bool set_m_swimmerClass(int setVal);
	int get_m_swimmerClass();
	bool set_m_confScore(float setVal);
	float get_m_confScore();

	bool allZeroes();

private:
	std::string CLASSES[6] = { "On_Block", "Diving", "Swimming", "Underwater", "Turning", "Finishing" }; //the six possible classes

	int m_frame;
	int m_boxID;
	int m_swimmerClass;
	float m_confScore;
};

#endif // !TRACKINGBOX_H
