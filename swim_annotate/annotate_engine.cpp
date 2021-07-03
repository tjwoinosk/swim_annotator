#include <string>
#include <iostream>
#include <ctype.h>

#include "annotate_engine.h"
#include "test_swim_detect_network.h"
#include "sub_video.h"
#include "graph_drawing.h"
#include "sinusoid_maker.h"

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
  case ship_data_with_pics:
    ship_data_for_yolo(true);
    break;
  case ship_data:
    ship_data_for_yolo(false);
    break;
  case analize_swim_detect:
    analize_swimmer_detection_netowrk();
    break;
  case make_subvid:
    make_sub_vid_using_tracking();
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
  cout << "To create data for YOLO and EXCLUDE JPEG images, press (4)" << endl;
  cout << "To create data for YOLO WITH JPEG images, press (5)" << endl;
  cout << "To analize swimmer detection network with current video, press (6)" << endl;
  cout << "To create sub-video, press (7)" << endl;
  cout << "\nlab>> ";
  cin >> answer;
  if (cin.fail()) cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
  case 4: current_request = ship_data;
    break;
  case 5: current_request = ship_data_with_pics;
    break;
  case 6: current_request = analize_swim_detect;
    break;
  case 7: current_request = make_subvid;
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

  stroke_annotate test;
  string mod_file_name = file_name;
  char resp = '0';
  char resp2 = '0';
  size_t pos = 0;
  int num = 0;
  string lead = "";
  char new_num[3] = "00";
  //test.graph_example();
  //test.file_example();

  while (1) {
    cout << "Would you like to do all lanes at once? (y/n)" << endl;
    cin >> resp;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (resp == 'y') {
      while (resp == 'y') {
        if (!test.start_stroke_counting(mod_file_name)) {
          while (1) {
            cout << "Is this the last one? (y/n)" << endl;
            cin >> resp2;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (resp2 == 'y') {
              resp = '0';
              break;
            }
            else if (resp2 == 'n') {
              break;
            }
          }
        }
        //modify mod_file_name by adding one to the number after the first "_"
        cout << "Old name... " << mod_file_name << endl;
        pos = mod_file_name.find_first_of('_', 0);
        num = stoi(mod_file_name.substr(pos + 1, 2));
        lead = mod_file_name.substr(0, pos+1);
        sprintf(new_num, "%2.2i", (num+1));
        mod_file_name = lead + string(new_num) + mod_file_name.substr(pos + 3, (mod_file_name.size() - 1));
        cout << "New name... " << mod_file_name << endl;
      }
      break;
    }
    else if (resp == 'n') {
      test.start_stroke_counting(file_name);
      break;
    }
  }

  return true;
}


//create data for yolo to use
//open application with the smallest named file
//specife file structures must occure for this to work
bool annotate_engine::ship_data_for_yolo(bool func_update_JPEG)
{
  box_annotate box_work;
  bool out_of_files = false;
  int file_num = 0;
  int picture_num = 0;
  bool  update_text = true;
  bool update_JPEG = func_update_JPEG;
  int class_stats[6];
  int total_class_stats = 0;
  int ii = 0;
  int* sub_class_stats;

  ofstream output_class_stats("class_stats.txt");

  for (ii = 0; ii < 6; ii++)  class_stats[ii] = 0;

  while (!out_of_files) {
    cout << "collecting data "<< file_num << ", from file " << (to_string(file_num) + ".avi") <<endl;

    if (box_work.load_video_for_boxing( to_string(file_num) + ".avi" )) {
      if (!box_work.create_training_set(&picture_num, update_text, update_JPEG)) {
        cout << "Failing to create training sets" << endl;
        return false;
      }
      //get class stats
      sub_class_stats = box_work.get_class_stats();
      for (ii = 0; ii < 6; ii++) class_stats[ii] += sub_class_stats[ii];
      box_work.~box_annotate();
    }
    else if (box_work.load_video_for_boxing(to_string(file_num) + ".mp4")) {
      if (!box_work.create_training_set(&picture_num, update_text, update_JPEG)) {
        cout << "Failing to create training sets" << endl;
        return false;
      }
      //get class stats
      sub_class_stats = box_work.get_class_stats();
      for (ii = 0; ii < 6; ii++) class_stats[ii] += sub_class_stats[ii];
      box_work.~box_annotate();
    }
    else {
      out_of_files = true;
    }
    file_num++;
  }

  //write class stats to a file
  if (output_class_stats.is_open()) {
    output_class_stats << "The amout of data for each class is the following:" << endl;
    for (ii = 0; ii < 6; ii++) {
      output_class_stats << CLASSES[ii] << " count: " << class_stats[ii] << endl;
      total_class_stats += class_stats[ii];
    }
    output_class_stats << "Total annotations collected: " << total_class_stats << endl;
    output_class_stats.close();
  }

  return true;
}


//produce video of what netowrk sees
//produce a summary of results
//if interactive mode iou is not used
//iou is the inter value of the iou desired muliptied by 100 
bool annotate_engine::analize_swimmer_detection_netowrk_non_inter(int iou)
{

  test_swim_detect_network tester(iou);

  tester.get_network_results(file_name);
  tester.save_network_results(file_name);

  return false;
}


//produce video of what netowrk sees
//produce a summary of results 
bool annotate_engine::analize_swimmer_detection_netowrk()
{

  test_swim_detect_network tester;

  tester.get_network_results(file_name);
  tester.save_network_results(file_name);

  return false;
}


//shows the boxes produced by annotated data and saves a file in //data//
void annotate_engine::interpolate_annotated_boxes(bool show) {
  
  swimmer_tracking testing;

  testing.annotation_tracking(file_name);
  if(show)
    testing.show_video_of_tracking(file_name);
  string str = file_name;
  str.replace(str.end() - 4, str.end(), "_gt.txt");
  testing.save_results_in_text_file(str);

}


//given video and proper data cfg and weight files produces detections of that video, save in data//
void annotate_engine::create_detection_files(bool show) {
  
  swimmer_tracking testing;

  testing.make_detection_file(file_name);
  if(show)
    testing.show_video_of_tracking(file_name);
  string str = file_name;
  str.replace(str.end() - 4, str.end(), "_det.txt");
  testing.save_results_in_text_file(str);

}

//Using annotations of swimmers make subvideo of each swimmer
//this asks the user if you would like to update the detection file
void annotate_engine::make_sub_vid_using_tracking() {
 
  sub_video make_subvid;
  //swimmer_tracking testing;
  string str = file_name;
  char resp = '0', temp = '0';

  do
  {
    cout << "Would you like to update the detection file? (y/n)" << endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin >> temp;
    if ((temp == 'n') || (temp == 'y')) {
      resp = temp;
    }
  } while (resp == '0');

  //make_subvid.make_subvideo(file_name);

  if (resp == 'y') make_subvid.make_detection_file(file_name);

  make_subvid.make_subvideo_using_sort_tracker(file_name);
 
  return;
}


//needs files localy named "data" and "output"
//no user input
void annotate_engine::make_sub_vid_using_tracking_auto_detect(bool update_detection_file)
{
  sub_video make_subvid;
  if (update_detection_file) {
    make_subvid.make_detection_file(file_name);
  }
  make_subvid.make_subvideo_using_sort_tracker(file_name);

  return;
}


