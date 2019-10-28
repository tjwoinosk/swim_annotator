// This code is copied and modified from BigVision LLC. It is based on the OpenCV project. 
//It is subject to the license terms in the LICENSE file found in this distribution and at http://opencv.org/license.html
#include "test_swim_detect_network.h"
#include <list>
#include <iomanip>
#include <math.h>

test_swim_detect_network::test_swim_detect_network()
{
  
}


test_swim_detect_network::test_swim_detect_network(int map_val)
{
  IOU_val = float(map_val) / 100;
  interactive_mode = false;
}


bool compare_hold_data(const hold_data& first, const hold_data& second)
{
  if (first.good_conf > second.good_conf) {
    return true;
  }
  else {
    return false;
  }
}


void test_swim_detect_network::save_network_results(string file_name)
{
  string new_file_name = file_name;
  new_file_name.replace(new_file_name.end() - 4, new_file_name.end(), "_track_mAP.txt");
  ofstream map_results(new_file_name);
  int ii = 0;
  int jj = 0;
  int kk = 0;
  bool lane_data[10];
  int num_good = 0;
  int num_total = 0;
  double std_dev = 0;
  double mean_val = 0;
  float track_mAP = 0;

  //Save mAP results for swimmer tracking
  if(!map_results.is_open()){
    cout << "could not open " << new_file_name << endl;
  }
  //sum up all ap values
  for (ii = 0; ii < track_AVGprecision.size(); ii++) track_mAP += track_AVGprecision[ii];
  if (track_AVGprecision.size() != 0) {
    map_results << "mAP value of finding a swimmer in any class is: " << track_mAP / float(track_AVGprecision.size()) << endl;
  }
  else {
    map_results << "mAP value of finding a swimmer could not be calculated: div by zero" << endl;
  }
  map_results << "Skip size is " << get_skip_size() << endl;
  for (ii = 0; ii < track_AVGprecision.size(); ii++) {
    map_results << "frame " << ii * get_skip_size() << ": ";
    map_results << track_AVGprecision[ii] << endl;
  }
  map_results.close();

  //Save results for failing to find lanes in swimmer tracking
  new_file_name.clear();
  new_file_name = file_name;
  new_file_name.replace(new_file_name.end() - 4, new_file_name.end(), "_track_lanes_missed.txt");
  map_results.open(new_file_name);

  if (!map_results.is_open()) {
    cout << "could not open " << new_file_name << endl;
  }

  map_results << "Lane numbers (zero indexed) missed by the model per frame are:" << endl;
  map_results << "Skip size is " << get_skip_size() << endl;
  for (ii = 0; ii < swimmer_in_lane.size(); ii++) {
    map_results << "frame " << ii*get_skip_size() << ": ";
    for (jj = 0; jj < 10; jj++) {
      if(swimmer_in_lane[ii][jj] == false) map_results << jj << " ";
    }
    map_results << endl;
  }

  map_results.close();

  //Save mAP results for each class
  new_file_name.clear();
  new_file_name = file_name;
  new_file_name.replace(new_file_name.end() - 4, new_file_name.end(), "_class_mAPs.txt");
  map_results.open(new_file_name);
  if (!map_results.is_open()) {
    cout << "could not open " << new_file_name << endl;
  }
  map_results << "Classes: 0 == on_block, 1 == diving, 2 == swimming, 3 == underwater, 4 == turning, 5 == finishing" << endl;
  map_results << "mAP value of finding a swimmer in each class is: " << endl;
  for (ii = 0; ii < 6; ii++) {
    track_mAP = 0;
    int total_mAP = 0;//used to remove true negitives from mAP calculations
    for (jj = 0; jj < class_precision[ii].size(); jj++) {
      if (class_precision[ii][jj] > 0) {//if class_precision[ii][jj] == -1 then true negivtive was found
        track_mAP += class_precision[ii][jj];
        total_mAP++;
      }
    }
    if (total_mAP == 0) {
      map_results << ii << ". NOdata";
    }
    else {
      map_results << ii << ". " << track_mAP / float(total_mAP) << endl;
    }
  }
  map_results << "Skip size is " << get_skip_size() << endl;
  map_results << "AP per frame for each class (-1 represents true negitive):" << endl;

  for (ii = 0; ii < class_precision[0].size(); ii++) map_results << setw(8) << ii*get_skip_size();
  map_results << endl;

  for (ii = 0; ii < 6; ii++) {
    map_results << ii << ". ";
    for (jj = 0; jj < class_precision[ii].size(); jj++) {
      map_results << fixed << setw(6) << setprecision(4) << class_precision[ii][jj];
      if (jj < (class_precision[ii].size() - 1)) {
        map_results << ", ";
      }
    }
    map_results << endl;
  }
  map_results.close();

  //Save lane results for each class
  new_file_name.clear();
  new_file_name = file_name;
  new_file_name.replace(new_file_name.end() - 4, new_file_name.end(), "_class_lanes_missed.txt");
  map_results.open(new_file_name);
  if (!map_results.is_open()) {
    cout << "could not open " << new_file_name << endl;
  }
  map_results << "Classes: 0 == on_block, 1 == diving, 2 == swimming, 3 == underwater, 4 == turning, 5 == finishing" << endl;
  map_results << "Avgerage and std dev of detection rate per class and per lane:" << endl;
  for (ii = 0; ii < 6; ii++) {//class num
    map_results << "Class " << ii << ": ";
    for (jj = 0; jj < 10; jj++) {//lane num
      map_results << "L" << jj << " ";
      //calculate avg finding rate per lane
      num_good = 0;
      num_total = 0;
      //getting mean
      for (kk = 0; kk < swimmer_in_lane_per_class[ii].size(); kk++) {
        if (swimmer_in_lane_per_class[ii][kk][jj] == true) {
          num_good++;
          num_total++;
        }
        else {
          num_total++;
        }
      }
      if (num_total > 1 ) {
        mean_val = double(num_good) / double(num_total);
        //getting std dev
        std_dev = 0;
        for (kk = 0; kk < swimmer_in_lane_per_class[ii].size(); kk++) {
          if (swimmer_in_lane_per_class[ii][kk][jj] == true) {
            std_dev = std_dev + pow((1 - mean_val), 2);
          }
          else {
            std_dev = std_dev + pow((0 - mean_val), 2);
          }
        }
        std_dev = pow(std_dev / (double(num_total)-1),0.5);

        map_results << fixed << setw(4) << setprecision(2) << mean_val << " (";
        map_results << fixed << setw(6) << setprecision(4) << std_dev << ")";
      }
      else {
        map_results << "NoDA";
      }
      if (jj < 9) {
        map_results << ", ";
      }
      else {
        map_results << endl;
      }
    }
  }
  map_results.close();

  //Save lane results for each class
  new_file_name.clear();
  new_file_name = file_name;
  new_file_name.replace(new_file_name.end() - 4, new_file_name.end(), "README.txt");
  map_results.open(new_file_name);
  map_results << "Results are calculated with IOU = " << IOU_val << endl;

  map_results.close();
}


//Take the values predicted by the model and compare with ground truth to produce mAP value
void test_swim_detect_network::compare_results_with_ground(int frame_num)
{
  list<hold_data> predicted_swimmers;//full copy of swimmers predicted by the model
  list<hold_data> sub_pred;//A subset of predicted_swimmers use to store specific class data
  hold_data temp_pusher;//used to load model predictions into predicted swimmers
  vector<swim_data>* ground;//used to load the ground truth into ground swimmers, ground classes, and ground lanes
  //Each index in the following three vectors is connected to the data in each other vector
  vector<Rect> ground_swimmers;//hold complete grounth truth Rects
  vector<int> ground_classes;//hold ground truth class numbers
  vector<int> ground_lane;//hold complete ground truth lane numbers
  //Each index in the floowing two vectors is connected to the data in each other vector
  vector<Rect> sub_grounds;
  vector<int> sub_lanes;

  vector<float> precision_holder;
  vector<float> recall;
  int ii = 0;
  int jj = 0;
  int num_tp = 0;
  float average_precision = 0;
  array<bool,10> lane_results;

  //make a list of found boxes from most confident to least
  if ((good_boxes.size() == good_class_IDs.size()) && (good_class_IDs.size() == good_confs.size())) {
    for (ii = 0; ii < good_confs.size(); ii++) {
      temp_pusher.good_box = good_boxes.at(ii);
      temp_pusher.good_conf = good_confs.at(ii);
      temp_pusher.good_ID = good_class_IDs.at(ii);
      predicted_swimmers.push_back(temp_pusher);
    }
    predicted_swimmers.sort(compare_hold_data);
  }
  else {
    cout << "Error: the data collected in the vectors from frame " << frame_num << " is not consistant" << endl;
    return;
  }

  //get ground truth
  for (ii = 0; ii < 10; ii++) {
    ground = get_swim_data(frame_num, ii);
    if (ground != nullptr) {
      //Make vector of classless boxes for determining tp and fp
      for (jj = 0; jj < ground->size(); jj++) {
        if ((ground->at(jj).box_class != -1) && (ground->at(jj).swimmer_box.area() != 0)) {
          ground_swimmers.push_back(ground->at(jj).swimmer_box);
          ground_classes.push_back(ground->at(jj).box_class);
          ground_lane.push_back(ground->at(jj).lane_num);
        }
      }
    }
    else {
      cout << "Null pointer was returned when loading ground truth data" << endl;
      return;
    }
  }
  
  //Tracking calculations
  num_tp = ground_swimmers.size();
  if (num_tp != 0) {//check if no ground truth values exist
    //calculate ap for tracking
    find_AP_function_for_swimmer_tracking(predicted_swimmers, ground_swimmers, &precision_holder, &recall, ground_lane, &lane_results);
    average_precision = integrate_PR_curve(precision_holder, recall);
    swimmer_in_lane.push_back(lane_results);
    track_AVGprecision.push_back(average_precision);
  }
  else {//number of tp was zero
    //set lane results for this frame to be all true because when there is no tp it is impoible to have somthing go undetected
    for (ii = 0; ii < 10; ii++) lane_results[ii] = true;
    swimmer_in_lane.push_back(lane_results);
    //save AP value for tracking
    if (predicted_swimmers.size() == 0) {
      track_AVGprecision.push_back(1);//If the model says there is nothing in the frame when there is nothing
    }
    else {
      track_AVGprecision.push_back(0);//if it predicts there is somthing when there is nothing
    }
  }

  //class mAP calculations
  for (ii = 0; ii < 6; ii++) {
    make_class_subset(ii, predicted_swimmers, ground_swimmers, ground_classes, ground_lane, &sub_pred, &sub_grounds, &sub_lanes);
    num_tp = sub_grounds.size();
    if (num_tp != 0) {
      find_AP_function_for_swimmer_tracking(sub_pred, sub_grounds, &precision_holder, &recall, sub_lanes, &lane_results);
      average_precision = integrate_PR_curve(precision_holder, recall);
      class_precision[ii].push_back(average_precision);
      swimmer_in_lane_per_class[ii].push_back(lane_results);
    }
    else {//number of tp was zero
      //set lane results for this frame to be all true because when there is no tp it is impoible to have somthing go undetected
      for (jj = 0; jj < 10; jj++) lane_results[jj] = true;
      swimmer_in_lane_per_class[ii].push_back(lane_results);
      //save AP value for this frame and class
      if (sub_pred.size() == 0) {
        class_precision[ii].push_back(-1);//If the model says there is nothing in the frame when there is nothing
      }
      else {
        class_precision[ii].push_back(0);//if it predicts there is somthing when there is nothing
      }
    }
  }
}


//Do the AP calculation for tracking swimmers in general
//also find lanes that were not detected and saves them in the class
void test_swim_detect_network::find_AP_function_for_swimmer_tracking(list<hold_data> prediction,
  vector<Rect> grounds, vector<float>* precision, vector<float>* recall, vector<int> ground_lane, array<bool,10>* lane_results)
{
  float temp_inter = 0;
  float temp_union = 0;
  float highest_iou = 0;
  float num_tp = float(grounds.size());
  float temp_iou = 0;
  int index_of_high_iou = 0;
  int ii = 0;
  int jj = 0;
  int tp = 0;
  int fp = 0;
  list<hold_data>::iterator iter;
  float average_precision = 0;

  recall->clear();
  precision->clear();

  //Do the AP calculation
  ii = 1;//number of hits produced
  //Take each swimmer predicted and check it with the ground truths
  for (iter = prediction.begin(); iter != prediction.end(); iter++) {
    highest_iou = 0;
    temp_iou = 0;
    index_of_high_iou = 0;
    //look threw all ground truth boxes to find the best match
    for (jj = 0; jj < grounds.size(); jj++) {
      temp_inter = (iter->good_box & grounds.at(jj)).area();
      temp_union = (iter->good_box | grounds.at(jj)).area();
      temp_iou = temp_inter / temp_union;
      if (temp_iou > highest_iou) {
        highest_iou = temp_iou;
        index_of_high_iou = jj;
      }
    }
    if ((highest_iou) > IOU_val) {
      //tp found
      tp++;
      grounds.at(index_of_high_iou) = Rect(0, 0, 1, 1); //make the box zero so it wont be used again
    }
    precision->push_back(float(tp) / float(ii));
    recall->push_back(float(tp) / num_tp);
    ii++;
  }
  //Edit the precision vector it removes the "wiggel" in the PR curve
  for (ii = (precision->size() - 2); ii >= 0; ii--) {//dont worry about the last value becuase it has no value to its right
    if (precision->at(ii) < precision->at(ii + 1)) {
      precision->at(ii) = precision->at(ii + 1);
    }
  }

  //initalize the array of bools to be all true so that only lanes that were accutaly missed will be false
  //rather than having lanes the didnt have swimmers in them be falce
  for (ii = 0; ii < 10; ii++) lane_results->at(ii) = true;
  //find which lanes where not found
  for (ii = 0; ii < grounds.size(); ii++) {
    if (grounds[ii] != Rect(0, 0, 1, 1)) lane_results->at(ground_lane[ii]) = false;//swimmer was missed
  }
  return;
}


//Make subsets of ground data based on thier class for the ap fuction finder to use
void test_swim_detect_network::make_class_subset(int class_num, list<hold_data> prediction, vector<Rect> grounds,
  vector<int> ground_classes, vector<int> ground_lanes, list<hold_data>* sub_pred, vector<Rect>* sub_grounds, vector<int>* sub_lanes) 
{
  list<hold_data>::iterator itor;
  int ii = 0;

  //clear the vector classes for the new class
  sub_grounds->clear();
  sub_pred->clear();
  sub_lanes->clear();
  //Fill sub_pred and sub_grounds and sub_lanes with the correct values
  //Go threw prediction and find the matching class_num 
  for (itor = prediction.begin(); itor != prediction.end(); itor++) {
    if (itor->good_ID == class_num) {
      sub_pred->push_back(*itor);
    }
  }
  //Go threw ground truth and find the matching class_num
  if ((ground_classes.size() == grounds.size()) && (grounds.size() == ground_lanes.size())) {
    for (ii = 0; ii < ground_classes.size(); ii++) {
      if (ground_classes[ii] == class_num) {
        sub_grounds->push_back(grounds[ii]);
        sub_lanes->push_back(ground_lanes[ii]);
      }
    }
  }
  else {
    cout << "ERROR: Ground classes, grounds, and ground_lanes are not all equal in size" << endl;
  }
}


//Do the integration of the precision recall vectors
float test_swim_detect_network::integrate_PR_curve(vector<float> precision, vector<float> recall)
{
  float average_precision = 0;
  int ii = 0;
  int jj = 0;

  //Now that we have the PR graph in vectors precision and rcall we must find the area under the curve they make
  //asume size of precision == size of recall
  if ((recall.size() > 0) && recall[0] != 0) {//catch the area between the y origin and the frist recall value, if it exists
    average_precision = precision[0] * recall[0];
  }
  //intagrate the rest of the data (the data is a special case where there are only drops and no slanted lines)
  ii = 0;
  while (ii < precision.size()) {
    jj = 1;
    //look for runs of the same value in the precision vector
    while (((ii + jj) < precision.size()) && (precision[ii + jj] == precision[ii])) {
      jj++;
    }
    if (ii + jj >= precision.size()) {//At the end of the curve 
      average_precision += (precision[ii] * (recall[ii + (jj - 1)] - recall[ii]));
    }
    else {
      average_precision += (precision[ii] * (recall[ii + jj] - recall[ii]));//calculate box area
    }
    ii += jj;//skip the run we just found to look for the next run
  }

  return average_precision;
}


//Used to test how the predicted resutls look against the ground results
//comment in get_network_results to remove/add this function
void test_swim_detect_network::add_ground_to_video(Mat& frame, int frame_num)
{
  vector<swim_data>* ground;//used to load the ground truth into ground swimmers, ground classes, and ground lanes
  vector<Rect> ground_swimmers;//hold complete grounth truth Rects
  vector<int> ground_classes;//hold ground truth class numbers

  int ii = 0;
  int jj = 0;
  int blue, green, red = 0;
  //get ground for the frame
  for (ii = 0; ii < 10; ii++) {
    ground = get_swim_data(frame_num, ii);
    if (ground != nullptr) {
      //Make vector of classless boxes for determining tp and fp
      for (jj = 0; jj < ground->size(); jj++) {
        if ((ground->at(jj).box_class != -1) && (ground->at(jj).swimmer_box.area() != 0)) {
          ground_swimmers.push_back(ground->at(jj).swimmer_box);
          ground_classes.push_back(ground->at(jj).box_class);
        }
      }
    }
    else {
      cout << "Null pointer was returned when loading ground truth data" << endl;
      return;
    }
  }

  //Draw a rectangle displaying the bounding box
  for (ii = 0; ii < ground_swimmers.size(); ii++) { 
    
    switch (ground_classes[ii]) {
    case 0://Brown
      blue = 0; red = 100; green = 50;
      break;
    case 1://red
      blue = 0; red = 255; green = 0;
      break;
    case 2://green
      blue = 0; red = 0; green = 255;
      break;
    case 3://orange
      blue = 0; red = 255; green = 127;
      break;
    case 4://yellow
      blue = 0; red = 255; green = 255;
      break;
    case 5://purple
      blue = 255; red = 128; green = 0;
      break;
    default:
      break;
    }
    rectangle(frame, ground_swimmers[ii], Scalar(blue, green, red), 2, 1); 
  }
}


//Main working fuction!! every other fuction is used in this definition. 
void test_swim_detect_network::get_network_results(string file_name)
{
  // Load names of classes
  string classesFile = "classes.names";
  ifstream ifs(classesFile.c_str());
  string line;
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
  string str, outputFile;
  VideoCapture cap;
  VideoWriter video;
  Mat frame, blob;
  int frame_num = 0;

  str = file_name;

  if (load_video_for_boxing(str)) {
    // Open the video file
    cap.open(str);
    str.replace(str.end() - 4, str.end(), "_yolo_out_cpp.avi");
    outputFile = str;
    destroyWindow(AN_WINDOW_NAME);
  } 
  else {
    cout << "Could not open the input image/video stream" << endl;
    return;
  }

  // Get the video writer initialized to save the output video
  video.open(outputFile, VideoWriter::fourcc('M', 'J', 'P', 'G'), 28, Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)));

  // Create a window
  static const string kWinName = "Deep learning object detection in OpenCV";
  namedWindow(kWinName, WINDOW_NORMAL);

  //Get mAP value to compute with
  if (interactive_mode) {
    do
    {
      cout << "What IOU value would you like to do the mAP calcualtions with (1 - 99)? ";
      cin >> iou_val_user;
      cout << endl;
    } while ((iou_val_user > 99) & (iou_val_user < 1));
    IOU_val = float(iou_val_user) / 100;
  }

  //Set the starting frame to be analized 
  //cap.set(CAP_PROP_POS_FRAMES, 0);

  //Process frames.
  while (waitKey(1) < 0)
  {
    // get frame from the video
    cap >> frame;
    
    // Stop the program if reached end of video
    if (frame.empty()) {
      cout << "Done processing !!!" << endl;
      cout << "Output file is stored as " << outputFile << endl;
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
    postprocess(frame, outs);
    
    // Put efficiency information. The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    string label = format("Inference time for a frame : %.2f ms", t);
    putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

    //For dealing with the fact that we skip frames when lableing data
    frame_num = cap.get(CAP_PROP_POS_FRAMES);
    if ((frame_num % get_skip_size()) == 0) {
      compare_results_with_ground(frame_num / get_skip_size());
      //add ground to the video for testing
      //class 0: Brown, class 1: Red, class 2: Green, class 3: Orange, class 4: Yellow, class 5: Purple
      //add_ground_to_video(frame, frame_num / get_skip_size());
    }

    // Write the frame with the detection boxes
    Mat detectedFrame;
    frame.convertTo(detectedFrame, CV_8U);
    video.write(detectedFrame);
    imshow(kWinName, frame);
    
    //Ends work early for debuging
    /*
    if ((frame_num % 50) == 49) {
      break;
    }
    //*/
  }

  cap.release();
  video.release();
  destroyWindow(kWinName);

  return;
}


//Remove the bounding boxes with low confidence using non-maxima suppression
void test_swim_detect_network::postprocess(Mat& frame, const vector<Mat>& outs)
{
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
  NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
  //clear the containers for network preformance
  good_class_IDs.clear();
  good_boxes.clear();
  good_confs.clear();
  for (size_t i = 0; i < indices.size(); ++i)
  {
    int idx = indices[i];
    Rect box = boxes[idx];

    //save results for later processing
    good_boxes.push_back(box);
    good_class_IDs.push_back(classIds[idx]);
    good_confs.push_back(confidences[idx]);

    drawPred(classIds[idx], confidences[idx], box.x, box.y,
      box.x + box.width, box.y + box.height, frame);
  }
}


//Draw the predicted bounding box
void test_swim_detect_network::drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
  //Draw a rectangle displaying the bounding box
  rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 3);

  //Get the label for the class name and its confidence
  string label = format("%.2f", conf);
  if (!classes.empty())
  {
    CV_Assert(classId < (int)classes.size());
    label = classes[classId] + ":" + label;
  }

  //Display the label at the top of the bounding box
  int baseLine;
  Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
  top = max(top, labelSize.height);
  rectangle(frame, Point(left, top - round(1.5 * labelSize.height)), Point(left + round(1.5 * labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
  putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 1);
}


//Get the names of the output layers
vector<String> test_swim_detect_network::getOutputsNames(const Net& net)
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



