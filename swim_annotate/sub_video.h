#pragma once
#include "swimmer_tracking.h"

/*
References: [3]
*/


class sub_video :
  public swimmer_tracking
{

private:
  //holds pointers to desired values in results
  vector<track_data> sorted_data;
  bool sorted_data_is_valid;

public:
  sub_video();

  //Returns an in order array of pointers that point to the lane number requested 
  //fills up a vector of one object to be turned into subvideo
  //Checks the vector to see if there are no duplicates or frames in a wrong order
  void extract_data(int lane_number);

  //Create subvideo 
  void make_subvideo_using_sort_tracker(string file_name);

  //Finds the right aspect ratio for the sub-video based on the tracking resutls
  void find_best_aspect(int& hight, int& width);

  //Finds the right aspect ratio for the sub-video based on the tracking resutls
  void find_best_aspect(int& hight, int& width, int& m_hight, int& m_width, int& v_hight, int& v_width);

  //Saves the video as "video_name.ext" in the workind directory 
  void make_video(string video_name, string sub_video_name);

  //Saves the video as "video_name.ext" in the workind directory 
  void make_video(string video_name, vector<int> object_ids);

  //scales the video to a standard size so they are all the same
  void scale_video();


};

