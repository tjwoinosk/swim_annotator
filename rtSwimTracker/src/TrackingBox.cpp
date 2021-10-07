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