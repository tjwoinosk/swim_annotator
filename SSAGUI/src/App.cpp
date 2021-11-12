// rtSwimTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include "buttonGUI.h"
#include "SSAGUI.h"
#include "buttonGUI.h"
#include "postProcessRealTimeTracking.h"

int main()
{
	/*
	SSAGUI ssaVideo("14.mp4");
	ssaVideo.playVideo();
	*/
	/*
	SSAGUI ssaVideo("14.mp4");
	ssaVideo.playVideo(10);	
	postProcessRealTimeTracking testprocess;
	std::cout << std::endl << std::endl << "DONE DONE DONE DONE" << std::endl << std::endl;
	ssaVideo.make_video("14.mp4", "14_TEST.mp4");
	std::cout << std::endl << std::endl << "DONE again DONE again DONE again DONE again" << std::endl << std::endl;
	*/

	
	SSAGUI ssaVideo("14.mp4");
	ssaVideo.playVideoTest(14, 20, 26, cv::Point_<float>(300, 275));
	

	//x = 299, y = 325 - second lane
}