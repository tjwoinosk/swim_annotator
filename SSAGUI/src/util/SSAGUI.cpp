// VideoStream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "SSAGUI.h"

SSAGUI::SSAGUI(string videoFile) {
	video = videoFile;
	videoStream.open(videoFile);
	frameAnalysisObj.setAnalyzeSwimmer(false);
	frameAnalysisObj.setIDSelectedSwimmer(-1);
	frameAnalysisObj.setindexSelectedSwimmer(0); //TODO should this be zero?
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

			//Videos can come in multiple sizes, need output in a fixed size
			cv::resize(frame, frameResized, cv::Size(vidSize_width, vidSize_height)); 

			int widthButtons = frameResized.cols / 3;
			startButton = Rect(0, 0, widthButtons, buttonHeight);
			stopButton = Rect(frameResized.cols / 3, 0, widthButtons+1, buttonHeight);
			cancelButton = Rect(stopButton.x+stopButton.width, 0, widthButtons+1, buttonHeight);
			canvas = Mat3b(frameResized.rows + startButton.height, frameResized.cols, Vec3b(0, 0, 0));

			canvas(startButton) = buttonColor; //Colour
			canvas(stopButton) = buttonColor; //Colour
			canvas(cancelButton) = buttonColor; //Colour

			if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == true) {
				//A swimmer is selected and we want to track the swimmer
				std::cout << std::endl << "in main - tracking" << std::endl;
				std::vector<TrackingBox> trackingForThisFrame = frameAnalysisObj.analyzeVideo(frame);
				resultsTrackingSingleSwimmer.push_back(trackingForThisFrame[frameAnalysisObj.getindexSelectedSwimmer()]); 
				float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
				float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);
				TrackingBox newBox = frameAnalysisObj.resizeBox(scaleX, scaleY, trackingForThisFrame[frameAnalysisObj.getindexSelectedSwimmer()]);
				rectangle(frameResized, newBox, Scalar(100, 230, 0), 4);
			}
			else if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == false) {
				//A swimmer is selected but we are not yet tracking the swimmer
				std::cout << std::endl << "in main - selected but not tracking" << std::endl;
				postProcessRealTimeTracking processObj;
				std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame); 
				float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
				float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);
				TrackingBox newBox = frameAnalysisObj.resizeBox(scaleX, scaleY, toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()]);
				rectangle(frameResized, newBox, Scalar(0, 190, 255), 4);
			}

			frameResized.copyTo(canvas(Rect(0, startButton.height, frameResized.cols, frameResized.rows)));

			putText(canvas, buttonTextStart, Point(startButton.width * 0.35, startButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
			putText(canvas, buttonTextStop, Point(stopButton.x + (stopButton.width * 0.35), stopButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
			putText(canvas, buttonTextCancel, Point(cancelButton.x + (cancelButton.width * 0.35), cancelButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));

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
			std::cout << std::endl<<" start button clicked" << std::endl;
			rectangle(canvas, startButton, Scalar(0, 0, 255), 2);
			isPlaying = true;
		
			if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == false) { 
				std::cout << std::endl << " start button - tracking" << std::endl;
				resultsTrackingSingleSwimmer.clear();
				frameAnalysisObj.setAnalyzeSwimmer(true);
				std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame); 
				resultsTrackingSingleSwimmer.push_back(toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()]);
				float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
				float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);
				TrackingBox newBox = frameAnalysisObj.resizeBox(scaleX, scaleY, toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()]);
				rectangle(frameResized, newBox, Scalar(0, 190, 255), 4);
			}
		}
		else if (stopButton.contains(Point(x, y)))
		{
			std::cout << std::endl << " stop button clicked" << std::endl;
			rectangle(canvas, stopButton, Scalar(0, 0, 255), 2);
			isPlaying = false;

			if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == true) {
				std::cout << std::endl << " stop button - writing data" << std::endl;
				frameAnalysisObj.setAnalyzeSwimmer(false);
				frameAnalysisObj.setIDSelectedSwimmer(-1);
				//TODO write to file the output - overwrite or append?
				fileFinder find;
				std::string resFileName = "detectionDataNEW.txt";
				std::string resFileAbsPath = "";
				std::ofstream resultsFile;

				try
				{
					resFileAbsPath = find.returnDataLocation() + resFileName;
					resultsFile.open(resFileAbsPath);
					
					for (int i = 0; i < resultsTrackingSingleSwimmer.size(); i++) {
						resultsTrackingSingleSwimmer[i].outputToFile(resultsFile);
					}
					resultsFile.close();
				}
				catch (const std::exception& e)
				{
					std::cout << "Could not open " << resFileAbsPath << std::endl << e.what() << std::endl;
				}

			}
		}
		else if (cancelButton.contains(Point(x, y))) {
			std::cout << std::endl << " cancel button clicked" << std::endl;
			rectangle(canvas, cancelButton, Scalar(0, 0, 255), 2);
			//If you selected a swimmer, but didnt start tracking, then you can cancel your selection
			if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == false) {
				frameAnalysisObj.setIDSelectedSwimmer(-1);
			}
		}
		else {
			std::cout << std::endl << " on screen clicked" << std::endl;
			postProcessRealTimeTracking processObj;
			std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame);
			float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
			float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);
			frameAnalysisObj.resizeBoxes(scaleX, scaleY, toGetTrajectoryFrom);

			int y_inFrame = y - buttonHeight; //Account for offset from buttons to get position on the video image
			int idFound = processObj.trajectoryMatcher(cv::Point_<float>(x, y_inFrame), toGetTrajectoryFrom); //TODO error check
			if (!frameAnalysisObj.setIDSelectedSwimmer(idFound)) { std::cout << std::endl<< "Failed to set ID of swimmer" << std::endl; }
			int indexSwimmer = frameAnalysisObj.findindexSelectedSwimmer(frameAnalysisObj.getIDSelectedSwimmer(), toGetTrajectoryFrom);
			if (!frameAnalysisObj.setindexSelectedSwimmer(indexSwimmer)) { std::cout << std::endl << "Failed to set index of swimmer" << std::endl; }
			
			rectangle(frameResized, toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()], Scalar(150, 200, 150), 10);
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

