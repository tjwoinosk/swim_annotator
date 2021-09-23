#ifndef FRAMEANALYSIS_H
#define FRAMEANALYSIS_H

#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

class frameAnalysis
{
private:

	// global variables 

public:

	frameAnalysis();

	void analyzeVideo(VideoCapture videoToAnalyze);

};


#endif // !FRAMEANALYSIS_H
