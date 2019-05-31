#include "video_editor.h"

#include <iostream>

//prints the options for video edditing 
void video_editor::print_video_options() {

/*
1.	Go to selected frame 
2.	play video 
3.	pause video
4.	Go to next frame
5.	Go to last frame
6.	Exit video view options
7.	Show annotations, remove annotations (toggle)
8.	Trim video
9.	Make sub video
*/
  cout << "\nVideo viewing options\n" << endl;

  cout << "Go to selected frame, press \" \"" << endl;
  cout << "play video, press \" \"" << endl;
  cout << "pause video, press \" \"" << endl;
  cout << "Go to next frame, press \" \"" << endl;
  cout << "Go to last frame, press \" \"" << endl;
  cout << "Trim video options, press \" \"" << endl;
  cout << "Make sub video, press \" \"" << endl;
  cout << "Exit video view, press \" \"" << endl;

}

//wait for incomming key strokes to controll video viewer
bool video_editor::video_viewer_controller() {
  //use istream::get to get charaters from the keybord without pressing enter

}