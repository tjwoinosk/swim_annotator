#pragma once

#include <iostream>
#include <fstream>//file manipulation
#include <vector>


#include <opencv2/opencv.hpp> //displaying video
#include <opencv2/core/types.hpp> //for the rect object
#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>

using namespace std;
using namespace cv;
//----------------- 1 ------- 2 ------ 3 --------- 4 ------- 5 ------- 6 --------------------------------
enum class_names {on_block, diving, swimming, underwater, turning, finishing}; //the six possible classes

struct swim_data
{
  Rect swimmer_box;
  int box_class;
  int lane_num;
};

class supper_annotator
{
private:
  //processes data
  int number_of_frames;
  
  //video display data
  VideoCapture an_video;
  int current_frame;
  int current_swimmer;//lane number of swimmer
  int current_class;//current class of the annotaion
  string video_file;
  Rect current_box;
  int skip_size; //how many frames to skip every new frame

  //Annotation data
  swim_data **all_data;

public:
  //default constructor. No need for any other definitions. 
  supper_annotator();

  ~supper_annotator();

  //select the lane number of the swimmer you are annotating
  //must be called first 
  //Finished
  void select_lane_number();

  //loads the video in video file into the video object 
  //sets the number_of_frames, current_frame to zero, and opens the VideoCapture object
  //must be called second
  //
  bool load_video(string video_file);

  //Create ROI
  //finished
  bool create_ROI_in_pool();

  //returns a pointer to the swim data produced
  swim_data* get_swim_data(int frame_no, int lane_no);

  //displays the current frame with or without annotation
  //works
  bool display_current_frame();

  //move to next frame 
  //save current annotation in all_data
  //predict the box in the next frame
  void predict_next_frame();

  //moves to the next frame
  //finished
  void next_frame();

  //move to the last frame
  //finished
  void last_frame();

  //Go to the frame num specified
  //finished
  void go_to_frame();

  //exit supper annotator
  bool quit_and_save_data();

  //prints the annotation options
  //Is used in display current frame automaticly
  //finished
  bool annotation_options();

  //saves the current_box rect object in the class to the all_data and the text file 
  bool save_annotation();

  //load completed work onto the textfile
  bool update_text_file();

  //changes the current class lable for the box created
  void change_class();


};