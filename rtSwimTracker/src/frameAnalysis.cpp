#include "frameAnalysis.h"

frameAnalysis::frameAnalysis()
{
}

void frameAnalysis::analyzeVideo(string videoToAnalyzeName)
{
	//Read video object frame by frame - keep reading until the end

	//Call detector on the single frame
	//Call SORT on the output of the detector on the single frame

	//Use the object SORT returns to write to a file output
	VideoCapture videoToAnalyze;
	Mat frame;

	videoToAnalyze.open(videoToAnalyzeName);
	if (!videoToAnalyze.isOpened()) {
		cout << "Could not open video file for analysis" << endl;
		return;
	}
	
	videoToAnalyze >> frame;

	while (true) { //TODO do we want it to be infinite?

		videoToAnalyze >> frame;
		// Stop the program if reached end of video
		if (frame.empty()) {
			cout << "Done processing !!!" << endl;
			break;
		}

		//call detector and SORT here
		// 
		//TODO test code:
		cout << "processing frame" << endl;
		imshow("test", frame);
		waitKey(30);
		//TODO end test code
	}

	//TODO test code:
	cout << "Done processing" << endl;
	//TODO end test code

	return;

}
