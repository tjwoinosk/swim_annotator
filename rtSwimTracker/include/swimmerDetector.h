#ifndef SWIMMERDETECTOR_H
#define SWIMMERDETECTOR_H

#include "TrackingBox.h"
#include <string>
#include <opencv2/core.hpp>

class swimmerDetector
{
public:
	swimmerDetector();
	~swimmerDetector();

	TrackingBox detectSwimmers(cv::Mat frame);

private:

	//void postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, int frame_num);
	//std::vector<std::string> getOutputsNames(const cv::dnn::Net& net);

};

#endif // !SWIMMERDETECTOR_H

