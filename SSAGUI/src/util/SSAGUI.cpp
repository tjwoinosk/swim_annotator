// VideoStream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "SSAGUI.h"

SSAGUI::SSAGUI(string videoFile) {
	video = videoFile;
	videoStream.open(videoFile);
	startAnalyzeSwimmer = false;
	idSwimmerSelected = -1;
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

			if (idSwimmerSelected > -1 && startAnalyzeSwimmer == true) {
				std::vector<TrackingBox> trackingForThisFrame = frameAnalysisObj.analyzeVideo(frame);
				resultsTrackingSwimmer.push_back(trackingForThisFrame); 
				//TODO this needs to only push back for selected swimmer
			}
			//else if (idSwimmerSelected > -1 && startAnalyzeSwimmer == false) {
			//	postProcessRealTimeTracking processObj;
			//	std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame);
			//	//rectangle(frameResized, toGetTrajectoryFrom[idSwimmerSelected], Scalar(150, 200, 150), 2);
			//	rectangle(canvas, toGetTrajectoryFrom[idSwimmerSelected], Scalar(150, 200, 150), 2);
			//	std::cout << std::endl << std::endl << " finding next box" << std::endl << std::endl;
			//}

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
			
			if (idSwimmerSelected > -1 && startAnalyzeSwimmer == false) {
				postProcessRealTimeTracking processObj;
				std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame);
				rectangle(frameResized, toGetTrajectoryFrom[idSwimmerSelected], Scalar(150, 200, 150), 2);
				//rectangle(canvas, toGetTrajectoryFrom[idSwimmerSelected], Scalar(150, 200, 150), 2);
				std::cout << std::endl << std::endl << " finding next box" << std::endl << std::endl;
			}

			setMouseCallback(appName, callBackFunc, this);
			char c = waitKey(10);

			// Display the resulting frame
			imshow(appName, canvas); 

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

			rectangle(canvas, startButton, Scalar(0, 0, 255), 2);
			isPlaying = true;
		
			if (idSwimmerSelected > -1 && startAnalyzeSwimmer == false) { //TODO should we use a varaible in here or frameAnalysis?
				resultsTrackingSwimmer.clear();
				std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame); //TODO error check
				startAnalyzeSwimmer = true;
				resultsTrackingSwimmer.push_back(toGetTrajectoryFrom); //push back analysis from this frame as well so it isn't missed
				//TODO this needs to only push back for selected swimmer
			}
		}
		else if (stopButton.contains(Point(x, y)))
		{
			cout << "--stop button clicked!" << endl;
			rectangle(canvas, stopButton, Scalar(0, 0, 255), 2);
			isPlaying = false;

			if (idSwimmerSelected > -1 && startAnalyzeSwimmer == true) {
				startAnalyzeSwimmer = false;
				idSwimmerSelected = -1;
				//TODO write to file the output - overwrite or append?
				fileFinder find;
				std::string resFileName = "detectionDataNEW.txt";
				std::string resFileAbsPath = "";
				std::ofstream resultsFile;

				try
				{
					resFileAbsPath = find.returnDataLocation() + resFileName;
					resultsFile.open(resFileAbsPath);
					for (int i = 0; i < resultsTrackingSwimmer.size(); i++) {
						for (int j = 0; j < resultsTrackingSwimmer[i].size(); j++) {
							resultsTrackingSwimmer[i][j].outputToFile(resultsFile);
						}
					}
					resultsFile.close();
				}
				catch (const std::exception& e)
				{
					std::cout << "Could not open " << resFileAbsPath << std::endl << e.what() << std::endl;
				}

			}
		}
		else {

			//TODO is it okay to assume that a click outside of stop and start button is only on the frame itself?
			postProcessRealTimeTracking processObj;
			std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame);
			idSwimmerSelected = processObj.trajectoryMatcher(cv::Point_<float>(x, y), toGetTrajectoryFrom); //TODO error check
			std::cout << std::endl << std::endl <<" SWIMMER ID = " << idSwimmerSelected << std::endl << std::endl;
			rectangle(frameResized, toGetTrajectoryFrom[idSwimmerSelected], Scalar(150, 200, 150), 2);

			cout << "Coodinates: " << x << ", " << y << endl;
		}

		imshow(appName, canvas);
		waitKey(10);
	}

	if (event == EVENT_LBUTTONUP)
	{
		rectangle(canvas, startButton, Scalar(200, 200, 200), 2);
		rectangle(canvas, stopButton, Scalar(200, 200, 200), 2);
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
