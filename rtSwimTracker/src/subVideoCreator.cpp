#include "subVideoCreator.h"

/*
References: [8], [9], [10], [11], [12], [13]
*/

//TODO what file format to accept and save in?

subVideoCreator::subVideoCreator(string videoFile)
{
	frameAnalysisObj_ptr = NULL;
	videoFileName = videoFile;
	//videoStream.open(videoFile);
}

void subVideoCreator::make_video()
{
	string str, outputFile;
	VideoWriter video;
	Mat frame, crop_frame;
	const int final_aspect = 150;
	int hight = 0, width = 0;
	int v_hight = 0, v_width = 0, m_hight = 0, m_width = 0;
	int x = 0, y = 0, box_w = 0, box_h = 0;
	int ii = 0;

	startVideo();

	if (!frameAnalysisValid()) {
		std::cout << " ERROR: Unable to create video. FrameAnalysis obj not set." << std::endl;
		return;
	}

	std::vector<TrackingBox> resultsTrackingSingleSwimmer = frameAnalysisObj_ptr->getSingleSwimmerResults();
	
	if (resultsTrackingSingleSwimmer.size() == 0) {
		std::cout << " ERROR: Unable to create video. No data on any swimmers." << std::endl;
		closeVideo();
		return;
	}

	bool data_is_valid = true;

	for (int i = 0; i < resultsTrackingSingleSwimmer.size(); i++) {
		if (resultsTrackingSingleSwimmer[ii].x < 0) {
			data_is_valid = false;
		}
	}
	//end 

	if (data_is_valid) {
		// Open the video file
		std::cout << "Opening... " << videoFileName << endl;
		if (!isVideoStreamValid()) {
			std::cout << "Issues opening " << videoFileName << endl;
		}

		str = videoFileName;
		str.replace(str.end() - 4, str.end(), "_sub_video.mp4"); //TODO avi or mp4?
		outputFile = str;

		//Decided best hight and width to take from the origanl video
		find_best_aspect(hight, width);

		// Get the video writer initialized to save the output video
		//video.open(outputFile, VideoWriter::fourcc('M', 'J', 'P', 'G'), 28, Size(final_aspect, final_aspect));
		video.open(outputFile, VideoWriter::fourcc('m', 'p', '4', 'v'), 28, Size(final_aspect, final_aspect));
		//TODO is the above okay? and is there a better way? what format do we want?
		//TDO the above is from  https://stackoverflow.com/questions/61807359/write-frame-into-mp4-format-using-opencv2-and-c

		//get starting values
		getVideoStream() >> frame;
		Mat frame_buf(frame.rows + hight * 2, frame.cols + width * 2, frame.depth());

		//namedWindow("tester", WINDOW_NORMAL);

		for (ii = 0; ii < resultsTrackingSingleSwimmer.size(); ii++) {

			// get frame from the video
			if (resultsTrackingSingleSwimmer[ii].get_m_frame() != getVideoStream().get(CAP_PROP_POS_FRAMES)) {
				getVideoStream().set(CAP_PROP_POS_FRAMES, resultsTrackingSingleSwimmer[ii].get_m_frame()); //might make this very slow
			}
			getVideoStream() >> frame;

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
		closeVideo();
		video.release();
		std::cout << "finished!" << endl;
	}
	else {
		std::cout << "No data on lane " << videoFileName << "Not making sub-video!" << endl;
	}

	return;
}

void subVideoCreator::find_best_aspect(int& hight, int& width)
{
	std::vector<TrackingBox> resultsTrackingSingleSwimmer = frameAnalysisObj_ptr->getSingleSwimmerResults();
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


void subVideoCreator::setFrameAnalysis(frameAnalysis& frameObj)
{
	frameAnalysisObj_ptr = &frameObj;
	//TODO check for memory leaks
}

void subVideoCreator::startVideo()
{
	videoStream.open(videoFileName);
}

void subVideoCreator::closeVideo()
{
	getVideoStream().release();
}

VideoCapture& subVideoCreator::getVideoStream()
{
	return videoStream;
}

int subVideoCreator::isVideoStreamValid()
{
	if (!getVideoStream().isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return 0;
	}
	else {
		return 1;
	}
}

bool subVideoCreator::frameAnalysisValid()
{
	if (frameAnalysisObj_ptr == NULL)
		return false; //TODO is this enough?
	return true;
}

