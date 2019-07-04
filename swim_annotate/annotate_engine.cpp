#include <string>
#include <iostream>
#include <ctype.h>

#include "annotate_engine.h"

using namespace std;

annotate_engine::annotate_engine() {
  //make up a defalt root directory for inizalization 
  file_name = "nothing_yet";
  current_request = waiting_for_request;
}


annotate_engine::annotate_engine(string video_file) {
  //make up a defalt root directory for inizalization 
  file_name = video_file;
  current_request = waiting_for_request;
}

//Waits for the users next request 
void annotate_engine::service_next_request() {

  switch (current_request) {
  case make_boxes:
    //start image annotate class
    if (!run_box_annotator()) {
      cout << "Could not run box annotator with this file" << endl;
    }
    break;
  case count_strokes:
    run_stroke_annotator();
    break;
  case exit_opt:
    current_request = exit_opt;
    break;
  default:
    break;
  }
}

//Checks if app requested to shut down
bool annotate_engine::is_app_finished() {
  if (current_request == exit_opt) return true;
  return false;
}

//Save all application things
void annotate_engine::kill_app() {
  cout << "Killing app!" << endl;
}


//prints the user options
bool annotate_engine::print_general_lab_options() {
  char answer;
  int ans = 0;

  cout << "\nSelect from the following options:" << endl << endl;
  cout << "Start Annotating Video With Boxes, press (1)" << endl;
  cout << "Start Counting Strokes, press (2)" << endl;
  cout << "Quit Labelling And Exit, press (3)" << endl;
  cout << "\nlab>> ";
  cin >> answer;

  if (!isdigit(answer)) {
    return false;
  } else {
    ans = int(answer) - 48;//convert to int
  }
 
  switch (ans) {
  case 1: current_request = make_boxes;
    break;
  case 2: current_request = count_strokes;
    break;
  case 3: current_request = exit_opt;
    break;
  default: current_request = err_val;
    cout << "An unrecognised value was input\n";
    return false;
  }

  return true;
}

//run the supper annotator to anotate the video
bool annotate_engine::run_box_annotator() {
  
  box_annotate box_work;
  cout << "Loading file for box annotation!" << endl;
  if (box_work.load_video_for_boxing(file_name)) {
    box_work.start_up();
    while (box_work.display_current_frame()) {
      //keep looping
    }
    return true;
  }
  return false;
}

bool annotate_engine::run_stroke_annotator() {

  stroke_annotate stroke_work;

  if (!stroke_work.load_video_for_stroke_counting(file_name)) {
    cout << "could not load video for stroke counting" << endl;
    return false;
  }
  else {
    cout << "Loading file for stroke annotation!" << endl;
    stroke_work.play_video();
    return true;
  }
}

