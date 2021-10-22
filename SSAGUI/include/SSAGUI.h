#ifndef SSAGUI_H
#define SSAGUI_H

#include <opencv2\opencv.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//Adapted from https://stackoverflow.com/questions/33937800/how-to-make-a-simple-window-with-one-button-using-opencv-highgui-only/33938726
//Adapted from https://stackoverflow.com/questions/25748404/how-to-use-cvsetmousecallback-in-class

static const string buttonTextStart = "Start";
static const string buttonTextStop = "Stop";
static const string appName = "Smart Swim App";
static const Vec3b buttonColor = Vec3b(200, 200, 200);

class SSAGUI
{
public:
	SSAGUI(string videoFile); //TODO ssaGUI
	~SSAGUI();

	Mat frame;
	Rect startButton, stopButton;
	bool isPlaying = true;
	
	void playVideo(int videoDelay);


private:
	// private variables for video streaming
	string video;
	VideoCapture videoStream;

	// private variables for GUI
//	Mat frame;
	//Rect startButton, stopButton;

	VideoCapture getVideoStream();
	int isVideoStreamValid();
	void closeVideo();

//	void setupVideoGUI();
	static void callBackFunc(int event, int x, int y, int, void* userdata);
	void secondCall( int event, int x, int y);
};

#endif
