#ifndef SWIMMERDETECTOR_H
#define SWIMMERDETECTOR_H

#include "TrackingBox.h"
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

class swimmerDetector
{
public:
	swimmerDetector();
	~swimmerDetector();

	std::vector<TrackingBox> detectSwimmers(cv::Mat frame);

	void configureDetector();

	bool setConfThreshold(float valSet);
	float getConfThreshold();
	bool setNmsThreshold(float valSet);
	float getNmsThreshold();

private:

	std::vector<TrackingBox> postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, int frame_num);
	std::vector<std::string> getOutputLayerIDStrings(const cv::dnn::Net& net);

	std::string m_classesFile = "classes.names";
	std::string m_modelConfiguration = "yolov3.cfg";
	std::string m_modelWeights = "yolov3.weights";

	cv::dnn::Net m_net;

	float confThreshold;
	float nmsThreshold;
};

#endif // !SWIMMERDETECTOR_H
