#pragma once
#include "box_annotate.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp> //for the rect object
#include <vector>

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

const int MAX_SORT = 100000;

class swimmer_tracking :
  public box_annotate
{

private:
  //var that holds all tracking results for a video
  vector<track_data> results;
  //holds pointers to desired values in results
  track_data *sorted_data[MAX_SORT];//his can hold up to 5 minutes of frames @30 fps

public:

  swimmer_tracking();

  //Accessors
  track_data** get_track_data() { return sorted_data; }

  //Returns an in order array of pointers that point to the lane number requested 
  void make_res_values(int lane_number);

  //Do tracking using annotations
  //Fill the vector call results with structs representing objects for each swimmer in each frame
  //This fuction will try to ignore swimmers who are not in the race even when
  // they were annotated. The racer should be in vector position 0.
  void annotation_tracking(string file_name);
  

  //Do tracking... Use other peoples work as much as possible




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

