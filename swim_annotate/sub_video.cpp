#include "sub_video.h"
#include <math.h>

using namespace std;

sub_video::sub_video()
{
  sorted_data_is_valid = false;
}


//For each object to make subvideo with...
//Call a fuction that tracks swimers and fills results in swimmer_tracking.h
//make_res_values(lane_num) must be called first
//Then make_video(file_name) must be called
void sub_video::make_subvideo(string file_name)
{
  int jj = 0;
  int ii = 0;
  int hight = 0;
  int width = 0;
  
 
  //Find all tracked swimmers from one lane (object ID) - make sure they are in order
  /*
  for (ii = 0; ii < 24; ii++) {
    extract_data(ii);
    
    make_video(file_name);
  }
  //*/

  //Make video while scaleing the video to a standard size
    
  //use opencv resize()

  //*
  if (annotation_tracking(file_name)) {
    for (ii = 0; ii < 10; ii++) {
      extract_data(ii);
      make_video(file_name);
    }
  } 
  else { 
    cout << "Could not make any subvideo :(" << endl;
  }
  //*/

  return;
}


//Returns an in order array of pointers that point to the lane number requested 
void sub_video::extract_data(int lane_number)
{
  int ii = 0;
  int jj = 0;
  vector<track_data> raw_data;
  vector<track_data>* checker = nullptr;
  bool data_check = true;
  sorted_data.clear();

  checker = get_results();

  if (checker == nullptr) {
    cout << "Raw data does not exist!" << endl;
    return;
  }
  raw_data = *checker;

  //Get all the indecies of the objects holding the lane number
  for (ii = 0; ii < raw_data.size(); ii++) {
    if (raw_data[ii].object_ID == lane_number) {
      sorted_data.push_back(raw_data[ii]);
      if (raw_data[ii].frame_pos.x < 0) {
        data_check = false;
      } 
    }
  }

  sorted_data_is_valid = data_check;
  //Sort to make sure they are in correct order... 
  //maby already in correct order... so no need

  return;
}


//Finds the right aspect ratio for the sub-video based on the tracking resutls
// Still to be completed need to find best way to select values
void sub_video::find_best_aspect(int& hight, int& width)
{
  hight = 0;
  width = 0;
  int max_hight = 0, min_hight = get_hight();
  double avg_hight = 0, var_hight = 0;
  int max_width = 0, min_width = get_width();
  double avg_width = 0, var_width = 0;
  int holder_hight = 0, holder_width = 0;
  double n = double(sorted_data.size());
  int ii = 0;

  for (ii = 0; ii < int(n); ii++) {

    holder_hight = sorted_data[ii].frame_pos.height;
    holder_width = sorted_data[ii].frame_pos.width;
    if ((holder_hight > 0) && (holder_width > 0)) {
      //get max and min vals of hight and width 
      if (holder_hight > max_hight) max_hight = holder_hight;
      if (holder_width > max_width) max_width = holder_width;
      if (holder_width < min_width) min_width = holder_width;
      if (holder_hight < min_hight) min_hight = holder_hight;

      //get avg hight and width
      avg_hight += holder_hight;
      var_hight += pow(double(holder_hight), double(2));
      avg_width += holder_width;
      var_width += pow(double(holder_hight), double(2));
    }
  }

  //calc avg and variance
  avg_hight /= n;
  avg_width /= n;
  //sample variance
  if (n != 1) {//NO zero div
    var_hight = (var_hight - pow(avg_hight, double(2)) * n) / (n - 1);
    var_width = (var_width - pow(avg_width, double(2)) * n) / (n - 1);
  }
  
  //Set the aspect raitio to the max hight and width
  width = max_width;
  hight = max_hight;

  //Set the aspect raitio to somthing that is not quite the max hight and width
  // yet to be attempted 

}


//Saves the video as "video_name.ext" in the workind directory
void sub_video::make_video(string video_name)
{

  string str, outputFile;
  VideoCapture cap;
  VideoWriter video;
  Mat frame, crop_frame;
  int hight = 0, width = 0;
  int x = 0, y = 0, box_w = 0, box_h = 0;
  int ii = 0;
  int blue, red, green;
 
  if (sorted_data_is_valid) {
    // Open the video file
    str = video_name;
    cout << "Opening.. " << str << endl;
    cap.open(str);
    if (!cap.isOpened()) {
      cout << "Issues opening " << str << endl;
    }
    str.replace(str.end() - 4, str.end(), "_sub_video.avi");
    outputFile = str;

    //Decided best hight and width to take from the origanl video
    find_best_aspect(hight, width);

    // Get the video writer initialized to save the output video
    video.open(outputFile, VideoWriter::fourcc('M', 'J', 'P', 'G'), 28, Size(width, hight));

    cap >> frame;
    Mat frame_buf(frame.rows + hight * 2, frame.cols + width * 2, frame.depth());

    namedWindow("tester", WINDOW_NORMAL);

    cap.set(CAP_PROP_POS_FRAMES, 0);

    for (ii = 0; ii < sorted_data.size(); ii++) {
      // get frame from the video
      cap >> frame;

      //needed to work cap object!!!!
      char c = (char)waitKey(25);
      if (c == 27)
        break;

      // Stop the program if reached end of video
      if (frame.empty()) {
        cout << "Done processing !!!" << endl;
        cout << "Output file is stored as " << outputFile << endl;
        waitKey(3000);
        break;
      }
      
      //Draw a rectangle displaying the bounding box
      switch (sorted_data[ii].class_id) {
      case 0://Brown
        blue = 0; red = 100; green = 50;
        break;
      case 1://red
        blue = 0; red = 255; green = 0;
        break;
      case 2://green
        blue = 0; red = 0; green = 255;
        break;
      case 3://orange
        blue = 0; red = 255; green = 127;
        break;
      case 4://yellow
        blue = 0; red = 255; green = 255;
        break;
      case 5://purple
        blue = 255; red = 128; green = 0;
        break;
      default:
        break;
      }
      rectangle(frame, sorted_data[ii].frame_pos, Scalar(blue, green, red), 1, 1);


      // form a border in-place
      copyMakeBorder(frame, frame_buf, hight, hight, width, width, BORDER_CONSTANT, 0);
      x = sorted_data[ii].frame_pos.x;
      box_w = sorted_data[ii].frame_pos.width;
      y = sorted_data[ii].frame_pos.y;
      box_h = sorted_data[ii].frame_pos.height;

      crop_frame = frame_buf(Rect(x+box_w/2+width/2, y+box_h/2+hight/2, width, hight));


      //scale video to a standard size
      imshow("tester", crop_frame);
      // Write the frame with the detection boxes
      Mat detectedFrame;
      crop_frame.convertTo(detectedFrame, CV_8U);

      video.write(detectedFrame);

    }

    destroyWindow("tester");

    cap.release();
    video.release();
    cout << "finished!" << endl;

  }
  else {
    cout << "No data on lane " << video_name << "Not making sub-video!" << endl;
  }

  return;

}


//scales the video to a standard size so they are all the same
void sub_video::scale_video()
{

}

//shows a video of all track boxes made
void sub_video::show_all_tracks(string video_name)
{

}


