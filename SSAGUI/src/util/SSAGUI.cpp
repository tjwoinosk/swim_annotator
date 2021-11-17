// VideoStream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "SSAGUI.h"

SSAGUI::SSAGUI(string videoFile) {
	video = videoFile;
	videoStream.open(videoFile);
	frameAnalysisObj.setAnalyzeSwimmer(false);
	frameAnalysisObj.setIDSelectedSwimmer(-1);
	frameAnalysisObj.setindexSelectedSwimmer(0); //TODO should this be zero?

	frameAnalysisObj_ptr = NULL;
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

void SSAGUI::playVideo() { //TODO find and remove all delays

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

void SSAGUI::playVideoTest(bool withCancellation, int frameNum_selectSwimmer, int frameNum_startTracking, int frameNum_stopTracking, cv::Point_<float> mouseClick_Test) //TODO do we want to check if testVid is true/false?
{
	//TODO do we want to ensure testing is set so it doesn't interfere with the playVideo() function? since they use the same variables when using the mouse callback function
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
	cv::resize(frame, frameResized, cv::Size(VIDSIZE_WIDTH, VIDSIZE_HEIGHT));


	int widthButtons = frameResized.cols / 3;
	startButton = Rect(0, 0, widthButtons, BUTTON_HEIGHT);
	stopButton = Rect(frameResized.cols / 3, 0, widthButtons + 1, BUTTON_HEIGHT);
	cancelButton = Rect(stopButton.x + stopButton.width, 0, widthButtons + 1, BUTTON_HEIGHT);
	canvas = Mat3b(frameResized.rows + startButton.height, frameResized.cols, Vec3b(0, 0, 0));

	canvas(startButton) = buttonColor; //Colour
	canvas(stopButton) = buttonColor; //Colour
	canvas(cancelButton) = buttonColor; //Colour

	std::cout << std::endl << " IN DRAW FRAME: drew rectangles " << std::endl;

	//if (frameAnalysisObj.isFollowing() && frameAnalysisObj.isTracking()) {
	if (frameAnalysisObj_ptr->isFollowing() && frameAnalysisObj_ptr->isTracking()) {
		//A swimmer is selected and we want to track the swimmer
		std::cout << std::endl << "in main - tracking frame num " << getVideoStream().get(CAP_PROP_POS_FRAMES) << std::endl;
		//TrackingBox trackingForThisFrame = frameAnalysisObj.analyzeVideo(frame);
		TrackingBox trackingForThisFrame = frameAnalysisObj_ptr->analyzeVideo(frame);
		//resultsTrackingSingleSwimmer.push_back(trackingForThisFrame[frameAnalysisObj.getindexSelectedSwimmer()]);
		//float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
		//float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);
		//TrackingBox newBox = frameAnalysisObj.resizeBox(scaleX, scaleY, trackingForThisFrame);
		float scaleX = frameAnalysisObj_ptr->findFrameScale(frameResized.cols, frame.cols);
		float scaleY = frameAnalysisObj_ptr->findFrameScale(frameResized.rows, frame.rows);
		TrackingBox newBox = frameAnalysisObj_ptr->resizeBox(scaleX, scaleY, trackingForThisFrame);
		rectangle(frameResized, newBox, Scalar(100, 230, 0), 4);
	}
	//else if (frameAnalysisObj.isFollowing() && !frameAnalysisObj.isTracking()) {
	else if (frameAnalysisObj_ptr->isFollowing() && !frameAnalysisObj_ptr->isTracking()) {
		//A swimmer is selected but we are not yet tracking the swimmer
		std::cout << std::endl << "in main - selected but not tracking" << std::endl;
		//postProcessRealTimeTracking processObj;
		//TrackingBox toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame);
		//float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
		//float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);
		//TrackingBox newBox = frameAnalysisObj.resizeBox(scaleX, scaleY, toGetTrajectoryFrom);
		TrackingBox toGetTrajectoryFrom = frameAnalysisObj_ptr->analyzeVideo(frame);
		float scaleX = frameAnalysisObj_ptr->findFrameScale(frameResized.cols, frame.cols);
		float scaleY = frameAnalysisObj_ptr->findFrameScale(frameResized.rows, frame.rows);
		TrackingBox newBox = frameAnalysisObj_ptr->resizeBox(scaleX, scaleY, toGetTrajectoryFrom);
		rectangle(frameResized, newBox, Scalar(0, 190, 255), 4);
	}
	std::cout << std::endl << " IN DRAW FRAME - finishing " << std::endl;


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
		
			//if (frameAnalysisObj.isFollowing() && !frameAnalysisObj.isTracking()) {  //TODO deal with index error checking - in case ID went to another index
			if (frameAnalysisObj_ptr->isFollowing() && !frameAnalysisObj_ptr->isTracking()) {  //TODO deal with index error checking - in case ID went to another index
				std::cout << std::endl << " start button - tracking" << std::endl;
				//resultsTrackingSingleSwimmer.clear();
				//frameAnalysisObj.setAnalyzeSwimmer(true);
				//frameAnalysisObj.setStatus(true, frameAnalysisObj.getIDSelectedSwimmer());
				frameAnalysisObj_ptr->setStatus(true, frameAnalysisObj_ptr->getIDSelectedSwimmer());
				//TrackingBox toGetTrajectoryFrom = frameAnalysisObj.analyzeVideo(frame); 
				TrackingBox toGetTrajectoryFrom = frameAnalysisObj_ptr->analyzeVideo(frame);
				//resultsTrackingSingleSwimmer.push_back(toGetTrajectoryFrom[frameAnalysisObj.getindexSelectedSwimmer()]);
				//float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
				//float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);
				//TrackingBox newBox = frameAnalysisObj.resizeBox(scaleX, scaleY, toGetTrajectoryFrom);
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

			//if (frameAnalysisObj.isFollowing() && frameAnalysisObj.isTracking()) {
			if (frameAnalysisObj_ptr->isFollowing() && frameAnalysisObj_ptr->isTracking()) {
				std::cout << std::endl << " stop button - writing data" << std::endl;
				//frameAnalysisObj.setStatus(false, -1);
				frameAnalysisObj_ptr->setStatus(false, -1);
				//frameAnalysisObj.setAnalyzeSwimmer(false);
				//frameAnalysisObj.setIDSelectedSwimmer(-1);
				//TODO write to file the output - overwrite or append?
				//frameAnalysisObj.writeToFile();
				frameAnalysisObj_ptr->writeToFile();
			}
		}
		else if (cancelButton.contains(Point(x, y))) {
			std::cout << std::endl << " cancel button clicked" << std::endl;
			rectangle(canvas, cancelButton, Scalar(0, 0, 255), 2);
			//If you selected a swimmer, but didnt start tracking, then you can cancel your selection
			//if (frameAnalysisObj.isFollowing() && !frameAnalysisObj.isTracking()) {
			if (frameAnalysisObj_ptr->isFollowing() && !frameAnalysisObj_ptr->isTracking()) {
				//frameAnalysisObj.setIDSelectedSwimmer(-1);
				std::cout << std::endl << " setting CANCEL" << std::endl;
				//frameAnalysisObj.setStatus(false, -1);
				frameAnalysisObj_ptr->setStatus(false, -1);
			}
		}
		else {
			std::cout << std::endl << " on screen clicked at x = " << x << " , y = " << y << std::endl;
			postProcessRealTimeTracking processObj;
			//frameAnalysisObj.analyzeVideo(frame);
			//float scaleX = frameAnalysisObj.findFrameScale(frameResized.cols, frame.cols);
			//float scaleY = frameAnalysisObj.findFrameScale(frameResized.rows, frame.rows);
			//std::vector<TrackingBox> resultsCurrent = frameAnalysisObj.getCurrentResults();
			//frameAnalysisObj.resizeBoxes(scaleX, scaleY, resultsCurrent);
			frameAnalysisObj_ptr->analyzeVideo(frame);
			float scaleX = frameAnalysisObj_ptr->findFrameScale(frameResized.cols, frame.cols);
			float scaleY = frameAnalysisObj_ptr->findFrameScale(frameResized.rows, frame.rows);
			std::vector<TrackingBox> resultsCurrent = frameAnalysisObj_ptr->getCurrentResults();
			frameAnalysisObj_ptr->resizeBoxes(scaleX, scaleY, resultsCurrent);

			int y_inFrame = y - BUTTON_HEIGHT; //Account for offset from buttons to get position on the video image
			int idFound = processObj.trajectoryMatcher(cv::Point_<float>(x, y_inFrame), resultsCurrent); //TODO error check
			
			//if (!frameAnalysisObj.setIDSelectedSwimmer(idFound)) { std::cout << std::endl<< "Failed to set ID of swimmer" << std::endl; }
			//int indexSwimmer = frameAnalysisObj.findindexSelectedSwimmer(frameAnalysisObj.getIDSelectedSwimmer(), resultsCurrent);
			//if (!frameAnalysisObj.setindexSelectedSwimmer(indexSwimmer)) { std::cout << std::endl << "Failed to set index of swimmer" << std::endl; }
			//frameAnalysisObj.setStatus(false, idFound);
			frameAnalysisObj_ptr->setStatus(false, idFound);

			//rectangle(frameResized, resultsCurrent[frameAnalysisObj.getindexSelectedSwimmer()], Scalar(150, 200, 150), 10);
			rectangle(frameResized, resultsCurrent[frameAnalysisObj_ptr->getindexSelectedSwimmer()], Scalar(150, 200, 150), 10);

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

//TODO what file format to accept and save in?
/*
void SSAGUI::make_video(string video_name, string sub_video_name)
{
	string str, outputFile;
	VideoCapture cap;
	VideoWriter video;
	Mat frame, crop_frame;
	const int final_aspect = 150;
	int hight = 0, width = 0;
	int v_hight = 0, v_width = 0, m_hight = 0, m_width = 0;
	int x = 0, y = 0, box_w = 0, box_h = 0;
	int ii = 0;

	//TODO added this to deal with sorted_data_is_valid
	bool data_is_valid = true;
	for (int i = 0; i < resultsTrackingSingleSwimmer.size(); i++) {
		if (resultsTrackingSingleSwimmer[ii].x < 0) {
			data_is_valid = false;
		}
	}
	//end 

	if (data_is_valid) {
		// Open the video file
		std::cout << "Opening... " << video_name << endl;
		cap.open(video_name); //TODO now we don't need to reopen do we? we are doing this after finishing though...
		if (!cap.isOpened()) {
			std::cout << "Issues opening " << video_name << endl;
		}

		str = sub_video_name;
		//str.replace(str.end() - 4, str.end(), "_sub_video.avi"); //TODO avi or mp4?
		outputFile = str;

		//Decided best hight and width to take from the origanl video
		find_best_aspect(hight, width);

		// Get the video writer initialized to save the output video
		//video.open(outputFile, VideoWriter::fourcc('M', 'J', 'P', 'G'), 28, Size(final_aspect, final_aspect));
		video.open(outputFile, VideoWriter::fourcc('m', 'p', '4', 'v'), 28, Size(final_aspect, final_aspect));
		//TODO is the above okay? and is there a better way? what format do we want?
		//TDO the above is from  https://stackoverflow.com/questions/61807359/write-frame-into-mp4-format-using-opencv2-and-c

		//get starting values
		cap >> frame;
		Mat frame_buf(frame.rows + hight * 2, frame.cols + width * 2, frame.depth());

		//namedWindow("tester", WINDOW_NORMAL);

		for (ii = 0; ii < resultsTrackingSingleSwimmer.size(); ii++) {

			// get frame from the video
			if (resultsTrackingSingleSwimmer[ii].get_m_frame() != cap.get(CAP_PROP_POS_FRAMES)) {
				cap.set(CAP_PROP_POS_FRAMES, resultsTrackingSingleSwimmer[ii].get_m_frame()); //might make this very slow
			}
			cap >> frame;

			//needed to work cap object!!!!
			char c = (char)waitKey();
			if (c == 27)
				break;

			// Stop the program if reached end of video
			if (frame.empty()) {
				std::cout << "Done processing !!!" << endl;
				std::cout << "Output file is stored as " << outputFile << endl;
				break;
			}


			// form a border in-place
			copyMakeBorder(frame, frame_buf, hight, hight, width, width, BORDER_CONSTANT, 0);
			x = resultsTrackingSingleSwimmer[ii].x;
			box_w = resultsTrackingSingleSwimmer[ii].width;
			y = resultsTrackingSingleSwimmer[ii].y;
			box_h = resultsTrackingSingleSwimmer[ii].height;
			crop_frame = frame_buf(Rect(x + box_w / 2 + width / 2, y + box_h / 2 + hight / 2, width, hight));

			//scale video to a standard size
			resize(crop_frame, crop_frame, Size(final_aspect, final_aspect), 0, 0, INTER_LINEAR);

			// Write the frame with the detection boxes
			Mat detectedFrame;
			crop_frame.convertTo(detectedFrame, CV_8U);

			video.write(detectedFrame);

			//imshow("tester", crop_frame);

		}

		//destroyWindow("tester");
		cap.release();
		video.release();
		std::cout << "finished!" << endl;

	}
	else {
		std::cout << "No data on lane " << video_name << "Not making sub-video!" << endl;
	}

	return;
}

void SSAGUI::find_best_aspect(int& hight, int& width)
{
	hight = 0;
	width = 0;
	int max_hight = 0, min_hight = videoStream.get(CAP_PROP_FRAME_HEIGHT);;
	double avg_hight = 0, var_hight = 0;
	int max_width = 0, min_width = videoStream.get(CAP_PROP_FRAME_WIDTH);
	double avg_width = 0, var_width = 0;
	int holder_hight = 0, holder_width = 0;
	double n = double(resultsTrackingSingleSwimmer.size());
	int ii = 0;

	for (ii = 0; ii < int(n); ii++) {

		holder_hight = resultsTrackingSingleSwimmer[ii].height;
		holder_width = resultsTrackingSingleSwimmer[ii].width;
		if ((holder_hight > 0) && (holder_width > 0)) {
			//get max and min vals of hight and width 
			if (holder_hight > max_hight) max_hight = holder_hight;
			if (holder_width > max_width) max_width = holder_width;
			if (holder_width < min_width) min_width = holder_width;
			if (holder_hight < min_hight) min_hight = holder_hight;

			//get avg hight and width
			avg_hight += holder_hight;
			var_hight += pow(double(holder_hight), double(2));
			avg_width += holder_width;
			var_width += pow(double(holder_width), double(2));
		}
	}

	//calc avg and variance
	avg_hight /= n;
	avg_width /= n;
	//sample variance
	if (n != 1) {//NO zero div
		var_hight = (var_hight - pow(avg_hight, double(2)) * n) / (n - 1);
		var_width = (var_width - pow(avg_width, double(2)) * n) / (n - 1);
	}

	//Set the aspect raitio to the max hight and width
	width = max_width;
	hight = max_hight;

	//Set the aspect raitio to somthing that is not quite the max hight and width
	// yet to be attempted 

	//TODO try to use variance and width/height averages and see how that looks

}
*/
