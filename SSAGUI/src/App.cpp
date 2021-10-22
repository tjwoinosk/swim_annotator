// rtSwimTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include "buttonGUI.h"
#include "SSAGUI.h"

int main()
{

	SSAGUI ssaVideo("0.avi");

	//ssaVideo.runButton();
//	buttonGUI test;
	//test.runButton();
	ssaVideo.playVideo(10);

}