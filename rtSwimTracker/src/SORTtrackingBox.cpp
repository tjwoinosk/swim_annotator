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

bool operator== (const TrackingBox& c1, const TrackingBox& c2)
{
	return((c1.frame == c2.frame) && (c1.id == c2.id));
}

bool operator!= (const TrackingBox& c1, const TrackingBox& c2)
{
	return(!(c1 == c2));
}

float TrackingBox::GetIOU(const TrackingBox& bb_gt)
{
	float in = (box & bb_gt.box).area();
	float un = box.area() + bb_gt.box.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (in / un);
}