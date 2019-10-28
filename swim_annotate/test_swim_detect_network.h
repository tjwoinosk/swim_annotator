// This code is copied and modified from BigVision LLC. It is based on the OpenCV project. 
//It is subject to the license terms in the LICENSE file found in this distribution and at http://opencv.org/license.html
#pragma once
#include "box_annotate.h"
#include <string>

using namespace dnn;
using namespace std;
using namespace cv;

struct hold_data
{
  Rect good_box;
  float good_conf;
  int good_ID;
};

class test_swim_detect_network :
  public box_annotate
{

private:

  //option flags
  bool interactive_mode = true;

  //Class paramiters
  float IOU_val = 0.25; //For calculating mAP values
  float confThreshold = 0.5; // Confidence threshold
  float nmsThreshold = 0.3;  // Non-maximum suppression threshold
  int inpWidth = 416;  // Width of network's input image
  int inpHeight = 416; // Height of network's input image
  vector<string> classes;
  //Store each frames result in these vectors
  vector<Rect> good_boxes;
  vector<int> good_class_IDs;
  vector<float> good_confs;
  //Store overall preformace for each frame
  vector<float> track_AVGprecision;
  vector<float> class_precision[6]; //0 == on_block, 1 == diving, 2 == swimming, 3 == underwater, 4 == turning, 5 == finsihing
  vector<array<bool,10>> swimmer_in_lane;
  vector<array<bool,10>> swimmer_in_lane_per_class[6];

  // Remove the bounding boxes with low confidence using non-maxima suppression
  void postprocess(Mat& frame, const vector<Mat>& out);

  // Draw the predicted bounding box
  void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);

  // Get the names of the output layers
  vector<String> getOutputsNames(const Net& net);

  //compare frame preditions with ground truth data
  void compare_results_with_ground(int frame_num);

  //Do the AP calculation for tracking swimmers in general
  //also find lanes that were not detected and saves them in the class
  void find_AP_function_for_swimmer_tracking(list<hold_data> prediction, vector<Rect> grounds, vector<float>* 
    precision, vector<float>* recall, vector<int> ground_lane, array<bool, 10>* lane_results);

  //Make subsets of ground data based on thier class for the ap fuction finder to use
  void make_class_subset(int class_num, list<hold_data> prediction, vector<Rect> grounds,
    vector<int> ground_classes, vector<int> ground_lanes, list<hold_data>* sub_pred, vector<Rect>* sub_grounds, vector<int>* sub_lanes);

  //Do the integration of the precision recall vectors
  float integrate_PR_curve(vector<float> precision, vector<float> recall);

  //fuction to add ground truth to video to confirm that data analysis is working correctly
  //class 0: Brown, class 1: Red, class 2: Green, class 3: Orange, class 4: Yellow, class 5: Purpel
  void add_ground_to_video(Mat& frame, int frame_num);

public:

  test_swim_detect_network();
  test_swim_detect_network(int map_val);

  void get_network_results(string file_name);
  void save_network_results(string file_name);

};

