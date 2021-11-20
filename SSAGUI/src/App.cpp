// rtSwimTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include "buttonGUI.h"
#include "SSAGUI.h"
#include "buttonGUI.h"
#include "postProcessRealTimeTracking.h"
#include "subVideoCreator.h"

int main()
{
	
	SSAGUI ssaVideo("14.mp4");
	frameAnalysis frameAnalysisObj;

	ssaVideo.setFrameAnalysis(frameAnalysisObj);
	ssaVideo.setToleranceX(0.25);
	ssaVideo.setToleranceY(0.25);
	ssaVideo.playVideo();
	//ssaVideo.playVideoTest(false, 130, 134, 140, cv::Point_<float>(300, 275)); //Lane two - 0.25, 0.25 means within; 0.15 = Y means outside Y
	//ssaVideo.playVideoTest(false, 130, 134, 140, cv::Point_<float>(655, 10)); //Lane two - 0.25, 0.25 means within; 0.15 = Y means outside Y


	//ssaVideo.playVideoTest(false, 130, 134, 140, cv::Point_<float>(300, 275));

	//ssaVideo.playVideoTest(false, 185, 190, 200, cv::Point_<float>(659, 83));
	//    ssaVideoTwo.playVideoTest(true, 14, 20, 26, cv::Point_<float>(300, 275)); //This should make no changes to the output file


	//subVidObj.setFrameAnalysis(frameAnalysisObj);
	//subVidObj.make_video();
}