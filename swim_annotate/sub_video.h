#pragma once
#include "swimmer_tracking.h"
class sub_video :
  public swimmer_tracking
{

private:

public:
  sub_video();

  //Create subvideo 
  void make_subvideo();

  //Finds the right aspect ratio for the sub-video based on the tracking resutls
  void find_best_aspect(int *hight, int *width);

  //scales the video to a standard size so they are all the same
  void scale_video();

};

