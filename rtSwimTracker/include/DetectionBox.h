#ifndef DETECTIONBOX_H
#define DETECTIONBOX_H

#include "TrackingBox.h"



class DetectionBox : public TrackingBox
{
public:
	DetectionBox() {};

	DetectionBox(int inSwimClass, float inConf, int inFrameNumber, int inIDNum, Rect_<float> inBox)
		: TrackingBox(inFrameNumber, inIDNum, inBox)
	{
		m_swimmerClass = inSwimClass;
		m_confScore = inConf;
	}

	int m_swimmerClass;
	float m_confScore;

	friend std::ostream& operator<< (std::ostream& out, const DetectionBox& box);
	friend std::istream& operator>> (std::istream& in, DetectionBox& box);

private:

	//const std::string CLASSES[6] = { "On_Block", "Diving", "Swimming", "Underwater", "Turning", "Finishing" }; //the six possible classes

};

#endif // !DETECTIONBOX_H
