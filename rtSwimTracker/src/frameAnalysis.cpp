#include "frameAnalysis.h"


frameAnalysis::frameAnalysis()
{
	analyzeSwimmer = false;
	idSelectedSwimmer = -1;
	indexSelectedSwimmer = 0;
}

//TODO the below function needs to be tested - also it could interfere with analyzeVideo(Mat frame) due to overlap in use of private variables
void frameAnalysis::analyzeVideo(std::string videoToAnalyzeName)
{
	cv::VideoCapture videoToAnalyze;
	cv::Mat frame;
	fileFinder find;
	std::string resFileName = "PipeTest_SingleSwimmer.txt";
	std::string resFileAbsPath = "";
	std::ofstream resultsFile;
	resFileAbsPath = find.absolutePath(resFileName);
	std::vector <TrackingBox> resultsAllSwimmers;

	resultsFile.open(resFileAbsPath);

	if (!resultsFile.is_open())
	{
		std::cerr << "Error: can not create file " << resFileName << std::endl;
		return;
	}

	videoToAnalyze.open(videoToAnalyzeName);
	if (!videoToAnalyze.isOpened()) {
		std::cout << "Could not open video file for analysis" << std::endl;
		return;
	}

	videoToAnalyze >> frame; //TODO should we have this outside the while loop? since it happens again

	while (analyzeSwimmer && idSelectedSwimmer != -1) {
		videoToAnalyze >> frame;

		if (frame.empty()) {
			break;
		}

		resultsAllSwimmers = analyzeVideo(frame);

		for (auto tb : resultsAllSwimmers)
		{
			if (tb.get_m_boxID() == idSelectedSwimmer) {
				resultsFile << tb; //TODO do this in a more efficient way
			}
		}
	}

	resultsFile.close();
	return;
}

std::vector<TrackingBox> frameAnalysis::analyzeVideo(cv::Mat frameToAnalyze)
{
	std::vector<TrackingBox> resultsDetector;
	std::vector<TrackingBox> resultsSORT;

	//1. use detector on the frame
	detectSwimmersInVideo.configureDetector();
	resultsDetector = detectSwimmersInVideo.detectSwimmers(frameToAnalyze);
	//TODO do we need to reset m_frame in vector from detector?
	
	//2. use sort algorithm on the output of the detector
	resultsSORT = trackSORTprocessorInVideo.singleFrameSORT(resultsDetector);

	return resultsSORT;
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
			results[jj].set_m_frame(std::stoi(buff)); //TODO print result if fails?

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
		if (maxFrame < tb.get_m_frame())
			maxFrame = tb.get_m_frame();
	}


	for (int fi = 0; fi < maxFrame; fi++)
	{
		for (auto tb : detData)
			if (tb.get_m_frame() == fi + 1) // frame num starts from 1
				tempVec.push_back(tb);
		detFrameData.push_back(tempVec);
		tempVec.clear();
	}
	return maxFrame;
}

void frameAnalysis::setAnalyzeSwimmer(bool valSetTo)
{
	analyzeSwimmer = valSetTo;
}

bool frameAnalysis::setIDSelectedSwimmer(int valSetTo)
{
	if (valSetTo < -1) { return false; }
	idSelectedSwimmer = valSetTo;
	return true;
}

bool frameAnalysis::getAnalyzeSwimmer()
{
	return analyzeSwimmer;
}

int frameAnalysis::getIDSelectedSwimmer()
{
	return idSelectedSwimmer;
}

bool frameAnalysis::setindexSelectedSwimmer(int valSetTo)
{
	if (valSetTo < 0) { return false; } //TODO it should also compare to the size of the vector
	indexSelectedSwimmer = valSetTo;
	return true;
}

int frameAnalysis::getindexSelectedSwimmer()
{
	return indexSelectedSwimmer;
}

int frameAnalysis::findindexSelectedSwimmer(int idSwimmer, std::vector<TrackingBox> allSwimmers)
{
	for (int i = 0; i < allSwimmers.size(); i++) {
		if (allSwimmers[i].get_m_boxID() == idSwimmer)
			return i;
	}
	return -1; //TODO should we return -1 or 0?
}

void frameAnalysis::resizeBoxes(float scaleX, float scaleY, std::vector<TrackingBox>& dataToResize)
{
	for (int i = 0; i < dataToResize.size(); i++) {
		float x1New = scaleX * dataToResize[i].x;
		float y1New = scaleY * dataToResize[i].y;
		int newWidth = scaleX * dataToResize[i].width;
		int newHeight = scaleY * dataToResize[i].height;

		dataToResize[i].x = x1New;
		dataToResize[i].y = y1New;
		dataToResize[i].width = newWidth;
		dataToResize[i].height = newHeight;
	}
	return;
}


float frameAnalysis::findFrameScale(int newFrameSize, int currentFrameSize)
{
	if (newFrameSize == 0) { return -1; }
	float scaleDimension = static_cast<float>(newFrameSize) / static_cast<float>(currentFrameSize);
	return scaleDimension;
}

TrackingBox frameAnalysis::resizeBox(float scaleX, float scaleY, TrackingBox boxToResize)
{
	TrackingBox resizedBox(boxToResize.get_m_swimmerClass(), boxToResize.get_m_confScore(), boxToResize.get_m_frame(), boxToResize.get_m_boxID(), boxToResize);
	resizedBox.x = scaleX * boxToResize.x;
	resizedBox.y = scaleY * boxToResize.y;
	resizedBox.width = scaleX * boxToResize.width;
	resizedBox.height = scaleY * boxToResize.height;
	return resizedBox;
}

