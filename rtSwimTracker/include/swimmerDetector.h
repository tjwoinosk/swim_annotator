#ifndef SWIMMERDETECTOR_H
#define SWIMMERDETECTOR_H

#include "DetectionBox.h"
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/dnn/dnn.hpp>

class swimmerDetector
{
public:
	swimmerDetector();
	~swimmerDetector();

	DetectionBox detectSwimmers(cv::Mat frame);

	void configureDetector();

private:

	DetectionBox postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, int frame_num); //TODO KIRAN NOTE  updated the return type
	std::vector<std::string> getOutputsNames(const cv::dnn::Net& net);

	std::string m_classesFile = "classes.names";
	std::string m_modelConfiguration = "yolov3.cfg";
	std::string m_modelWeights = "yolov3.weights";

	cv::dnn::Net m_net;

};

#endif // !SWIMMERDETECTOR_H

