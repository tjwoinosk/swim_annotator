#pragma once

#include <string>

using namespace std;


class video_editor
{
  string video_adress;

public:

  //prints the options for video edditing 
  void print_video_options();

  //wait for incomming key strokes to controll video viewer
  bool video_viewer_controller();







};

