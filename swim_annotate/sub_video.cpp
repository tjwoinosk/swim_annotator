#include "sub_video.h"
#include <math.h>

using namespace std;
using namespace cv;


sub_video::sub_video()
{
  sorted_data_is_valid = false;
}


//For each object to make subvideo with...
//Call a fuction that tracks swimers and fills results in swimmer_tracking.h
//make_res_values(lane_num) must be called first
//Then make_video(file_name) must be called
void sub_video::make_subvideo_using_sort_tracker(string file_name)
{
  int jj = 0;
  int ii = 0;
  int hight = 0;
  int width = 0;
  const int valid_size = 30*5;//video is usualy 30 frames per second 
  vector<int> valid_swimmers;//list of object ids that are valid
  vector<int> final_swimmers;
  int current_swimmer = 1;
  int holder = -1;
  int cntr = 0;
  string change_name;

  //Sort tracking needs uses the detection file
  sort_tracking(file_name);

  vector<track_data> look_threw = get_results(); //Unchanged...

  //list is in assending order by frame
  //look threw list to find all object ids they are in assending order
  // but they dont allways count up by one
  if (!look_threw.empty()) valid_swimmers.push_back(look_threw.front().object_ID);
  for (ii = 0; ii < look_threw.size(); ii++) {
    if (look_threw[ii].object_ID > valid_swimmers.back()) {
      valid_swimmers.push_back(look_threw[ii].object_ID);
    } 
  }

  //Make a list of all valid swimmers ie. tracks that contain more the a second of tracking
  //If there is a break in an objects tracking cntr will reset
  for (jj = 0; jj < valid_swimmers.size(); jj++) {
    holder = -1;
    cntr = 0;
    for (ii = 0; ii < look_threw.size(); ii++) {
      if ((holder == -1) && (look_threw[ii].object_ID == valid_swimmers[jj])) {
        cntr = 0;
        holder = look_threw[ii].frame_num;
        cntr++;
      }
      else if ((look_threw[ii].object_ID == valid_swimmers[jj]) && ((holder + cntr) == look_threw[ii].frame_num)) {//If swimmer is in next frame
        cntr++;
        if (cntr > valid_size) {
          final_swimmers.push_back(valid_swimmers[jj]);
          break;
        }
      }
      else if(((holder + cntr + 1) == look_threw[ii].frame_num) && (holder > -1)) {
        holder = -1;
      }
    }
  }
  
  cout << "Object IDs... " << endl;
  for (ii = 0; ii < final_swimmers.size(); ii++) {
    cout << final_swimmers[ii] << endl;
  }
  
  change_name = ".//output//" + file_name;
  change_name.replace(change_name.end() - 4, change_name.end(), "_0.avi");
  for (ii = 0; ii < final_swimmers.size(); ii++) {
    change_name.replace(change_name.end() - 5, change_name.end(), to_string(ii) + ".avi");
    extract_data(final_swimmers[ii]);
    make_video(file_name, change_name);
  }  
  //

  //uncoment to make the video as well
  //make_video(file_name, final_swimmers);
  
  //save tracking results in text file
  //change_results(look_threw);
  //change_name = file_name;
  //change_name.replace(change_name.end() - 4, change_name.end(), "_tr.txt");
  //save_results_in_text_file(change_name);

  return;
}


//Returns an in order array of pointers that point to the lane number requested 
void sub_video::extract_data(int lane_number)
{
  int ii = 0;
  int jj = 0;
  vector<track_data> raw_data;
  bool data_check = true;
  sorted_data.clear();

  raw_data = get_results();

  if (raw_data.empty()) {
    cout << "Raw data does not exist!" << endl;
    return;
  }

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
      var_width += pow(double(holder_width), double(2));
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


//Finds the right aspect ratio for the sub-video based on the tracking resutls
// Still to be completed need to find best way to select values
void sub_video::find_best_aspect(int& hight, int& width, int& m_hight, int& m_width, int& v_hight, int& v_width)
{
  hight = 0; width = 0; m_hight = 0; v_hight = 0; m_width = 0; m_hight = 0;
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
      var_width += pow(double(holder_width), double(2));
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
  m_width = avg_width;
  m_hight = avg_hight;
  v_width = var_width;
  v_hight = var_hight;

  //Set the aspect raitio to somthing that is not quite the max hight and width
  // yet to be attempted 

}


//Saves the video as "video_name.ext" in the workind directory
//Make video while scaleing the video to a standard size
void sub_video::make_video(string video_name, vector<int> object_ids)
{

  string str, outputFile;
  char buff[20];
  VideoCapture cap;
  VideoWriter video;
  Mat frame, crop_frame;
  const int final_aspect = 150;
  int hight = 0, width = 0;
  int x = 0, y = 0, box_w = 0, box_h = 0;
  int ii = 0, jj = 0;
  int num_objects = object_ids.size();
  fstream aspect_ratio_file;

  vector<track_data> raw_data = get_results();
  vector<track_data> frame_data;
  vector<VideoWriter> videos(num_objects);
  vector<int> hights(num_objects, 0);
  vector<int> widths(num_objects, 0);
  vector<int> m_wids(num_objects, 0);
  vector<int> m_highs(num_objects, 0);
  vector<int> v_wids(num_objects, 0);
  vector<int> v_highs(num_objects, 0);
  vector<string> file_names(num_objects);

  if (raw_data.empty()) {
    cout << "Raw data does not exist!" << endl;
    return;
  }

  
  // Open the video file
  cout << "Opening... " << video_name << endl;
  cap.open(video_name);
  if (!cap.isOpened()) {
    cout << "Issues opening " << video_name << endl;
  }


  
  // Get the video writer initialized to save the output video
  for (ii = 0; ii < num_objects; ii++) {
    sprintf(buff, "_%2.2d_sub_video.avi", ii);//"_sub_video.avi"
    str = ".//output//" + video_name;
    str.replace(str.end() - 4, str.end(), string(buff));
    cout << str << endl;
    file_names[ii] = str;
    extract_data(object_ids[ii]);
    //Decided best hight and width to take from the origanl video
    if (sorted_data_is_valid) {//check if there are occultions
      find_best_aspect(hights[ii], widths[ii], m_highs[ii],m_wids[ii],v_highs[ii],v_wids[ii]);
      videos[ii].open(str, VideoWriter::fourcc('M', 'J', 'P', 'G'), 28, Size(final_aspect, final_aspect));
    }
  }

  str = ".//output//" + video_name;
  str.replace(str.end() - 4, str.end(), "_AR_stats.txt");
  aspect_ratio_file.open(str,std::fstream::out);
  if (aspect_ratio_file.is_open()) {
    aspect_ratio_file << "Stats on sub-video aspect ratio sizes" << endl;
    for (ii = 0; ii < num_objects; ii++) {
      aspect_ratio_file << file_names[ii] << endl;
      aspect_ratio_file << "Max width:  " << widths[ii];
      aspect_ratio_file << "  Max height:  " << hights[ii] << endl;
      aspect_ratio_file << "Avg Width:  " << m_wids[ii];
      aspect_ratio_file << "  Avg Height:  " << m_highs[ii] << endl;
      aspect_ratio_file << "Var Width:  " << v_wids[ii];
      aspect_ratio_file << "  Var Hieght:  " << v_highs[ii] << endl << endl << endl;
    }
    aspect_ratio_file.close();
  }
  else {
    cout << "Was unable to open file on aspect stats" << endl;
  }


  //Get biggest sizes for making border
  for (ii = 0; ii < num_objects; ii++) {
    if (hights[ii] > hight) hight = hights[ii];
    if (widths[ii] > width) width = widths[ii];
  }

  //get starting values
  cap >> frame;
  Mat frame_buf(frame.rows + hight * 2, frame.cols + width * 2, frame.depth());
    
  cap.set(CAP_PROP_POS_FRAMES, 0);
  while(waitKey() != 27) {

    cap >> frame;
    // Stop the program if reached end of video
    if (frame.empty()) {
      cout << "Done processing !!!" << endl;
      break;
    }

    // form a border in-place
    copyMakeBorder(frame, frame_buf, hight, hight, width, width, BORDER_CONSTANT, 0);

    //Get data for this frame
    frame_data.clear();
    int current_frame = cap.get(CAP_PROP_POS_FRAMES) - 1;
    for (ii = 0; ii < raw_data.size(); ii++) {
      if (raw_data[ii].frame_num == current_frame) {
        frame_data.push_back(raw_data[ii]);
      }
    }

    Mat detectedFrame;
    for (ii = 0; ii < num_objects; ii++) {
      for (jj = 0; jj < frame_data.size(); jj++) {
        if (object_ids[ii] == frame_data[jj].object_ID) {
          x = frame_data[jj].frame_pos.x;
          box_w = frame_data[jj].frame_pos.width;
          y = frame_data[jj].frame_pos.y;
          box_h = frame_data[jj].frame_pos.height;
          crop_frame = frame_buf(Rect(x + box_w / 2 + width - widths[ii] / 2,
            y + box_h / 2 + hight - hights[ii] / 2, widths[ii], hights[ii]));
          //scale video to a standard size
          resize(crop_frame, crop_frame, Size(final_aspect, final_aspect), 0, 0, INTER_LINEAR);
          crop_frame.convertTo(detectedFrame, CV_8U);
          if(videos[ii].isOpened()) videos[ii].write(detectedFrame);
          break;
        }
      }
    }

    //Find lane ropes and use affine transform make lane ropes parallel to horizontals
    //Might not want to cut off after lane ropes becuase that will cut off current swimmer
    //see https://en.wikipedia.org/wiki/Digital_image_processing for more on affine
    //see ovencv warpAffine for fuction... find theta of lane ropes with x-axis and use rotaion 
    //solve for 2x3 matrix using linear algibra

  }

  cap.release();
  for(ii = 0; ii<num_objects;ii++) videos[ii].release();
  cout << "finished!" << endl;

  return;

}


//Saves the video as "video_name.ext" in the workind directory
//Make video while scaleing the video to a standard size
void sub_video::make_video(string video_name, string sub_video_name)
{
  string str, outputFile;
  VideoCapture cap;
  VideoWriter video;
  Mat frame, crop_frame;
  const int final_aspect = 150;
  int hight = 0, width = 0;
  int v_hight = 0, v_width = 0, m_hight = 0, m_width = 0;
  int x = 0, y = 0, box_w = 0, box_h = 0;
  int ii = 0;

  if (sorted_data_is_valid) {
    // Open the video file
    cout << "Opening... " << video_name << endl;
    cap.open(video_name);
    if (!cap.isOpened()) {
      cout << "Issues opening " << video_name << endl;
    }

    str = sub_video_name;
    str.replace(str.end() - 4, str.end(), "_sub_video.avi");
    outputFile = str;

    //Decided best hight and width to take from the origanl video
    find_best_aspect(hight, width);

    // Get the video writer initialized to save the output video
    video.open(outputFile, VideoWriter::fourcc('M', 'J', 'P', 'G'), 28, Size(final_aspect, final_aspect));

    //get starting values
    cap >> frame;
    Mat frame_buf(frame.rows + hight * 2, frame.cols + width * 2, frame.depth());
    
    //namedWindow("tester", WINDOW_NORMAL);

    for (ii = 0; ii < sorted_data.size(); ii++) {

      // get frame from the video
      if (sorted_data[ii].frame_num != cap.get(CAP_PROP_POS_FRAMES)) {
        cap.set(CAP_PROP_POS_FRAMES, sorted_data[ii].frame_num);//might make this very slow
      }
      cap >> frame;

      //needed to work cap object!!!!
      char c = (char)waitKey();
      if (c == 27)
        break;

      // Stop the program if reached end of video
      if (frame.empty()) {
        cout << "Done processing !!!" << endl;
        cout << "Output file is stored as " << outputFile << endl;
        break;
      }

      //Draw a rectangle displaying the bounding box
      /*
      int blue = 0, red = 0, green = 0;
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
      //*/

      // form a border in-place
      copyMakeBorder(frame, frame_buf, hight, hight, width, width, BORDER_CONSTANT, 0);
      x = sorted_data[ii].frame_pos.x;
      box_w = sorted_data[ii].frame_pos.width;
      y = sorted_data[ii].frame_pos.y;
      box_h = sorted_data[ii].frame_pos.height;
      crop_frame = frame_buf(Rect(x + box_w / 2 + width / 2, y + box_h / 2 + hight / 2, width, hight));

      //scale video to a standard size
      resize(crop_frame, crop_frame, Size(final_aspect, final_aspect), 0, 0, INTER_LINEAR);

      // Write the frame with the detection boxes
      Mat detectedFrame;
      crop_frame.convertTo(detectedFrame, CV_8U);

      video.write(detectedFrame);

      //imshow("tester", crop_frame);

    }

    //destroyWindow("tester");
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



