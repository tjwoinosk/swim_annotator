#include "TrackingBox.h"

std::ostream& operator<<(std::ostream& out, const TrackingBox& box)
{
  out << box.m_frame << "," << box.m_boxID << "," << box.x << "," << box.y << "," << box.width << "," << box.height << ",1,-1,-1,-1" << std::endl;
	return out;
}

std::istream& operator>>(std::istream& in, TrackingBox& box)
{
	float tpx, tpy, tpw, tph;
	char ch;

	in >> box.m_frame >> ch >> box.m_boxID >> ch;
	in >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;

	box.x = tpx;
	box.y = tpy;
	box.height = tph;
	box.width = tpw;

	return in;
}

bool operator== (const TrackingBox& c1, const TrackingBox& c2)
{
	return((c1.m_frame == c2.m_frame) && (c1.m_boxID == c2.m_boxID));
}

bool operator!= (const TrackingBox& c1, const TrackingBox& c2)
{
	return(!(c1 == c2));
}

void TrackingBox::outputToFile(std::ostream& out)
{
	//A version of <<
	out << m_frame << "," << m_boxID << "," << x << "," << y << "," << width << "," << height << ",1,-1,-1,-1" << std::endl;
	//return out;
}

void TrackingBox::inputFromFile(std::istream& in)
{
	float tpx, tpy, tpw, tph;
	char ch;

	in >> m_frame >> ch >> m_boxID >> ch;
	in >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;

	x = tpx;
	y = tpy;
	height = tph;
	width = tpw;
}

void TrackingBox::outputToFileDetection(std::ostream& out)
{
	out << m_frame << "," << m_boxID;
	out << "," << x << "," << y << "," << width << "," << height;
	out << "," << m_confScore << "," << m_swimmerClass << std::endl;
}

void TrackingBox::inputFromFileDetection(std::istream& in)
{
	float tpx, tpy, tpw, tph;
	float confScore;
	int swimmerClass, frame, objectID;
	char ch;

	in >> frame >> ch >> objectID >> ch;
	in >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;
	in >> ch >> confScore >> ch >> swimmerClass;

	m_frame = frame;
	m_boxID = objectID;
	x = tpx;
	y = tpy;
	height = tph;
	width = tpw;
	m_confScore = confScore;
	m_swimmerClass = swimmerClass;
}

bool TrackingBox::set_m_frame(int setVal)
{
	if (setVal < 0) { return false; } //TODO is this condition okay?
	m_frame = setVal;
	return true;
}

int TrackingBox::get_m_frame()
{
	return m_frame;
}

bool TrackingBox::set_m_boxID(int setVal)
{
	if (setVal < -1) { return false; } //TODO is this condition okay?
	m_boxID = setVal;
	return true;
}

int TrackingBox::get_m_boxID()
{
	return m_boxID;
}

bool TrackingBox::set_m_swimmerClass(int setVal)
{
	if (setVal < 0 || setVal > 5) { return false; } //TODO is this condition okay?
	m_swimmerClass = setVal;
	return true;
}

int TrackingBox::get_m_swimmerClass()
{
	return m_swimmerClass;
}

bool TrackingBox::set_m_confScore(float setVal)
{
	if (setVal < 0.0f || setVal > 1.0f) { return false; } //TODO is this condition okay?
	m_confScore = setVal;
	return true;
}

float TrackingBox::get_m_confScore()
{
	return m_confScore;
}

double TrackingBox::GetIOU(const TrackingBox& bb_gt)
{
	float in = ((*this) & bb_gt).area();
	float un = this->area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}

void TrackingBox::updateBox(const cv::Rect_<float>& input)
{
	x = input.x;
	y = input.y;
	height = input.height;
	width = input.width;
}