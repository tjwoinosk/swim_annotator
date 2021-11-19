// VideoStream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "SSAGUI.h"

SSAGUI::SSAGUI(string videoFile) {
	video = videoFile;
	videoStream.open(videoFile);
	frameAnalysisObj_ptr = NULL;
}

SSAGUI::~SSAGUI() {

}

VideoCapture& SSAGUI::getVideoStream() {
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

void SSAGUI::playVideo() { 

	if (isVideoStreamValid()) {
		while (1) {
			// Capture frame-by-frame
			if (isPlaying)
				getVideoStream() >> frame;

			// If the frame is empty, break immediately
			if (frame.empty())
				break;

			drawOnFrame();

			setMouseCallback(appName, callBackFunc, this);
			char c = waitKey(VIDEO_DELAY);

			// Display the resulting frame
			imshow(appName, canvas); 

			// Press  ESC on keyboard to exit
			if (c == 27)
				break;
		}
		closeVideo();
	}
}

void SSAGUI::playVideoTest(bool withCancellation, int frameNum_selectSwimmer, int frameNum_startTracking, int frameNum_stopTracking, cv::Point_<float> mouseClick_Test) 
{
	if (isVideoStreamValid()) {
		while (1) {
			if (isPlaying)
				getVideoStream() >> frame;

			if (frame.empty())
				break;

			drawOnFrame();

			if (getVideoStream().get(CAP_PROP_POS_FRAMES) == frameNum_selectSwimmer) {
				secondCall(cv::EVENT_LBUTTONDOWN, mouseClick_Test.x, mouseClick_Test.y + BUTTON_HEIGHT); //x,y = location of swimmer on screen, note y = y of swimmer + buttonHeight
			}

			if (withCancellation) {
				if (getVideoStream().get(CAP_PROP_POS_FRAMES) == frameNum_startTracking) {
					secondCall(cv::EVENT_LBUTTONDOWN, cancelButton.x + 10, cancelButton.y + 10);
				}
			}
			else {
				if (getVideoStream().get(CAP_PROP_POS_FRAMES) == frameNum_startTracking) {
					secondCall(cv::EVENT_LBUTTONDOWN, startButton.x + 10, startButton.y + 10);
				}
				else if (getVideoStream().get(CAP_PROP_POS_FRAMES) == frameNum_stopTracking && isPlaying != false) {
					secondCall(cv::EVENT_LBUTTONDOWN, stopButton.x + 10, stopButton.y + 10);
				}
				else if (getVideoStream().get(CAP_PROP_POS_FRAMES) == frameNum_stopTracking && isPlaying == false) {
					waitKey(50);
					secondCall(cv::EVENT_LBUTTONDOWN, startButton.x + 10, startButton.y + 10);
				}
			}

			char c = waitKey(VIDEO_DELAY);

			// Display the resulting frame
			imshow(appName, canvas);

			// Press  ESC on keyboard to exit
			if (c == 27)
				break;
		}
		closeVideo();
	}
}

void SSAGUI::setFrameAnalysis(frameAnalysis& frameObj)
{
	frameAnalysisObj_ptr = &frameObj; 
	//TODO check for memory leaks
}

void SSAGUI::drawOnFrame()
{
	//TODO pick which method of resizing is appropriate, and what size
	/*
	float resizeRatioWidth = vidSize/ static_cast<float>(frame.rows);
	float resieRatioHeight = vidSize / static_cast<float>(frame.cols);
	cv::resize(frame, frameResized, cv::Size(), resizeRatioWidth, resieRatioHeight);
	*/

	//Videos can come in multiple sizes, need output in a fixed size 
	cv::resize(frame, frameResized, cv::Size(VIDSIZE_WIDTH, VIDSIZE_HEIGHT), INTER_LINEAR);


	int widthButtons = frameResized.cols / 3;
	startButton = Rect(0, 0, widthButtons, BUTTON_HEIGHT);
	stopButton = Rect(frameResized.cols / 3, 0, widthButtons + 1, BUTTON_HEIGHT);
	cancelButton = Rect(stopButton.x + stopButton.width, 0, widthButtons + 1, BUTTON_HEIGHT);
	canvas = Mat3b(frameResized.rows + startButton.height, frameResized.cols, Vec3b(0, 0, 0));

	canvas(startButton) = buttonColor; //Colour
	canvas(stopButton) = buttonColor; //Colour
	canvas(cancelButton) = buttonColor; //Colour

	if (frameAnalysisObj_ptr->isFollowing() && frameAnalysisObj_ptr->isTracking()) {
		//A swimmer is selected and we want to track the swimmer
		TrackingBox trackingForThisFrame = frameAnalysisObj_ptr->analyzeVideo(frame);
		float scaleX = frameAnalysisObj_ptr->findFrameScale(frameResized.cols, frame.cols);
		float scaleY = frameAnalysisObj_ptr->findFrameScale(frameResized.rows, frame.rows);
		TrackingBox newBox = frameAnalysisObj_ptr->resizeBox(scaleX, scaleY, trackingForThisFrame);
		rectangle(frameResized, newBox, Scalar(100, 230, 0), 4);
	}
	else if (frameAnalysisObj_ptr->isFollowing() && !frameAnalysisObj_ptr->isTracking()) {
		//A swimmer is selected but we are not yet tracking the swimmer
		TrackingBox toGetTrajectoryFrom = frameAnalysisObj_ptr->analyzeVideo(frame);
		float scaleX = frameAnalysisObj_ptr->findFrameScale(frameResized.cols, frame.cols);
		float scaleY = frameAnalysisObj_ptr->findFrameScale(frameResized.rows, frame.rows);
		TrackingBox newBox = frameAnalysisObj_ptr->resizeBox(scaleX, scaleY, toGetTrajectoryFrom);
		rectangle(frameResized, newBox, Scalar(0, 190, 255), 4);
	}

	frameResized.copyTo(canvas(Rect(0, startButton.height, frameResized.cols, frameResized.rows)));

	putText(canvas, buttonTextStart, Point(startButton.width * 0.35, startButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
	putText(canvas, buttonTextStop, Point(stopButton.x + (stopButton.width * 0.35), stopButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
	putText(canvas, buttonTextCancel, Point(cancelButton.x + (cancelButton.width * 0.35), cancelButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));

	return;
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
			std::cout << std::endl<<" start button clicked" << std::endl;
			rectangle(canvas, startButton, Scalar(0, 0, 255), 2);
			isPlaying = true;
		
			if (frameAnalysisObj_ptr->isFollowing() && !frameAnalysisObj_ptr->isTracking()) {  //TODO deal with index error checking - in case ID went to another index
				frameAnalysisObj_ptr->setStatus(true, frameAnalysisObj_ptr->getIDSelectedSwimmer());
				TrackingBox toGetTrajectoryFrom = frameAnalysisObj_ptr->analyzeVideo(frame);
				float scaleX = frameAnalysisObj_ptr->findFrameScale(frameResized.cols, frame.cols);
				float scaleY = frameAnalysisObj_ptr->findFrameScale(frameResized.rows, frame.rows);
				TrackingBox newBox = frameAnalysisObj_ptr->resizeBox(scaleX, scaleY, toGetTrajectoryFrom);
				rectangle(frameResized, newBox, Scalar(0, 190, 255), 4);
			}
		}
		else if (stopButton.contains(Point(x, y)))
		{
			std::cout << std::endl << " stop button clicked" << std::endl;
			rectangle(canvas, stopButton, Scalar(0, 0, 255), 2);
			isPlaying = false;

			if (frameAnalysisObj_ptr->isFollowing() && frameAnalysisObj_ptr->isTracking()) {
				frameAnalysisObj_ptr->setStatus(false, -1);
				frameAnalysisObj_ptr->writeToFile();
			}
		}
		else if (cancelButton.contains(Point(x, y))) {
			std::cout << std::endl << " cancel button clicked" << std::endl;
			rectangle(canvas, cancelButton, Scalar(0, 0, 255), 2);
			//If you selected a swimmer, but didnt start tracking, then you can cancel your selection
			if (frameAnalysisObj_ptr->isFollowing() && !frameAnalysisObj_ptr->isTracking()) {
				frameAnalysisObj_ptr->setStatus(false, -1);
			}
		}
		else {
			if (!frameAnalysisObj_ptr->isFollowing()) {
				frameAnalysisObj_ptr->setVideoData(frame, 0.1, 0.1); //TODO What should be deltaX and deltaY? I just added random value
				std::cout << std::endl << " on screen clicked at x = " << x << " , y = " << y << std::endl;
				postProcessRealTimeTracking processObj;
				frameAnalysisObj_ptr->analyzeVideo(frame);
				float scaleX = frameAnalysisObj_ptr->findFrameScale(frameResized.cols, frame.cols);
				float scaleY = frameAnalysisObj_ptr->findFrameScale(frameResized.rows, frame.rows);
				std::vector<TrackingBox> resultsCurrent = frameAnalysisObj_ptr->getCurrentResults();
				frameAnalysisObj_ptr->resizeBoxes(scaleX, scaleY, resultsCurrent);

				int y_inFrame = y - BUTTON_HEIGHT; //Account for offset from buttons to get position on the video image
				int idFound = processObj.trajectoryMatcher(cv::Point_<float>(x, y_inFrame), resultsCurrent); //TODO error check
				frameAnalysisObj_ptr->setStatus(false, idFound);

				rectangle(frameResized, resultsCurrent[frameAnalysisObj_ptr->getindexSelectedSwimmer()], Scalar(150, 200, 150), 10);
			}
		}

		imshow(appName, canvas);
		waitKey(10);
	}

	if (event == EVENT_LBUTTONUP)
	{
		rectangle(canvas, startButton, Scalar(200, 200, 200), 2);
		rectangle(canvas, stopButton, Scalar(200, 200, 200), 2);
		rectangle(canvas, cancelButton, Scalar(200, 200, 200), 2);
	}
}

