#pragma once
#include "swimmer_tracking.h"
class sub_video :
  public swimmer_tracking
{

private:
  //holds pointers to desired values in results
  vector<track_data> sorted_data;//his can hold up to 30s of frames @30 fps
  bool sorted_data_is_valid;

public:
  sub_video();

  //Returns an in order array of pointers that point to the lane number requested 
  //fills up a vector of one object to be turned into subvideo
  //Checks the vector to see if there are no duplicates or frames in a wrong order
  void extract_data(int lane_number);

  //Create subvideo 
  void make_subvideo(string file_name);

  //Finds the right aspect ratio for the sub-video based on the tracking resutls
  void find_best_aspect(int& hight, int& width);

  //Saves the video as "video_name.ext" in the workind directory 
  void make_video(string video_name);

  //scales the video to a standard size so they are all the same
  void scale_video();

  //shows a video of all track boxes made
  void show_all_tracks(string video_name);

};

