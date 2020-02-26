#pragma once

#include <opencv2/videoio/videoio.hpp> //displaying video
#include <opencv2/opencv.hpp> //displaying video
#include <math.h>

#include "graph_drawing.h"
#include "sinusoid_maker.h"
#include "SA_file_mannager.h"

using namespace std;
using namespace cv;

class stroke_annotate 
{

private:

  VideoCapture cap;
  SA_file_mannager man_file;
  graph_drawing grapher;

  string video_file = "uninit.mp4";

  //video display
  string video_window_name = "uninit_name.mp4";
  double video_speed = 1;//A fraction of the original FPS of the video
  const int num_skip_back = 5;
  const double min_speed = 6; //equivilant to 16x slower (must be grater than zero)
  const double max_speed = 1; //equivilant to 2x faster (must be grater than zero)

  //data
  bool swimmer_is_swimming = false;

  //Show options for vid once
  //wait for user to choose one
  //Also display in video options
  void print_vid_dialog();

  //Start annotating video (edit mode)
  //Fist ask if swimming is swimming or not, set swimmer_is_swimming accordingly (edit mode)
  //View any annotations and/or watch video (view mode)
  void annotate_video(bool is_edit_mode);

  //Specify stroke being prefomed in video (If not already spcifed ask to change)
  //Save all work in file
  //kill all windows
  void quit_stroke_annotator();

  //prints options when editing or viewing stroke annotations
  void print_video_options(bool in_view_mode);

  //asks user to input if swimmer is swimming or not
  void get_swimmer_stait();

  void change_speed(bool speed_up);

  //flips the swimmer_is_swimming flag
  void toggel_swimming();

  //Changes cntr_end and cntr_star aproprately
  //Modifies the data in the grapher object
  void skip_back(int n_storkes, int &cntr_end, int &cntr_start, bool in_edit_mode);

  //pause the video
  void toggel_pause() { while (waitKey(0) != 'p'); }

  //Tells the grapher object that a stroke occured
  //Upates the grapher accordingly
  void mark_stroke(int& cntr_start, int& cntr_end);


public:

  //constructors

  //exmple for how to use the graphing class
  void graph_example();

  //exmpale for how to use the file class
  void file_example();

  //loads and opens all objects required for stroke annotation 
  //Display all windows
  void start_stroke_counting(string video_file);
  
};