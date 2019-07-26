#pragma once

#include <iostream>
#include <fstream>//file manipulation
#include <vector>
#include <errno.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp> //for the rect object
#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/tracking.hpp>

constexpr auto AN_WINDOW_NAME = "Annotating Window";

using namespace std;
using namespace cv;


class supper_annotator
{
private:
  
  //video display data
  int number_of_frames;
  int current_frame;
  string video_file_name;
  int skip_size; //how many frames to skip every new frame
  VideoCapture an_video;
  bool video_file_open;
  double FPS_vid;
  int hight;
  int width;

  //annotation data
  int current_swimmer;//lane number of swimmer, zero indexed to the pool 
  //So a 10 lane pool would have lanes 0-9, but an 8 and 6 lane pool would not be zero indexed 

public:
  
  //default constructor. No need for any other definitions. 
  supper_annotator();
  supper_annotator(int class_skip_size);

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

  //setters
  void set_skip_size(int set_skip) { skip_size = set_skip; return; }
  void set_current_frame(int frame_num) { current_frame = frame_num; }

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

  //Create ROI
  bool create_ROI_in_pool(Rect* current_box);

  //returns a Mat object holding the current frame  
  Mat get_current_Mat();
  
  void quit_app();

};