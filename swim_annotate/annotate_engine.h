#pragma once

#include <string> 

#include "box_annotate.h"
#include "stroke_annotate.h"

using namespace std;

enum input_options {make_boxes, count_strokes, exit_opt, err_val, waiting_for_request, 
  ship_data, ship_data_with_pics, analize_swim_detect, make_subvid};

class annotate_engine {

  string file_name;
  input_options current_request;

public:

  //constructors
  annotate_engine();
  annotate_engine(string video_file);

  //Waits for the users next request 
  void service_next_request();

  //Checks if app requested to shut down
  bool is_app_finished();

  //Save all application things
  void kill_app();

  //prints the user options
  bool print_general_lab_options();

  //run the supper annotator to anotate the video
  bool run_box_annotator();

  bool run_stroke_annotator();

  //create data for yolo to use
  //open application with the smallest named file
  bool ship_data_for_yolo(bool func_update_JPEG);

  //produce analytics for the current detection network
  bool analize_swimmer_detection_netowrk();

  //produce video of what netowrk sees
  //produce a summary of results
  //if interactive mode iou is not used
  //iou is the inter value of the iou desired muliptied by 100 
  bool analize_swimmer_detection_netowrk_non_inter(int iou);

  //Using annotations of swimmers make subvideo of each swimmer
  void make_sub_vid_from_annotations();

};

