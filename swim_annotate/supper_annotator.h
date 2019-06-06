#pragma once

#include <iostream>
#include <opencv2/opencv.hpp> //displaying video
#include <opencv2/core/types.hpp> //for the rect object
#include <opencv2/highgui.hpp>

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

  string video_file;

  int current_swimmer;
  int number_of_frames;
  int current_frame;

  Mat current_image;

  VideoCapture an_video;

  swim_data* all_data;

public:
  //default constructor. No need for any other definitions. 
  supper_annotator();

  ~supper_annotator();

  //loads the video in video file into the video object 
  //sets the number_of_frames, current_frame to zero, and opens the VideoCapture object
  bool load_video(string video_file);

  //select the lane number of the swimmer you are annotating
  //return prevouse lane number
  bool select_lane_number(int lane_num);

  //Create ROI
  bool create_ROI_in_pool();

  //returns the swim data produced
  swim_data* get_swim_data();

  //move to next frame and save current annotation in all_data 

  //moves to the next frame
  bool next_frame();

  //move to the last frame
  bool last_frame();

  //Go to the frame num specified
  bool go_to_frame(int frame_num);

  //exit supper annotator
  bool quit_and_save_data();

  //create a better prediction
  //bool create_better_prediction();

};