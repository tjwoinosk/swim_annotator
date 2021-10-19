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

void SSAGUI::playVideo(int videoDelay) {
	if (isVideoStreamValid()) {
		while (1) {
			// Capture frame-by-frame
			Mat frame;
			getVideoStream() >> frame;
            frame(buttonStart) = Vec3b(200, 200, 200); //Colour
            frame(buttonStop) = Vec3b(200, 200, 200); //Colour

			// If the frame is empty, break immediately
			if (frame.empty())
				break;

			// Display the resulting frame
			imshow("Frame", frame);


			// Delay frames
			char c = (char)waitKey(videoDelay);

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
    SSAGUI* ssaGUI = reinterpret_cast<SSAGUI*>(userdata);
    ssaGUI->secondCall(event, x, y);
}

void SSAGUI::secondCall(int event, int x, int y)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        if (buttonStart.contains(Point(x, y)))
        {
            cout << "**START button clicked!" << endl;
            rectangle(canvas, buttonStart, Scalar(0, 0, 255), 2);
        }
        else if (buttonStop.contains(Point(x, y)))
        {
            cout << "--STOP button clicked!" << endl;
            rectangle(canvas, buttonStop, Scalar(0, 0, 255), 2);
        }
    }
    if (event == EVENT_LBUTTONUP)
    {
        rectangle(canvas, buttonStart, Scalar(200, 200, 200), 2);
        rectangle(canvas, buttonStop, Scalar(200, 200, 200), 2);
    }

    imshow(appName, canvas);
    waitKey(1);
}

void SSAGUI::runButton()
{
    // An image
    Mat3b img(400, 400, Vec3b(100, 200, 150));

    // Your button
    buttonStart = Rect(0, 0, img.cols / 2, 50);
    buttonStop = Rect(img.cols / 2, 0, img.cols / 2, 50);

    // The canvas
    canvas = Mat3b(img.rows + buttonStart.height, img.cols, Vec3b(0, 0, 0));
    //vertical height, width, colour

    // Draw the button
    canvas(buttonStart) = Vec3b(200, 200, 200); //Colour
    canvas(buttonStop) = Vec3b(200, 200, 200); //Colour
    //I think the operator () on canvas will take the input (buttonStart) and adjust the matrix at that
    //location (in canvas) to match buttonStart based on the x,y coordinates and height, width of buttonstart

    // Draw the image
    img.copyTo(canvas(Rect(0, buttonStart.height, img.cols, img.rows)));

    //Draw the text on the buttons - Draw last so they show up above all the other drawings
    putText(canvas, buttonTextStart, Point(buttonStart.width * 0.35, buttonStart.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
    putText(canvas, buttonTextStop, Point(buttonStop.width + (buttonStop.width * 0.35), buttonStop.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));

    // Setup callback function
    namedWindow(appName);
    setMouseCallback(appName, callBackFunc, this);

    imshow(appName, canvas);
    waitKey();
}