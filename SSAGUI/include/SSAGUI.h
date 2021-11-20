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
static const string buttonTextCancel = "Cancel";
static const string appName = "Smart Swim App";
static const Vec3b buttonColor = Vec3b(200, 200, 200);

class SSAGUI
{
public:
	SSAGUI(string videoFile); 
	~SSAGUI();

	void playVideo();

	void playVideoTest(bool withCancellation, int frameNum_selectSwimmer, int frameNum_startTracking, int frameNum_stopTracking, cv::Point_<float> mouseClick_Test); //TODO this is to test playVideo

	void setFrameAnalysis(frameAnalysis& frameObj);

	float getToleranceX();
	float getToleranceY();
	void setToleranceX(float val);
	void setToleranceY(float val);

private:
	// private variables for video streaming
	bool isPlaying = true;
	string video;
	VideoCapture videoStream;
	
	// variables for drawing the frames and buttons
	cv::Mat3b canvas;
	Mat frame;
	Mat frameResized;
	Rect startButton, stopButton, cancelButton;

	frameAnalysis* frameAnalysisObj_ptr;

	float toleranceX;
	float toleranceY;
	
	const int VIDEO_DELAY = 10;
	const float VIDSIZE_WIDTH = 680;
	const float VIDSIZE_HEIGHT = 400;
	const int BUTTON_HEIGHT = 50;
	

	VideoCapture& getVideoStream();
	int isVideoStreamValid();
	void closeVideo();

	static void callBackFunc(int event, int x, int y, int, void* userdata);
	void secondCall(int event, int x, int y);

	void drawOnFrame();
};

#endif