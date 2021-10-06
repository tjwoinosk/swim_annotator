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

void frameAnalysis::sortOnFrame(string seqName)
{
	cout << "Processing " << seqName << "..." << endl;

	boost::timer::cpu_timer measureSORT;

	sortTrackerPiplelined SORTprocessor;

	vector<TrackingBox> detData;
	vector<vector<TrackingBox>> detFrameData;

	int maxFrame = 0;
	getDataFromDetectionFile(seqName, detData);
	maxFrame = groupingDetectionData(detData, detFrameData);


	// prepare result file.
	string resFileName = seqName;
	resFileName.replace(resFileName.end() - 4, resFileName.end(), "_det.txt");
	ofstream resultsFile;
	resultsFile.open(resFileName);

	if (!resultsFile.is_open())
	{
		cerr << "Error: can not create file " << resFileName << endl;
		return;
	}

	vector<TrackingBox> tempResults;
	tempResults.clear();
	
	measureSORT.start();
	for (int fi = 0; fi < maxFrame; fi++)
	{
		tempResults.clear();

		measureSORT.resume();
		tempResults = SORTprocessor.singleFrameSORT(detFrameData[fi]);
		measureSORT.stop();

		for (auto tb : tempResults)
		{
			resultsFile << tb;
		}
	}

	boost::timer::nanosecond_type divTerm = static_cast<boost::timer::nanosecond_type>(maxFrame);
	boost::timer::cpu_times res = measureSORT.elapsed();
	res.system /= divTerm;
	res.user /= divTerm;
	res.wall /= divTerm;

	cout << "Single Frame SORT speed (AVG): " << boost::timer::format(res) << endl;
	

	resultsFile.close();
}


/*
The purpose of this function is to read in the file whose name is speciied by the input detFileName
and put the information of this file into a vector of TrackingBoxes, which is the argument detData
*/
void frameAnalysis::getDataFromDetectionFile(string detFileName, vector<TrackingBox>& detData)
{
	string detLine;
	istringstream ss;
	ifstream detectionFile;

	detectionFile.open(detFileName);
	if (!detectionFile.is_open())
	{
		cerr << "Error: can not find file " << detFileName << endl;
		return;
	}

	TrackingBox tb;
	while (getline(detectionFile, detLine))
	{
		ss.str(detLine);
		ss >> tb;
		detData.push_back(tb);
	}
	detectionFile.close();

	return;
}


/*
This function takes an input detData that is all the detection data stored in a vector of TrackingBox
and then grouping TrackingBoxes for a single frame into a vector, and storing this vector into another
vector which is the input detFrameData.
The result is a 2D array like vector where each element points to a vector of TrackingBoxes of the same frame.
*/
int frameAnalysis::groupingDetectionData(vector<TrackingBox> detData, vector<vector<TrackingBox>>& detFrameData)
{
	int maxFrame = 0;
	vector<TrackingBox> tempVec;

	for (auto tb : detData) // find max frame number
	{
		if (maxFrame < tb.frame)
			maxFrame = tb.frame;
	}


	for (int fi = 0; fi < maxFrame; fi++)
	{
		for (auto tb : detData)
			if (tb.frame == fi + 1) // frame num starts from 1
				tempVec.push_back(tb);
		detFrameData.push_back(tempVec);
		tempVec.clear();
	}
	return maxFrame;
}