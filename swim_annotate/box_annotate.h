#pragma once
#include "supper_annotator.h"
#include <iomanip>
#include <vector>

using namespace std;
using namespace cv;

//------------------ 1 ------- 2 ------ 3 -------- 4 ------- 5 ------- 6 --------------------------------
enum class_names { on_block, diving, swimming, underwater, turning, finishing }; //the six possible classes, redundant...

struct swim_data
{
  Rect swimmer_box;
  int box_class;
  int lane_num;
};

class box_annotate :
  public supper_annotator
{
private:

  //stuff for box annotator
  Rect current_box;
  int current_box_num;//zero indexed 
  int num_possible_data_lines;
  int current_class;

  //tracker data
  Ptr<TrackerKCF> tracker;
  bool good_track;
  bool fast_ROI_mode;

  //Annotation data
  vector<swim_data>** all_data;

public:

  //default constructor
  box_annotate();

  ~box_annotate();

  //loads the video in video file into the video object 
  //sets the number_of_frames, current_frame to zero, and opens the VideoCapture object
  //must be called second
  bool load_video_for_boxing(string video_file);

  //returns a pointer to the swim data produced
  //Data warning!! Relative data position in output file is equal to the current frame / skip size!
  // an example can be seen in mark_as_absent(), int(current_frame / skip_size) == int frame_no
  vector<swim_data>* get_swim_data(int frame_no, int lane_no);

  //displays the current frame with or without annotation
  //works
  bool display_current_frame();

  //move to next frame 
  //save current annotation in all_data
  //predict the box in the next frame
  void predict_next_frame();

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
  bool create_training_set(int *picture_num, bool update_text, bool update_JPEG);

  //changes the current box number
  //Creates a new swimmer in the frame if ROI is selected
  void change_current_box_num();

  /*inharated class------

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
  //finished
  void next_frame();

  //move to the last frame
  //finished
  void last_frame();

  //Go to the frame num specified
  //finished
  void go_to_frame();

  //changes the current class lable for the box created
  void change_class();

  //Create ROI
  bool create_ROI_in_pool(Rect* current_box);

  //returns a Mat object holding the current frame
  Mat get_current_Mat();
  */// ---------------------- END

};

