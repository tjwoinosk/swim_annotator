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

			startButton = Rect(0, 0, frameResized.cols / 2, 50);
			stopButton = Rect(frameResized.cols / 2, 0, frameResized.cols / 2, 50);
			canvas = Mat3b(frameResized.rows + startButton.height, frameResized.cols, Vec3b(0, 0, 0));

			canvas(startButton) = buttonColor; //Colour
			canvas(stopButton) = buttonColor; //Colour

			if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == true) {
				std::vector<TrackingBox> trackingForThisFrame = frameAnalysisObj.analyzeVideo(frameResized);
				resultsTrackingSingleSwimmer.push_back(trackingForThisFrame[frameAnalysisObj.getindexSelectedSwimmer()]); 
				rectangle(frameResized, trackingForThisFrame[frameAnalysisObj.getindexSelectedSwimmer()], Scalar(100, 230, 0), 4);
			}
			else if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == false) {
				postProcessRealTimeTracking processObj;
				//std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame);
				std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frameResized); //TODO should we analyze frameResized or frame? (This applies to all uses of analyzeVideo()
				//TODO is this waht we want to do to deal with resizing? will this work? I think its buggy but I can't tell ... need to test more
				/*float scaleX = frame.cols / vidSize_width;
				float scaleY = frame.rows / vidSize_height;
				std::cout << " ------- X = " << scaleX << " Y = " << scaleY << std::endl;
				cv::Point_<float> p1 = cv::Point_<float>((1 / scaleX) * toGetTrajectoryFrom[indexID].x, (1 / scaleY) * toGetTrajectoryFrom[indexID].y);
				cv::Point_<float> p2 = cv::Point_<float>((1 / scaleX) * (toGetTrajectoryFrom[indexID].x + toGetTrajectoryFrom[indexID].width), (1 / scaleY) * (toGetTrajectoryFrom[indexID].y + toGetTrajectoryFrom[indexID].height));
				rectangle(frameResized, p1, p2, Scalar(150, 200, 150), 10);*/
				//TODO end
				rectangle(frameResized, toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()], Scalar(0, 190, 255), 4);
			}

			//TODO TEST ------ TEST ----- TEST -------------------------------------------------
			//---------------------
			//std::vector<TrackingBox> trackingForThisFrameTWO = testFrameTWo.analyzeVideo(frame);
			//float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
			//float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);

			//frameAnalysisObj.resizeBoxes(scaleX, scaleY, trackingForThisFrameTWO);
			//------
			frameAnalysis testFrameOne;
			frameAnalysis testFrameTWo;

			std::cout << std::endl << std::endl;
			std::vector<TrackingBox> trackingForThisFrame = testFrameOne.analyzeVideo(frameResized);
			for (int i = 0; i < trackingForThisFrame.size(); i++) {
				std::cout << " ++++    " << trackingForThisFrame[i] << std::endl;
			}
			
			std::vector<TrackingBox> trackingForThisFrameTWO = testFrameTWo.analyzeVideo(frame);
			float scaleX = testFrameTWo.findFrameScale(frameResized.cols, frame.cols);
			float scaleY = testFrameTWo.findFrameScale(frameResized.rows, frame.rows);
			std::cout << " FRAME WIDTH = " << frame.cols << " RESIZED = " << frameResized.cols << std::endl;
			std::cout << " FRAME HEIGHT = " << frame.rows << " RESIZED = " << frameResized.rows << std::endl;

			std::cout << " ------- X = " << scaleX << " Y = " << scaleY << std::endl;
			std::cout << " ------- 1/X = " << 1/scaleX << " 1/Y = " << 1/scaleY << std::endl;

			for (int i = 0; i < trackingForThisFrameTWO.size(); i++) {
				std::cout << " ^^^^   " << trackingForThisFrameTWO[i] << std::endl;
			}
			testFrameTWo.resizeBoxes(scaleX, scaleY, trackingForThisFrameTWO);

			for (int i = 0; i < trackingForThisFrame.size(); i++) {
				std::cout << " ****    " << trackingForThisFrameTWO[i] << std::endl;
			}
			
			//TODO END TEST ------ END TEST ----- END TEST -------------------------------------------------


			frameResized.copyTo(canvas(Rect(0, startButton.height, frameResized.cols, frameResized.rows)));

			putText(canvas, buttonTextStart, Point(startButton.width * 0.35, startButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
			putText(canvas, buttonTextStop, Point(stopButton.width + (stopButton.width * 0.35), stopButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
			
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
			rectangle(canvas, startButton, Scalar(0, 0, 255), 2);
			isPlaying = true;
		
			if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == false) { //TODO should we use a varaible in here or frameAnalysis?
				resultsTrackingSingleSwimmer.clear();
				frameAnalysisObj.setAnalyzeSwimmer(true);
				std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frameResized); 
				resultsTrackingSingleSwimmer.push_back(toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()]);
			}
		}
		else if (stopButton.contains(Point(x, y)))
		{
			rectangle(canvas, stopButton, Scalar(0, 0, 255), 2);
			isPlaying = false;

			if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == true) {
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
		else {
			postProcessRealTimeTracking processObj;
			std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frameResized);
			int idFound = processObj.trajectoryMatcher(cv::Point_<float>(x, y), toGetTrajectoryFrom); //TODO error check
			if (!frameAnalysisObj.setIDSelectedSwimmer(idFound)) { std::cout << std::endl<< "Failed to set ID of swimmer" << std::endl; }
			int indexSwimmer = frameAnalysisObj.findindexSelectedSwimmer(frameAnalysisObj.getIDSelectedSwimmer(), toGetTrajectoryFrom);
			if (!frameAnalysisObj.setindexSelectedSwimmer(indexSwimmer)) { std::cout << std::endl << "Failed to set index of swimmer" << std::endl; }

			//TODO is this what we want to do to deal with resizing? will this work?
			/*float scaleX = frame.cols / vidSize_width;
			float scaleY = frame.rows / vidSize_height;
			std::cout << " ------- X = " << scaleX << " Y = " << scaleY << std::endl;
			cv::Point_<float> p1 = cv::Point_<float>((1 / scaleX) * toGetTrajectoryFrom[indexID].x, (1 / scaleY) * toGetTrajectoryFrom[indexID].y);
			cv::Point_<float> p2 = cv::Point_<float>((1 / scaleX) * (toGetTrajectoryFrom[indexID].x + toGetTrajectoryFrom[indexID].width), (1 / scaleY) * (toGetTrajectoryFrom[indexID].y + toGetTrajectoryFrom[indexID].height));
			rectangle(frameResized, p1, p2, Scalar(150, 200, 150), 2);*/
			//TODO end
			
			rectangle(frameResized, toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()], Scalar(150, 200, 150), 10);
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

