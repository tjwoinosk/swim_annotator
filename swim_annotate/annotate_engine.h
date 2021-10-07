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

private:

  //Change file name
  void change_video_file_name(string new_file_name) {
    file_name = new_file_name;
  }

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

  //Using tracking of swimmers make subvideo of each swimmer
  void make_sub_vid_using_tracking();

  //needs files localy named "data" and "output"
  //data holds the pre SORT detection files
  //output holds the post SORT detection file and the subvideo
  void make_sub_vid_using_tracking_auto_detect(bool update_detection_file);

  //shows the boxes produced by annotated data and saves a file in data//
  void interpolate_annotated_boxes(bool show);

  //given video and proper data cfg and weight files produces detections of that video, save in data//
  void create_detection_files(bool show);

  //given video, proper data cfg, weight file and detection file, produce of that video, save in data//
  void create_tracking_video(bool show = false);

  //Detection file, and groud truth tracking file required 
  void test_pipeline_tracker(string detFile, string gtTrackingFile);

  //given video, proper data cfg, weight file and detection file, produce of that video, save in data//
  void createGroundTruthDetection();

};

