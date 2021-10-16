#ifndef BUTTONGUI_H
#define BUTTONGUI_H

#include <opencv2\opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//Adapted from https://stackoverflow.com/questions/33937800/how-to-make-a-simple-window-with-one-button-using-opencv-highgui-only/33938726
//Adapted from https://stackoverflow.com/questions/25748404/how-to-use-cvsetmousecallback-in-class

class buttonGUI
{

public:
	buttonGUI(); //TODO ssaGUI
	~buttonGUI();

	void runButton();

private:
	//TODO integrate ray's code
	cv::Mat3b canvas;
	cv::Rect buttonStart;
	cv::Rect buttonStop;

	std::string buttonTextStart = "Start";
	std::string buttonTextStop = "Stop";
	std::string winName = "Smart Swim App";

	static void callBackFunc(int event, int x, int y, int, void* userdata);
	void secondCall(int event, int x, int y);
};

#endif // !BUTTONGUI_H
