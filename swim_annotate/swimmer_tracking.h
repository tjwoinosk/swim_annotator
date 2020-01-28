#pragma once
#include "box_annotate.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp> //for the rect object
#include <vector>
#include <fstream>

#include "sort_tracker.h"
#include <Eigen/Dense>

using namespace Eigen;

using namespace dnn;
using namespace std;
using namespace cv;

//this will be used to create subvideo
struct track_data {
  int frame_num;
  int object_ID;
  Rect frame_pos;
  double conf_score;
  int class_id;
};


class swimmer_tracking :
  public box_annotate
{

private:

  //var that holds all tracking results for a video
  vector <track_data> results;

  //Get the names of the output layers
  //This fuction is used in the make detection file system
  vector<String> getOutputsNames(const Net& net);

  //For detection system
  float confThreshold = 0.5; // Confidence threshold
  float nmsThreshold = 0.3;  // Non-maximum suppression threshold
  int inpWidth = 416;  // Width of network's input image
  int inpHeight = 416; // Height of network's input image

protected:
  //Accessors
  vector<track_data>* get_results() { return &results; }

public:

  swimmer_tracking();

  //Do tracking using annotations
  //Fill the vector call results with structs representing objects for each swimmer in each frame
  //This fuction will try to ignore swimmers who are not in the race even when
  // they were annotated. The racer should be in vector position 0.
  bool annotation_tracking(string file_name);
  
  //Makes a detection file for tracking algorithums to work with
  //file contains detection information on each frame
  //see MOT2016 challenge for detection file details
  void make_detection_file(string file_name);

  //Remove the bounding boxes with low confidence using non-maxima suppression
  //saves the detection results into the classes reuslts var
  //This fuction is used in the maek detection file system 
  void postprocess(Mat& frame, const vector<Mat>& outs, int frame_num);

  //Save the contents of results into a text file with input name
  //Used by the make detection file fuction to save the contence of results into text file name
  void save_results_in_text_file(string text_file_name);

  //Use SORT algorithum
  //Reads the contence of the detection file and saves the tracked
  //data into the classes reuslts var
  //Needs work as currently the same swimmer get tracked multiple times
  void sort_tracking(string text_file_name);

  //Used for testing and evaluating the tracking algorithum
  //Will produce video of all tracked swimmers results
  void show_video_of_tracking(string file_name);

  //Find the cov mats for kalman filter from ground truth
  //Mat 1 is the cov of the prosess noise denoted as Q
  //Mat 2 is the cov of the observation noise denoted as R
  Mat_<float> calculate_proc_noise_covs();
  //Matrix<float, Dynamic, Dynamic> calculate_proc_noise_covs();

  //Take data and replace all values with accellerations
  //this is used in the noise covs fuction
  //state_num, number of values in the vector of data to find the covariance mat of
  //t, finite difference value for calculating acceleration
  void calc_accelerations(vector<vector<float>> &data, int state_num, float t);

  

  //Use opencv TLD tracking algorithum to track swimmers
  //Use detection to give algorithum an ROI 
  //If tracker fails give new ROI with detection
  //void preform_TLD_tracking(string text_file_name);



  /*inharated class (Box annotate)------
  //default constructor
  box_annotate();

  ~box_annotate();

  int* get_class_stats() { return class_stats; }

  //loads the video in video file into the video object 
  //sets the number_of_frames, current_frame to zero, and opens the VideoCapture object
  //must be called second
  bool load_video_for_boxing(string video_file);

  //returns a pointer to the swim data produced
  //Data warning!! Relative data position in output file is equal to the current frame / skip size!
  // an example can be seen in mark_as_absent(), int(current_frame / skip_size) == int frame_no
  vector<swim_data>* get_swim_data(int frame_no, int lane_no);

  //displays the current frame with or without annotation
  bool display_current_frame();

  //move to next frame 
  //save current annotation in all_data
  //predict the box in the next frame
  bool predict_next_frame();

  //exit supper annotator
  bool quit_and_save_data();

  //prints the annotation options
  //Is used in display current frame automaticly
  //finished
  bool annotation_options(char reply);

  //saves the current_box rect object in the class to the all_data and the text file 
  bool save_annotation();

  //load completed work onto the textfile
  bool update_text_file();

  //changes the current class lable for the box created
  void change_class();

  //sets up the app for start
  //Lets the user select lane number
  //Puts the user at the most recent annotation in that lane
  void start_up();

  //finds the latest annoation for the current lane number
  void find_latest_annotation(bool noise);

  //looks at each lane to see if there is a missed frame
  //if the lane has not been started then it will also not show anything
  void check_for_completion();

  //Tells the program that the swimmer is not in the frame
  void mark_as_absent();

  //reset the tracker
  void reset_tracker();

  //creates new text files and JPEG pictures that YOLO can use 
  //Data saved in hard coaded location
  //    C:/Users/tim_w/Downloads/yolo_swim/JPEGImages/
  //    C:/Users/tim_w/Downloads/yolo_swim/labels/
  //picture_num is the name of the file
  //Update text flage updates the text files
  //Update JPEG flage updates the JPEG files
  bool create_training_set(int* picture_num, bool update_text, bool update_JPEG);

  //changes the current box number
  //Creates a new swimmer in the frame if ROI is selected
  void change_current_box_num();

  Inharated Inharated class (supper annotater)--- 
  //acsessors
  VideoCapture get_video_data() { return an_video; }
  int get_num_frames() { return number_of_frames; }
  int get_current_frame() { return current_frame; }
  string get_video_file() { return video_file_name; }
  int get_skip_size() { return skip_size; }
  int get_current_swimmer() { return current_swimmer; }
  double get_FPS_vid() { return FPS_vid; }
  int get_hight() { return hight; }
  int get_width() { return width; }
  int get_current_class() { return current_class; }

  //loads relavant video information into class
  bool load_video(string video_file);

  //select the lane number of the swimmer you are annotating
  //must be called first
  void select_lane_number();

  //moves to the next frame
  void next_frame();

  //move to the last frame
  void last_frame();

  //Go to the frame num specified
  void go_to_frame();

  //changes the current class lable for the box created
  void change_class();

  //Create ROI
  bool create_ROI_in_pool(Rect* current_box);

  //returns a Mat object holding the current frame
  Mat get_current_Mat();
  */// ---------------------- END
};

