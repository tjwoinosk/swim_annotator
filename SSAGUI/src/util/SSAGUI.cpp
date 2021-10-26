// VideoStream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "SSAGUI.h"

SSAGUI::SSAGUI(string videoFile) {
	video = videoFile;
	videoStream.open(videoFile);
}

SSAGUI::~SSAGUI() {

}

VideoCapture SSAGUI::getVideoStream() {
	return videoStream;
}


int SSAGUI::isVideoStreamValid() {
	if (!getVideoStream().isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return 0;
	}
	else {
		return 1;
	}
}

void SSAGUI::playVideo(int videoDelay = 10) {

	if (isVideoStreamValid()) {
		while (1) {
			// Capture frame-by-frame
			if (isPlaying)
				getVideoStream() >> frame;

			// If the frame is empty, break immediately
			if (frame.empty())
				break;

			//TODO pick which method of resizing is appropriate, and what size
			/*
			float resizeRatioWidth = vidSize/ static_cast<float>(frame.rows);
			float resieRatioHeight = vidSize / static_cast<float>(frame.cols);
			cv::resize(frame, frameResized, cv::Size(), resizeRatioWidth, resieRatioHeight);
			*/
			cv::resize(frame, frameResized, cv::Size(vidSize_width, vidSize_height)); //TODO this is to force to a fixed size

			startButton = Rect(0, 0, frameResized.cols / 2, 50);
			stopButton = Rect(frameResized.cols / 2, 0, frameResized.cols / 2, 50);

			canvas = Mat3b(frameResized.rows + startButton.height, frameResized.cols, Vec3b(0, 0, 0));

			canvas(startButton) = buttonColor; //Colour
			canvas(stopButton) = buttonColor; //Colour

			frameResized.copyTo(canvas(Rect(0, startButton.height, frameResized.cols, frameResized.rows)));

			putText(canvas, buttonTextStart, Point(startButton.width * 0.35, startButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
			putText(canvas, buttonTextStop, Point(stopButton.width + (stopButton.width * 0.35), stopButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));

			// Display the resulting frame
			imshow(appName, canvas);

			setMouseCallback(appName, callBackFunc, this);
			char c = waitKey(10);

			// Press  ESC on keyboard to exit
			if (c == 27)
				break;
		}
		closeVideo();
	}
}

void SSAGUI::closeVideo() {
	destroyAllWindows();
	getVideoStream().release();
}

void SSAGUI::callBackFunc(int event, int x, int y, int, void* userdata)
{
	SSAGUI* buttongui = reinterpret_cast<SSAGUI*>(userdata);
	buttongui->secondCall(event, x, y);
}

void SSAGUI::secondCall(int event, int x, int y)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		if (startButton.contains(Point(x, y)))
		{
			cout << "**start button clicked!" << endl;
			rectangle(frame, startButton, Scalar(0, 0, 255), 2);
			isPlaying = true;
		}
		else if (stopButton.contains(Point(x, y)))
		{
			cout << "--stop button clicked!" << endl;
			rectangle(frame, stopButton, Scalar(0, 0, 255), 2);
			isPlaying = false;
		}
		else {
			cout << "Coodinates: " << x << ", " << y << endl;
		}
	}

	if (event == EVENT_LBUTTONUP)
	{
		rectangle(frame, startButton, Scalar(200, 200, 200), 2);
		rectangle(frame, stopButton, Scalar(200, 200, 200), 2);
	}
}

//void SSAGUI::setupVideoGUI(Mat videoFrame) {
//    startButton = Rect(0, 0, videoFrame.cols / 2, 50);
//    stopButton = Rect(videoFrame.cols / 2, 0, frame.cols / 2, 50);
//    frame(startButton) = Vec3b(200, 200, 200); //Colour
//    frame(stopButton) = Vec3b(200, 200, 200); //Colour
//    putText(frame, buttonTextStart, Point(startButton.width * 0.35, startButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
//    putText(frame, buttonTextStop, Point(stopButton.width + (stopButton.width * 0.35), stopButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
//}
//
