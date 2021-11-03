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
				//std::vector<TrackingBox> trackingForThisFrame = frameAnalysisObj.analyzeVideo(frame);
				std::vector<TrackingBox> trackingForThisFrame = frameAnalysisObj.analyzeVideo(frameResized); //TODO which one?
				//resultsTrackingSwimmer.push_back(trackingForThisFrame); 
				resultsTrackingSingleSwimmer.push_back(trackingForThisFrame[frameAnalysisObj.getindexSelectedSwimmer()]); //TODO we should only track one swimmer
				//TODO this needs to only push back for selected swimmer
				//TODO do we want to draw the box in this case as well?
				std::cout << "--------TRACKING------------" << std::endl << std::endl;
				for (int i = 0; i < trackingForThisFrame.size(); i++) {
					std::cout << " *** " << trackingForThisFrame[i] << std::endl;
				}
				std::cout << "---------TRACKING END-----------" << std::endl << std::endl;
			}
			else if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == false) {
				postProcessRealTimeTracking processObj;
				//std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame);
				std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frameResized);
				//TODO is this waht we want to do to deal with resizing? will this work? I think its buggy but I cant tell ... need to test more
				/*float scaleX = frame.cols / vidSize_width;
				float scaleY = frame.rows / vidSize_height;
				std::cout << " ------- X = " << scaleX << " Y = " << scaleY << std::endl;
				cv::Point_<float> p1 = cv::Point_<float>((1 / scaleX) * toGetTrajectoryFrom[indexID].x, (1 / scaleY) * toGetTrajectoryFrom[indexID].y);
				cv::Point_<float> p2 = cv::Point_<float>((1 / scaleX) * (toGetTrajectoryFrom[indexID].x + toGetTrajectoryFrom[indexID].width), (1 / scaleY) * (toGetTrajectoryFrom[indexID].y + toGetTrajectoryFrom[indexID].height));
				rectangle(frameResized, p1, p2, Scalar(150, 200, 150), 10);*/
				//TODO end
				rectangle(frameResized, toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()], Scalar(150, 200, 150), 10);
				std::cout << std::endl << std::endl << " finding next box with ID = " << frameAnalysisObj.getIDSelectedSwimmer() << "  and index  = " << frameAnalysisObj.getindexSelectedSwimmer() << std::endl << std::endl;
				std::cout << toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()] << std::endl;
				std::cout << "  x = " << toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()].x << " y = " << toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()].y << " w = " << toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()].width << " h = " << toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()].height << std::endl;
				
				for (int i = 0; i < toGetTrajectoryFrom.size(); i++) {
					std::cout << " *** " << toGetTrajectoryFrom[i] << std::endl;
				}
				std::cout << "--------------------" << std::endl << std::endl;

				//TODO TEST ---- TEST --- TEST --- DELETE AFTERWARDS ---------------------------------
				for (int i = 0; i < toGetTrajectoryFrom.size(); i++) {
					if (i == frameAnalysisObj.getindexSelectedSwimmer()) { continue; }
					if(toGetTrajectoryFrom[i].get_m_boxID() == 1)
						rectangle(frameResized, toGetTrajectoryFrom[i], Scalar(250, 0, 0), 5);
					if (toGetTrajectoryFrom[i].get_m_boxID() == 2)
						rectangle(frameResized, toGetTrajectoryFrom[i], Scalar(0, 250, 0), 5);
					if (toGetTrajectoryFrom[i].get_m_boxID() == 3)
						rectangle(frameResized, toGetTrajectoryFrom[i], Scalar(0, 0, 250), 5);
					if (toGetTrajectoryFrom[i].get_m_boxID() == 4)
						rectangle(frameResized, toGetTrajectoryFrom[i], Scalar(90, 90, 0), 5);
					if (toGetTrajectoryFrom[i].get_m_boxID() == 5)
						rectangle(frameResized, toGetTrajectoryFrom[i], Scalar(90, 0, 90), 5);
					if (toGetTrajectoryFrom[i].get_m_boxID() == 6)
						rectangle(frameResized, toGetTrajectoryFrom[i], Scalar(0, 250, 250), 5);
					if (toGetTrajectoryFrom[i].get_m_boxID() == 7)
						rectangle(frameResized, toGetTrajectoryFrom[i], Scalar(250, 250, 0), 5);
					if (toGetTrajectoryFrom[i].get_m_boxID() == 8)
						rectangle(frameResized, toGetTrajectoryFrom[i], Scalar(244, 99, 129), 5);
				}

				//TODO END TEST ---- END TEST --- END TEST --- END DELETE AFTERWARDS ---------------------------------


				//TODO so we resize the image to fit in a fixed size, but now do we pass in the resized image to the detector or original?
				// if we pass in original, we need to take the coordinates and find them for the resized
			}

			//here you must do rectangle(frameResized, cv::Point_<float>(10, 10), cv::Point_<float>(110, 110), Scalar(150, 100, 150), 10); 

			frameResized.copyTo(canvas(Rect(0, startButton.height, frameResized.cols, frameResized.rows)));

			//here you must do rectangle(canvas, cv::Point_<float>(110, 110), cv::Point_<float>(210, 210), Scalar(180, 200, 150), 10); 

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
			cout << "**start button clicked!" << endl;
			rectangle(canvas, startButton, Scalar(0, 0, 255), 2);
			isPlaying = true;
		
			if (frameAnalysisObj.getIDSelectedSwimmer() > -1 && frameAnalysisObj.getAnalyzeSwimmer() == false) { //TODO should we use a varaible in here or frameAnalysis?
				//resultsTrackingSwimmer.clear();
				resultsTrackingSingleSwimmer.clear();
				//std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame); //TODO error check
				frameAnalysisObj.setAnalyzeSwimmer(true);
				std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frameResized); //TODO error check
				//resultsTrackingSwimmer.push_back(toGetTrajectoryFrom); //push back analysis from this frame as well so it isn't missed
				resultsTrackingSingleSwimmer.push_back(toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()]);
				//TODO this needs to only push back for selected swimmer
			}
		}
		else if (stopButton.contains(Point(x, y)))
		{
			cout << "--stop button clicked!" << endl;
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
					/*for (int i = 0; i < resultsTrackingSwimmer.size(); i++) {
						for (int j = 0; j < resultsTrackingSwimmer[i].size(); j++) {
							resultsTrackingSwimmer[i][j].outputToFile(resultsFile);
						}
					}*/
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
			std::cout << std::endl << std::endl << std::endl;
			std::cout << "Coodinates: " << x << ", " << y << std::endl;

			//TODO is it okay to assume that a click outside of stop and start button is only on the frame itself?
			postProcessRealTimeTracking processObj;
			//std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame);
			std::vector<TrackingBox> toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frameResized);
			int idFound = processObj.trajectoryMatcher(cv::Point_<float>(x, y), toGetTrajectoryFrom); //TODO error check
			if (!frameAnalysisObj.setIDSelectedSwimmer(idFound)) { std::cout << std::endl<< "Failed to set ID of swimmer" << std::endl; }
			std::cout << std::endl << std::endl <<" SWIMMER ID = " << frameAnalysisObj.getIDSelectedSwimmer() << std::endl << std::endl;
			//TODO below is not efficient
			int indexSwimmer = frameAnalysisObj.findindexSelectedSwimmer(frameAnalysisObj.getIDSelectedSwimmer(), toGetTrajectoryFrom);
			if (!frameAnalysisObj.setindexSelectedSwimmer(indexSwimmer)) { std::cout << std::endl << "Failed to set index of swimmer" << std::endl; }

			//TODO is this waht we want to do to deal with resizing? will this work?
			/*float scaleX = frame.cols / vidSize_width;
			float scaleY = frame.rows / vidSize_height;
			std::cout << " ------- X = " << scaleX << " Y = " << scaleY << std::endl;
			cv::Point_<float> p1 = cv::Point_<float>((1 / scaleX) * toGetTrajectoryFrom[indexID].x, (1 / scaleY) * toGetTrajectoryFrom[indexID].y);
			cv::Point_<float> p2 = cv::Point_<float>((1 / scaleX) * (toGetTrajectoryFrom[indexID].x + toGetTrajectoryFrom[indexID].width), (1 / scaleY) * (toGetTrajectoryFrom[indexID].y + toGetTrajectoryFrom[indexID].height));
			rectangle(frameResized, p1, p2, Scalar(150, 200, 150), 2);*/
			//TODO end
			for (int i = 0; i < toGetTrajectoryFrom.size(); i++) {
				std::cout << " *** " << toGetTrajectoryFrom[i] << std::endl;
			}
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

//void SSAGUI::setupVideoGUI(Mat videoFrame) {
//    startButton = Rect(0, 0, videoFrame.cols / 2, 50);
//    stopButton = Rect(videoFrame.cols / 2, 0, frame.cols / 2, 50);
//    frame(startButton) = Vec3b(200, 200, 200); //Colour
//    frame(stopButton) = Vec3b(200, 200, 200); //Colour
//    putText(frame, buttonTextStart, Point(startButton.width * 0.35, startButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
//    putText(frame, buttonTextStop, Point(stopButton.width + (stopButton.width * 0.35), stopButton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
//}
//
