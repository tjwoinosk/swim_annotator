#pragma once

#include <string> 

#include "supper_annotator.h"

using namespace std;

enum input_options {start_work, finish_up, waiting_for_request, exit_opt, err_val};

class annotate_engine {

  string file_name;
  input_options current_request;
  supper_annotator work;

public:

  //constructors
  annotate_engine();
  annotate_engine(string video_file);

  bool add_file_name(string file_name);

  //Waits for the users next request 
  void service_next_request();

  //Checks if app requested to shut down
  bool is_app_finished();

  //Save all application things
  void kill_app();

  //prints the user options
  bool print_general_lab_options();

  //run the supper annotator to anotate the video
  bool run_supper_annotator();

};

