#include "swimmerDetector.h"
#include <opencv2/opencv.hpp>

swimmerDetector::swimmerDetector()
{
}

swimmerDetector::~swimmerDetector()
{

}

TrackingBox swimmerDetector::detectSwimmers(cv::Mat frame)
{
  /*
  std::string classesFile = "classes.names";
  std::string modelConfiguration = "yolov3.cfg";
  std::string modelWeights = "yolov3.weights";

  int inpWidth, inpHeight;

  // Load the network
  cv::dnn::Net net = cv::dnn::readNetFromDarknet(modelConfiguration, modelWeights);
  net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
  net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

  cv::Mat blob;

  // Create a 4D blob from a frame.
  cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(inpWidth, inpHeight), cv::Scalar(0, 0, 0), true, false);

  //Sets the input to the network
  net.setInput(blob);

  // Runs the forward pass to get output of the output layers
  std::vector<cv::Mat> outs;
  //net.forward(outs, getOutputsNames(net));

  // Remove the bounding boxes with low confidence
  // Also saves the results in results
  postprocess(frame, outs, 1);
  */
  return TrackingBox();
}

/*
//Remove the bounding boxes with low confidence using non-maxima suppression
//saves the detection results into the classes reuslts var
//This fuction is used in the maek detection file system
void swimmerDetector::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, int frame_num) {
 
  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
  float confThreshold = 0.5;
  float nmsThreshold = 0.3;

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
  TrackingBox temp;

  cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
  //clear the containers for network preformance
  for (size_t i = 0; i < indices.size(); ++i)
  {
    int idx = indices[i];
    cv::Rect box = boxes[idx];

    //save results for later processing
    temp.frame_pos = box;
    temp.class_id = classIds[idx];
    temp.conf_score = confidences[idx];
    temp.frame_num = frame_num;
    temp.object_ID = -1;
    results.push_back(temp);
  }
  
}


//Get the names of the output layers
//This fuction is used in the make detection file system
std::vector<std::string> swimmerDetector::getOutputsNames(const cv::dnn::Net& net)
{
  
  static vector<String> names;
  if (names.empty())
  {
    //Get the indices of the output layers, i.e. the layers with unconnected outputs
    vector<int> outLayers = net.getUnconnectedOutLayers();

    //get the names of all the layers in the network
    vector<String> layersNames = net.getLayerNames();

    // Get the names of the output layers in names
    names.resize(outLayers.size());
    for (size_t i = 0; i < outLayers.size(); ++i)
      names[i] = layersNames[outLayers[i] - 1];
  }
  return names;
  
  return std::vector<std::string>();
}

*/

/*
//Makes a detection file for tracking algorithums to work with
//file contains detection information on each frame
//see MOT2016 challenge for detection file details
void swimmer_tracking::make_detection_file(string file_name)
{
  // Load names of classes
  string classesFile = "classes.names";
  ifstream ifs(classesFile.c_str());
  string line;
  vector<string> classes;

  while (getline(ifs, line)) classes.push_back(line);
  int iou_val_user = 0;

  // Give the configuration and weight files for the model
  String modelConfiguration = "yolov3.cfg";
  String modelWeights = "yolov3.weights";

  // Load the network
  Net net = readNetFromDarknet(modelConfiguration, modelWeights);
  net.setPreferableBackend(DNN_BACKEND_OPENCV);
  net.setPreferableTarget(DNN_TARGET_CPU);

  // Open a video file or an image file or a camera stream.
  string str;
  VideoCapture cap;
  VideoWriter video;
  Mat frame, blob;


  str = file_name;
  cap.open(str);
  str.replace(str.end() - 4, str.end(), "_det.txt");

  int finaly = cap.get(CAP_PROP_FRAME_COUNT) / 10;

  //Set the starting frame to be analized 
  //cap.set(CAP_PROP_POS_FRAMES, 0);

  int frame_num = cap.get(CAP_PROP_POS_FRAMES);
  //Process frames.
  while (waitKey(1) < 0)
  {
    // get frame from the video
    cap >> frame;

    // Stop the program if reached end of video
    if (frame.empty()) {
      cout << "Done creating detection file !!!" << endl;
      cout << "Output file is stored as " << str << endl;
      waitKey(3000);
      break;
    }

    // Create a 4D blob from a frame.
    blobFromImage(frame, blob, 1 / 255.0, Size(inpWidth, inpHeight), Scalar(0, 0, 0), true, false);

    //Sets the input to the network
    net.setInput(blob);

    // Runs the forward pass to get output of the output layers
    vector<Mat> outs;
    net.forward(outs, getOutputsNames(net));

    // Remove the bounding boxes with low confidence
    // Also saves the results in results
    postprocess(frame, outs, frame_num);

    if ((frame_num % finaly) == 0) {
      cout << "Another 10% completed... " << endl;
    }

    frame_num = cap.get(CAP_PROP_POS_FRAMES);
  }

  cap.release();

  //Save results in text file...
  save_results_in_text_file(str);

  return;

}


//Remove the bounding boxes with low confidence using non-maxima suppression
//saves the detection results into the classes reuslts var
//This fuction is used in the maek detection file system
void swimmer_tracking::postprocess(Mat& frame, const vector<Mat>& outs, int frame_num) {
  vector<int> classIds;
  vector<float> confidences;
  vector<Rect> boxes;

  for (size_t i = 0; i < outs.size(); ++i)
  {
    // Scan through all the bounding boxes output from the network and keep only the
    // ones with high confidence scores. Assign the box's class label as the class
    // with the highest score for the box.
    float* data = (float*)outs[i].data;
    for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
    {
      Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
      Point classIdPoint;
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
        boxes.push_back(Rect(left, top, width, height));
      }
    }
  }

  // Perform non maximum suppression to eliminate redundant overlapping boxes with
  // lower confidences
  vector<int> indices;
  track_data temp;

  NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
  //clear the containers for network preformance
  for (size_t i = 0; i < indices.size(); ++i)
  {
    int idx = indices[i];
    Rect box = boxes[idx];

    //save results for later processing
    temp.frame_pos = box;
    temp.class_id = classIds[idx];
    temp.conf_score = confidences[idx];
    temp.frame_num = frame_num;
    temp.object_ID = -1;
    results.push_back(temp);
  }
}


//Get the names of the output layers
//This fuction is used in the make detection file system
vector<String> swimmer_tracking::getOutputsNames(const Net& net)
{
  static vector<String> names;
  if (names.empty())
  {
    //Get the indices of the output layers, i.e. the layers with unconnected outputs
    vector<int> outLayers = net.getUnconnectedOutLayers();

    //get the names of all the layers in the network
    vector<String> layersNames = net.getLayerNames();

    // Get the names of the output layers in names
    names.resize(outLayers.size());
    for (size_t i = 0; i < outLayers.size(); ++i)
      names[i] = layersNames[outLayers[i] - 1];
  }
  return names;
}


*/