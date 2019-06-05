//Tim Woinoski
//2019-05-30
// swim_annotate.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This application labels swimming videos quicker

#include <iostream>
#include "mode_mannager.h"

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
  else if(argc > 2) {
    cout << "Too many input files!\ngood bye." << endl;
  }
  else {
    string videofile;
    VideoCapture video_lab(argv[1]);

    if (!video_lab.isOpened()) {
      cout << "could not read input file!\ngood bye." << endl;
    }
    else {

      cout << "\n\n\n";
      cout << "***************************************" << endl;
      cout << "*  Welcome to the swiming annotator!  *" << endl;
      cout << "***************************************" << "\n\n\n";


      while (1) {

        Mat frame;
        // Capture frame-by-frame
        video_lab >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
          break;

        // Display the resulting frame
        imshow("Frame", frame);

        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27)
          break;
      }

      // When everything done, release the video capture object
      video_lab.release();

      // Closes all the frames
      destroyAllWindows();
      /*
      mode_mannager app(startup_mode, videofile);


      while (!app.is_app_finished()) {
        app.service_next_request();
      }
      */
    }
 
  }

  return 0;
}






