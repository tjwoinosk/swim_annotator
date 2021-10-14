#include "frameAnalysis.h"


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

	videoToAnalyze.open(videoToAnalyzeName);
	if (!videoToAnalyze.isOpened()) {
		std::cout << "Could not open video file for analysis" << std::endl;
		return;
	}

	videoToAnalyze >> frame;

	while (true) { //TODO do we want it to be infinite?

		videoToAnalyze >> frame;
		// Stop the program if reached end of video
		if (frame.empty()) {
			std::cout << "Done processing !!!" << std::endl;
			break;
		}

		//call detector and SORT here
		// 
		//TODO test code:
		std::cout << "processing frame" << std::endl;
		imshow("test", frame);
		cv::waitKey(30);
		//TODO end test code
	}

	//TODO test code:
	std::cout << "Done processing" << std::endl;
	//TODO end test code

	return;

}

void frameAnalysis::analyzeVideo(cv::Mat frameToAnalyze)
{
	swimmerDetector detect;
	//std::vector<DetectionBox> resultsDetector;
	std::vector<TrackingBox> resultsDetector;

	sortTrackerPiplelined SORTprocessor;
	//std::vector<DetectionBox> resultsSORT;
	std::vector<TrackingBox> resultsSORT;

	//1. use detector on the frame
	detect.configureDetector();
	resultsDetector = detect.detectSwimmers(frameToAnalyze);

	//2. use sort algorithm on the output of the detector
	//resultsSORT = SORTprocessor.singleFrameSORT(resultsDetector);

	//TODO do something with output to test it - output detector and sort information, and show video
}


std::string frameAnalysis::sortOnFrame(SpeedReporter* report)
{
	sortTrackerPiplelined SORTprocessor;

	std::vector<TrackingBox> detData;
	std::vector<std::vector<TrackingBox>> detFrameData;

	fileFinder find;
	std::string resFileName = "PipeTest.txt";
	std::string resFileAbsPath = "";
	std::ofstream resultsFile;
	boost::timer::cpu_timer measureSORT;

	resFileAbsPath = find.absolutePath(resFileName);

	int maxFrame = 0;
	getDataFromDetectionFile(resFileAbsPath, detData);
	maxFrame = groupingDetectionData(detData, detFrameData);

	// prepare result file.
	resFileAbsPath.replace(resFileAbsPath.end() - 4, resFileAbsPath.end(), "_det.txt");
	resultsFile.open(resFileAbsPath);

	if (!resultsFile.is_open())
	{
		std::cerr << "Error: can not create file " << resFileName << std::endl;
		return resFileAbsPath;
	}

	std::vector<TrackingBox> tempResults;
	tempResults.clear();
	measureSORT.start();
	measureSORT.stop();
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

	if ((report != NULL) && !report->isBase())
	{
		report->inputMeasurement(measureSORT.elapsed(), maxFrame);
		report->reportSpeed();
	}

	resultsFile.close();
	
	return resFileAbsPath;
}


//std::string frameAnalysis::sortOnFrameDet()
//{
//	sortTrackerPiplelined SORTprocessor;
//
//	std::vector<TrackingBox> detData;
//	std::vector<std::vector<TrackingBox>> detFrameData;
//
//	fileFinder find;
//	std::string resFileName = "PipeTestDB.txt";
//	std::string resFileAbsPath = "";
//	std::ofstream resultsFile;
//
//	resFileAbsPath = find.absolutePath(resFileName);
//
//	int maxFrame = 0;
//	getDataFromDetectionFile(resFileAbsPath, detData);
//	maxFrame = groupingDetectionData(detData, detFrameData);
//
//	// prepare result file.
//	resFileAbsPath.replace(resFileAbsPath.end() - 4, resFileAbsPath.end(), "_det.txt");
//	resultsFile.open(resFileAbsPath);
//
//	if (!resultsFile.is_open())
//	{
//		std::cerr << "Error: can not create file " << resFileName << std::endl;
//		return resFileAbsPath;
//	}
//
//	std::vector<TrackingBox> tempResults;
//	std::vector<DetectionBox> inputDataDet;
//	tempResults.clear();
//	inputDataDet.clear();
//
//	for (int fi = 0; fi < maxFrame; fi++)
//	{
//		tempResults.clear();
//		inputDataDet.clear();
//
//		for (int i = 0; i < detFrameData[fi].size(); i++) {
//			//Converting to DetectionBox type to test the singleFrameSORT with input of vector<DetectionBox>
//			DetectionBox temp;
//			temp.m_frame = detFrameData[fi][i].m_frame;
//			temp.m_boxID = detFrameData[fi][i].m_boxID;
//			temp.updateBox(cv::Rect2f(detFrameData[fi][i].x, detFrameData[fi][i].y, detFrameData[fi][i].width, detFrameData[fi][i].height)); //TODO idk if its right
//			inputDataDet.push_back(temp);
//		}
//
//		tempResults = SORTprocessor.singleFrameSORT(inputDataDet);
//
//		for (auto tb : tempResults)
//		{
//			resultsFile << tb;
//		}
//	}
//
//	resultsFile.close();
//
//	return resFileAbsPath;
//}

std::string frameAnalysis::runDetectorOnFrames(SpeedReporter* report)
{
	fileFinder find;
	std::string resFileName = "detectionData.txt";
	std::string resFileAbsPath = "";
	std::ofstream resultsFile;
	boost::timer::cpu_timer measure;
	int numberFrames = 0;

	try
	{
		resFileAbsPath = find.returnDataLocation() + resFileName;
		resultsFile.open(resFileAbsPath);
	}
	catch (const std::exception& e)
	{
		std::cout << "Could not open " << resFileAbsPath << std::endl << e.what() << std::endl;
		return std::string();
	}

	std::string imgPath = "";
	int possibleNumImages = 100;
	const __int64 buffSize = 5;
	char buff[buffSize]{};
	cv::Mat img;
	swimmerDetector detect;
	//std::vector<DetectionBox> results;
	std::vector<TrackingBox> results;

	detect.configureDetector();

	measure.start();
	measure.stop();
	for (int ii = 0; ii < possibleNumImages; ii++)
	{
		sprintf_s(buff, buffSize, "%04i", ii);
		imgPath.assign(buff);
		imgPath = find.absolutePath(imgPath + ".bmp");

		if (imgPath.empty())
			continue;

		results.clear();
		img = cv::imread(imgPath);

		numberFrames++;
		measure.resume();
		results = detect.detectSwimmers(img);
		measure.stop();

		for (int jj = 0; jj < results.size(); jj++)
		{
			//TODO check this new code/method works fine for adjusting the frame number
			results[jj].m_frame = std::stoi(buff);

			//resultsFile << results[jj];
			results[jj].outputToFileDetection(resultsFile);

		}
		resultsFile << std::endl;
	}

	if ((report != NULL) && !report->isBase())
	{
		report->inputMeasurement(measure.elapsed(), numberFrames);
		report->reportSpeed();
	}

	resultsFile.close();
	return resFileAbsPath;
}

/*
The purpose of this function is to read in the file whose name is specified by the input detFileName
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

