#ifndef SUB_VIDEO_CREATOR_H
#define SUB_VIDEO_CREATOR_H

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include "frameAnalysis.h"

using namespace cv;
using namespace std;

class subVideoCreator
{
	//TODO add any references needed

public:

	subVideoCreator(string videoFile);
	void make_video();
	void setFrameAnalysis(frameAnalysis& frameObj);

	void startVideo();
	void closeVideo();
	VideoCapture& getVideoStream();
	int isVideoStreamValid();

private:
	frameAnalysis* frameAnalysisObj_ptr;
	VideoCapture videoStream;
	string videoFileName;

	void find_best_aspect(int& hight, int& width);
	bool frameAnalysisValid();
};

#endif // !SUB_VIDEO_CREATOR_H

