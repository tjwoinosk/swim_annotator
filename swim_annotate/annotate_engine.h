#pragma once

#include <string> 

#include "box_annotate.h"
#include "stroke_annotate.h"

using namespace std;

enum input_options { make_boxes, count_strokes, exit_opt, err_val, waiting_for_request, ship_data};

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
  bool ship_data_for_yolo();

};

