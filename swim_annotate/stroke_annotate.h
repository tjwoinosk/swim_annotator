#pragma once
#include "supper_annotator.h"
#include <list>

using namespace std;
using namespace cv;

struct swim_data_stroke {
  int cycle;//frame number
  int stroke_spec; //fly == 1, back_right == 2, back_left == 20,  breast == 3, free_right == 4, free_left == 40
};

class stroke_annotate :
  public supper_annotator
{
private:

  //Viewing variables
  int video_speed;//speed at which video plays (1x, 1/2x, 1/3x, ...)
  int max_speed;
  int min_speed;

  //Annotation data
  list<swim_data_stroke> stroke_data[10];
  int current_class;

public:

  //default constructor
  stroke_annotate();

  //loads the video in video file into the video object 
  //sets the number_of_frames, current_frame to zero, and opens the VideoCapture object
  //must be called second
  bool load_video_for_stroke_counting(string video_file);

  //displays the current frame with or without annotation
  //works
  bool play_video();

  //change annotation class 
  void change_class();

  //exit supper annotator
  bool quit_and_save_data();

  //saves the current_box rect object in the class to the all_data and the text file 
  void save_annotation(bool is_right);

  //load completed work onto the textfile
  bool update_text_file();

  //sets up the app for start
  void start_up();

  //pause video
  void pause_video();

  //change the viewing speed of the video 
  void change_video_speed(bool increase_speed);

  //search of the annotation and deletes it if and_delete is true
  bool check_for_annotation(int input_current_lane, int input_current_frame, bool and_delete);

  //goes to the most recently annotated frame
  void go_to_most_recent_annotation();

  void print_play_vid_dialog();


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

