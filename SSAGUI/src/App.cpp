// rtSwimTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include "buttonGUI.h"
#include "SSAGUI.h"
#include "buttonGUI.h"
#include "sort_tracker.h"
#include "sortTrackerPipelined.h"

int main()
{

	SSAGUI ssaVideo("01.mp4");

	//ssaVideo.runButton();
//	buttonGUI test;
	//test.runButton();
	ssaVideo.playVideo(10);
	sortTrackerPiplelined testPipeline;

}