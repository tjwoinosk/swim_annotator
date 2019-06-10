#pragma once

#include <iostream>
#include <fstream>//file manipulation


#include <opencv2/opencv.hpp> //displaying video
#include <opencv2/core/types.hpp> //for the rect object
#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>

using namespace std;
using namespace cv;

enum class_names {on_block, diving, swimming, underwater, turning, finishing}; //the six possible classes

struct swim_data
{
  Rect swimmer_box;
  class_names box_class;
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
  int current_swimmer;
  string video_file;
  Rect current_box;
  int skip_size; //how many frames to skip every new frame

  //Annotation data
  swim_data* all_data;

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

  //returns the swim data produced
  swim_data* get_swim_data();

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


  //create a better prediction
  //bool create_better_prediction();

};