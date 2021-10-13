#include "swimmerDetector.h"
#include <opencv2/opencv.hpp>

#include "fileFinder.h"

swimmerDetector::swimmerDetector()
{
}

swimmerDetector::~swimmerDetector()
{

}

void swimmerDetector::configureDetector()
{
    fileFinder find;
    // Load the network
    m_net = cv::dnn::readNetFromDarknet(find.absolutePath(m_modelConfiguration), find.absolutePath(m_modelWeights));

    m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    //m_net.setPreferableTarget(cv::dnn::DNN_TARGET_OPENCL);
}

std::vector<DetectionBox> swimmerDetector::detectSwimmers(cv::Mat frame)
{
    int inpWidth = 416;
    int inpHeight = 416;

    cv::Mat blob;

    // Create a 4D blob from a frame.
    cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(inpWidth, inpHeight), cv::Scalar(0, 0, 0), true, false);

    //Sets the input to the network
    m_net.setInput(blob);

    // Runs the forward pass to get output of the output layers
    std::vector<cv::Mat> outs;
    m_net.forward(outs, getOutputLayerIDStrings(m_net));

    // Remove the bounding boxes with low confidence
    // Also saves the results in results
    std::vector<DetectionBox> results = postprocess(frame, outs, 1); //TODO is it okay to just set the frame number to one?

    return results;
}


//Remove the bounding boxes with low confidence using non-maxima suppression
//saves the detection results into the classes reuslts var
//This fuction is used in the maek detection file system
std::vector<DetectionBox> swimmerDetector::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, int frame_num) {

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    float confThreshold = 0.5f; //TODO should these be private/public variables in the class?
    float nmsThreshold = 0.3f; //TODO should these be private/public variables in the class?

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            cv::Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    std::vector<int> indices;
    std::vector<DetectionBox> results; //TODO added this - check if works
    DetectionBox temp;

    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    //clear the containers for network preformance
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];

        //save results for later processing
        temp.updateBox(box);
        temp.m_swimmerClass = classIds[idx];
        temp.m_confScore = confidences[idx];
        temp.m_frame = frame_num;
        temp.m_boxID = -1;
        //results.push_back(temp); //TODO we need to return something to save the results
        results.push_back(temp);
    }

    return results;
}


//Get the names of the output layers
//This fuction is used in the make detection file system
std::vector<std::string> swimmerDetector::getOutputLayerIDStrings(const cv::dnn::Net& net)
{
    static std::vector < std::string > names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        std::vector<int> outLayers = net.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        std::vector<std::string> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
    return std::vector<std::string>();
}
