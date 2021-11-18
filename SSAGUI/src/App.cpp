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
	subVideoCreator subVidObj("14.mp4");

	ssaVideo.setFrameAnalysis(frameAnalysisObj);
	ssaVideo.playVideo();

	//subVidObj.setFrameAnalysis(frameAnalysisObj);
	//subVidObj.make_video();
}