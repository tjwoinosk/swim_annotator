#include "..\include\DetectionBox.h"

std::ostream& operator<<(std::ostream& out, const DetectionBox& box)
{
	out << box.m_frame << "," << box.m_boxID;
	out << "," << box.x << "," << box.y << "," << box.width << "," << box.height;
  out << "," << box.m_confScore << "," << box.m_swimmerClass << std::endl;
  return out;
}

std::istream& operator>>(std::istream& in, DetectionBox& box)
{
	float tpx, tpy, tpw, tph;
	float confScore;
	int swimmerClass, frame, objectID;
	char ch;

	in >> frame >> ch >> objectID >> ch;
	in >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;
	in >> ch >> confScore >> ch >> swimmerClass;

	box.m_frame = frame;
	box.m_boxID = objectID;
	box.x = tpx;
	box.y = tpy;
	box.height = tph;
	box.width = tpw;
	box.m_confScore = confScore;
	box.m_swimmerClass = swimmerClass;

	return in;
}
