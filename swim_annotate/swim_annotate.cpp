//Tim Woinoski
//2019-05-30
// swim_annotate.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This application labels swimming videos quicker

#include <iostream>
#include <string.h>
#include "annotate_engine.h"

#include <opencv2/videoio/videoio.hpp> //displaying video
#include <opencv2/opencv.hpp> //displaying video

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
  cout << "reading input file..." << endl;
  if (argc == 1) {
    cout << "Need a file to work on!\ngood bye." << endl;
  }
  else if(argc > 4) {
    cout << "Too many input files!\ngood bye." << endl;
  }
  else {
    string videofile;
    VideoCapture video_lab(argv[1]); //check if file can be opened befor starting

    if (!video_lab.isOpened()) {
      cout << "could not read input file!\ngood bye." << endl;
    }
    else {

      video_lab.release();

      cout << "\n\n\n";
      cout << "***************************************" << endl;
      cout << "*  Welcome to the swiming annotator!  *" << endl;
      cout << "***************************************" << "\n\n\n";

      annotate_engine app(argv[1]);

      if (argc == 2) {
        //get user input
        while (!app.is_app_finished()) {
          while (!app.print_general_lab_options());
          app.service_next_request();
        }
      }
      else {
        //Alows program to be run many times automaticly given correct files... see \test_network\automate
        //Ex.
        //swim_annotate.exe video.mp4 -a iou_val
        if (strcmp(argv[2], "-a") == 0) {
          if (argc == 4) {
            if ((stoi(argv[3]) < 100) && (stoi(argv[3]) > 0)) {
              cout << "testing network with iou of " << float(stoi(argv[3])) / 100 << endl;
              app.analize_swimmer_detection_netowrk_non_inter(stoi(argv[3]));
            }
            else {
              cout << "iou value must be exculsivly between 0 and 100" << endl;
            }
          }
          else {
            //Defalt iou value
            app.analize_swimmer_detection_netowrk_non_inter(25);
          }
        }
        else {
          cout << "Incorrrect flag used" << endl;
        }
      }
      
      app.kill_app();
      
    }
 
  }

  return 0;
}

