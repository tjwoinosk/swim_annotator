#ifndef SSAGUI_H
#define SSAGUI_H

#include <opencv2\opencv.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <iostream>
#include "frameAnalysis.h"
#include "postProcessRealTimeTracking.h"

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

	

	void playVideo(int videoDelay);


private:
	// private variables for video streaming
	Mat frame; 
	Rect startButton, stopButton;
	const int buttonHeight = 50;
	bool isPlaying = true;
	string video;
	VideoCapture videoStream;
	const float vidSize_width = 680;
	const float vidSize_height = 400;
	cv::Mat3b canvas;
	Mat frameResized;
	frameAnalysis frameAnalysisObj;
	std::vector<TrackingBox> resultsTrackingSingleSwimmer;
	
	VideoCapture getVideoStream();
	int isVideoStreamValid();
	void closeVideo();

	static void callBackFunc(int event, int x, int y, int, void* userdata);
	void secondCall(int event, int x, int y);
};

#endif
