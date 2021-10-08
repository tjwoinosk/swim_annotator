#include "frameAnalysis.h"
#include "swimmerDetector.h"
#include "DetectionBox.h"

frameAnalysis::frameAnalysis()
{
}

void frameAnalysis::analyzeVideo(std::string videoToAnalyzeName)
{
	//Read video object frame by frame - keep reading until the end

	//Call detector on the single frame
	//Call SORT on the output of the detector on the single frame

	//Use the object SORT returns to write to a file output
	cv::VideoCapture videoToAnalyze;
	cv::Mat frame;
	swimmerDetector detectSwimmerObj;

	videoToAnalyze.open(videoToAnalyzeName);
	if (!videoToAnalyze.isOpened()) {
		std::cout << "Could not open video file for analysis" << std::endl;
		return;
	}
	
	videoToAnalyze >> frame;

	detectSwimmerObj.configureDetector();

	while (true) { //TODO do we want it to be infinite?

		videoToAnalyze >> frame;
		// Stop the program if reached end of video
		if (frame.empty()) {
			std::cout << "Done processing !!!" << std::endl;
			break;
		}

		//call detector and SORT here
		DetectionBox currentFrameDetections = detectSwimmerObj.detectSwimmers(frame);

		////TODO test code:
		//std::cout << "processing frame" << std::endl;
		//imshow("test", frame);
		//cv::waitKey(30);
		////TODO end test code


	}

	//TODO test code:
	std::cout << "Done processing" << std::endl;
	//TODO end test code

	return;

}

void frameAnalysis::detectionOnImage(std::string imageName)
{
	std::cout << "STARTING THIS FUNCTION" << std::endl;

	cv::Mat frameImage = cv::imread(imageName);
	swimmerDetector detectSwimmerObj;

	detectSwimmerObj.configureDetector();
	DetectionBox currentFrameDetections = detectSwimmerObj.detectSwimmers(frameImage);

	std::cout << "DONE THIS FUNCTION " << currentFrameDetections.size() << std::endl;
}

void frameAnalysis::sortOnFrame(std::string seqName)
{
	std::cout << "Processing " << seqName << "..." << std::endl;

	sortTrackerPiplelined SORTprocessor;

	std::vector<TrackingBox> detData;
	std::vector<std::vector<TrackingBox>> detFrameData;

	int maxFrame = 0;
	getDataFromDetectionFile(seqName, detData);
	maxFrame = groupingDetectionData(detData, detFrameData);


	// prepare result file.
	std::string resFileName = seqName;
	resFileName.replace(resFileName.end() - 4, resFileName.end(), "_det.txt");
	std::ofstream resultsFile;
	resultsFile.open(resFileName);

	if (!resultsFile.is_open())
	{
		std::cerr << "Error: can not create file " << resFileName << std::endl;
		return;
	}

	std::vector<TrackingBox> tempResults;
	tempResults.clear();
	
	for (int fi = 0; fi < maxFrame; fi++)
	{
		tempResults.clear();

		tempResults = SORTprocessor.singleFrameSORT(detFrameData[fi]);

		for (auto tb : tempResults)
		{
			resultsFile << tb;
		}
	}

	resultsFile.close();
}


/*
The purpose of this function is to read in the file whose name is speciied by the input detFileName
and put the information of this file into a vector of TrackingBoxes, which is the argument detData
*/
void frameAnalysis::getDataFromDetectionFile(std::string detFileName, std::vector<TrackingBox>& detData)
{
	std::string detLine;
	std::istringstream ss;
	std::ifstream detectionFile;

	detectionFile.open(detFileName);
	if (!detectionFile.is_open())
	{
		std::cerr << "Error: can not find file " << detFileName << std::endl;
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
int frameAnalysis::groupingDetectionData(std::vector<TrackingBox> detData, std::vector<std::vector<TrackingBox>>& detFrameData)
{
	int maxFrame = 0;
	std::vector<TrackingBox> tempVec;

	for (auto tb : detData) // find max frame number
	{
		if (maxFrame < tb.m_frame)
			maxFrame = tb.m_frame;
	}


	for (int fi = 0; fi < maxFrame; fi++)
	{
		for (auto tb : detData)
			if (tb.m_frame == fi + 1) // frame num starts from 1
				tempVec.push_back(tb);
		detFrameData.push_back(tempVec);
		tempVec.clear();
	}
	return maxFrame;
}