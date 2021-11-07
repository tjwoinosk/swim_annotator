// rtSwimTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include "buttonGUI.h"
#include "SSAGUI.h"
#include "buttonGUI.h"
#include "postProcessRealTimeTracking.h"

int main()
{

	SSAGUI ssaVideo("14.mp4");

	//ssaVideo.runButton();
//	buttonGUI test;
	//test.runButton();
	ssaVideo.playVideo(10);	
	postProcessRealTimeTracking testprocess;
	std::cout << std::endl << std::endl << "DONE DONE DONE DONE" << std::endl << std::endl;
	ssaVideo.make_video("14.mp4", "14_TEST.mp4");
	std::cout << std::endl << std::endl << "DONE again DONE again DONE again DONE again" << std::endl << std::endl;

}