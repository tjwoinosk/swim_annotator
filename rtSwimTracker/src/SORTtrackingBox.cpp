#include "SORTtrackingBox.h"

std::ostream& operator<<(std::ostream& out, const TrackingBox& box)
{
  out << box.frame << "," << box.id << "," << box.box.x << "," << box.box.y << "," << box.box.width << "," << box.box.height << ",1,-1,-1,-1" << std::endl;
	return out;
}

std::istream& operator>>(std::istream& in, TrackingBox& box)
{
	float tpx, tpy, tpw, tph;
	char ch;

	in >> box.frame >> ch >> box.id >> ch;
	in >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;

	box.box = cv::Rect_<float>(cv::Point_<float>(tpx, tpy), cv::Point_<float>(tpx + tpw, tpy + tph));

	return in;
}
