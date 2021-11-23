#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>
//Add --detect_memory_leak=0 to debug command to remove memory leak output

#include "TrackingBox.h"
#include "fileFinder.h"
#include "SSAGUI.h"


BOOST_AUTO_TEST_SUITE(trackSingleSwimmerTestSuite)
BOOST_AUTO_TEST_CASE(testLaneTwoTracking)
{
    //For video 14.mp4 the coordinates (x, y) = (299, 325) 
    //correspond to the swimmer in the second lane
    //near the start of the video

    fileFinder find;
    std::string outputName = "detectionDataNEW.txt";
    std::string gtName;
    std::string line_gt, line_output;
    SSAGUI ssaVideo("14.mp4");
    frameAnalysis frameAnalysisObj;

    frameAnalysisObj.setAnalyzeSwimmer(false);
    frameAnalysisObj.setIDSelectedSwimmer(-1);
    frameAnalysisObj.setindexSelectedSwimmer(0);

    ssaVideo.setFrameAnalysis(frameAnalysisObj);

    outputName = find.absolutePath(outputName);
    gtName = outputName;
    gtName.replace(gtName.end() - 4, gtName.end(), "T1.txt");
    gtName.replace(gtName.end() - 4, gtName.end(), "GT.txt");

    ssaVideo.playVideoTest(false, 14, 20, 26, cv::Point_<float>(300, 275));

    std::ifstream ifs1(outputName);
    std::ifstream ifs2(gtName);

    while (std::getline(ifs1, line_output) && std::getline(ifs2, line_gt))
    {
        std::istringstream read_output(line_output);
        std::istringstream read_gt(line_gt);
        TrackingBox output_box;
        TrackingBox gt_box;

        output_box.inputFromFile(read_output);
        gt_box.inputFromFile(read_gt);

        //BOOST_TEST(output_box == gt_box); //TODO IDs and frame num will not match unless updated
        /*
        BOOST_CHECK_CLOSE(output_box.x, gt_box.x, 1.5); // 1.5 = 1.5%
        BOOST_CHECK_CLOSE(output_box.y, gt_box.y, 1.5);
        BOOST_CHECK_CLOSE(output_box.width, gt_box.width, 5.2);
        BOOST_CHECK_CLOSE(output_box.height, gt_box.height, 5.2);
        */
        BOOST_CHECK_CLOSE(output_box.x, gt_box.x, 5); // 1.5 = 1.5%
        BOOST_CHECK_CLOSE(output_box.y, gt_box.y, 5);
        BOOST_CHECK_CLOSE(output_box.width, gt_box.width, 10);
        BOOST_CHECK_CLOSE(output_box.height, gt_box.height, 10);
    }
}
BOOST_AUTO_TEST_CASE(testLaneEightTracking)
{
    //For video 14.mp4 the coordinates (x, y) = (659 , 83)
    //correspond to the swimmer in the second lane
    //near frame number 190

    fileFinder find;
    std::string outputName = "detectionDataNEW.txt";
    std::string gtName;
    std::string line_gt, line_output;
    SSAGUI ssaVideo("14.mp4");
    frameAnalysis frameAnalysisObj;

    frameAnalysisObj.setAnalyzeSwimmer(false);
    frameAnalysisObj.setIDSelectedSwimmer(-1);
    frameAnalysisObj.setindexSelectedSwimmer(0);

    ssaVideo.setFrameAnalysis(frameAnalysisObj);

    outputName = find.absolutePath(outputName);
    gtName = outputName;
    gtName.replace(gtName.end() - 4, gtName.end(), "T2.txt");
    gtName.replace(gtName.end() - 4, gtName.end(), "GT.txt");
    gtName.replace(gtName.end() - 4, gtName.end(), "AG.txt");

    //ssaVideo.playVideoTest(true, 14, 20, 26, cv::Point_<float>(300, 275));
    //TODO how do you test cancelled? cuz it won't output anything... should you read the file beforehand, and then see if nothign changed?
    ssaVideo.playVideoTest(false, 185, 190, 200, cv::Point_<float>(659, 83));

    std::ifstream ifs1(outputName);
    std::ifstream ifs2(gtName);

    while (std::getline(ifs1, line_output) && std::getline(ifs2, line_gt))
    {
        std::istringstream read_output(line_output);
        std::istringstream read_gt(line_gt);
        TrackingBox output_box;
        TrackingBox gt_box;

        output_box.inputFromFile(read_output);
        gt_box.inputFromFile(read_gt);

        //BOOST_TEST(output_box == gt_box); //TODO IDs and frame num will not match unless updated
        /*
        BOOST_CHECK_CLOSE(output_box.x, gt_box.x, 1.5); // 1.5 = 1.5%
        BOOST_CHECK_CLOSE(output_box.y, gt_box.y, 1.6);
        BOOST_CHECK_CLOSE(output_box.width, gt_box.width, 5.2);
        BOOST_CHECK_CLOSE(output_box.height, gt_box.height, 5.2);
        */

        BOOST_CHECK_CLOSE(output_box.x, gt_box.x, 5); // 1.5 = 1.5%
        BOOST_CHECK_CLOSE(output_box.y, gt_box.y, 5);
        BOOST_CHECK_CLOSE(output_box.width, gt_box.width, 10);
        BOOST_CHECK_CLOSE(output_box.height, gt_box.height, 10);
    }
}
BOOST_AUTO_TEST_CASE(testLaneEightTrackingCancellation)
{
    //For video 14.mp4 the coordinates (x, y) = (659 , 83)
    //correspond to the swimmer in the second lane
    //near frame number 190

    fileFinder find;
    std::string outputName = "detectionDataNEW.txt";
    std::string gtName;
    std::string line_gt, line_output;
    SSAGUI ssaVideo("14.mp4");
    SSAGUI ssaVideoTwo("14.mp4");
    frameAnalysis frameAnalysisObj;
    frameAnalysis frameAnalysisObjTwo;

    ssaVideo.setFrameAnalysis(frameAnalysisObj);
    ssaVideoTwo.setFrameAnalysis(frameAnalysisObjTwo);

    outputName = find.absolutePath(outputName);
    gtName = outputName;
    gtName.replace(gtName.end() - 4, gtName.end(), "T2.txt");
    gtName.replace(gtName.end() - 4, gtName.end(), "GT.txt");

    ssaVideo.playVideoTest(false, 185, 190, 200, cv::Point_<float>(659, 83));
    ssaVideoTwo.playVideoTest(true, 14, 20, 26, cv::Point_<float>(300, 275)); //This should make no changes to the output file

    std::ifstream ifs1(outputName);
    std::ifstream ifs2(gtName);

    //The test is that the due to the order of the tests, the output should remain as 
    //that of test "testLaneEightTracking" even though we run "testLaneTwoTracking" with cancellation
    while (std::getline(ifs1, line_output) && std::getline(ifs2, line_gt))
    {
        std::istringstream read_output(line_output);
        std::istringstream read_gt(line_gt);
        TrackingBox output_box;
        TrackingBox gt_box;

        output_box.inputFromFile(read_output);
        gt_box.inputFromFile(read_gt);

        //BOOST_TEST(output_box == gt_box); //TODO IDs and frame num will not match unless updated
        /*
        BOOST_CHECK_CLOSE(output_box.x, gt_box.x, 1.5); // 1.5 = 1.5%
        BOOST_CHECK_CLOSE(output_box.y, gt_box.y, 1.6);
        BOOST_CHECK_CLOSE(output_box.width, gt_box.width, 5.2);
        BOOST_CHECK_CLOSE(output_box.height, gt_box.height, 5.2);
        */
        BOOST_CHECK_CLOSE(output_box.x, gt_box.x, 3); // 1.5 = 1.5%
        BOOST_CHECK_CLOSE(output_box.y, gt_box.y, 3);
        BOOST_CHECK_CLOSE(output_box.width, gt_box.width, 6);
        BOOST_CHECK_CLOSE(output_box.height, gt_box.height, 6);
    }
}
BOOST_AUTO_TEST_CASE(testLaneEightTrackingCancellationTestTwo)
{
    //For video 14.mp4 the coordinates (x, y) = (659 , 83)
    //correspond to the swimmer in the second lane
    //near frame number 190

    fileFinder find;
    std::string outputName = "detectionDataNEW.txt";
    std::string gtName;
    std::string line_gt, line_output;
    SSAGUI ssaVideo("14.mp4");
    SSAGUI ssaVideoTwo("14.mp4");
    frameAnalysis frameAnalysisObj;
    frameAnalysis frameAnalysisObjTwo;

    ssaVideo.setFrameAnalysis(frameAnalysisObj);
    ssaVideoTwo.setFrameAnalysis(frameAnalysisObjTwo);

    outputName = find.absolutePath(outputName);
    gtName = outputName;
    gtName.replace(gtName.end() - 4, gtName.end(), "T2.txt");
    gtName.replace(gtName.end() - 4, gtName.end(), "GT.txt");

    ssaVideoTwo.playVideoTest(true, 14, 20, 26, cv::Point_<float>(300, 275)); //This should make no changes to the output file
    ssaVideo.playVideoTest(false, 185, 190, 200, cv::Point_<float>(659, 83));

    std::ifstream ifs1(outputName);
    std::ifstream ifs2(gtName);

    //The test is that the due to the order of the tests, the output should remain as 
    //that of test "testLaneEightTracking" even though we run "testLaneTwoTracking" with cancellation
    //The cancellation happens first to ensure that after re-selection the correct data is recorded
    while (std::getline(ifs1, line_output) && std::getline(ifs2, line_gt))
    {
        std::istringstream read_output(line_output);
        std::istringstream read_gt(line_gt);
        TrackingBox output_box;
        TrackingBox gt_box;

        output_box.inputFromFile(read_output);
        gt_box.inputFromFile(read_gt);

        //BOOST_TEST(output_box == gt_box); //TODO IDs and frame num will not match unless updated
        /*
        BOOST_CHECK_CLOSE(output_box.x, gt_box.x, 1.5); // 1.5 = 1.5%
        BOOST_CHECK_CLOSE(output_box.y, gt_box.y, 1.6);
        BOOST_CHECK_CLOSE(output_box.width, gt_box.width, 5.2);
        BOOST_CHECK_CLOSE(output_box.height, gt_box.height, 5.2);
        */
        BOOST_CHECK_CLOSE(output_box.x, gt_box.x, 3); // 1.5 = 1.5%
        BOOST_CHECK_CLOSE(output_box.y, gt_box.y, 3);
        BOOST_CHECK_CLOSE(output_box.width, gt_box.width, 6);
        BOOST_CHECK_CLOSE(output_box.height, gt_box.height, 6);
    }
}
BOOST_AUTO_TEST_SUITE_END() //End output validation tests suite

BOOST_AUTO_TEST_SUITE(objectCenteringTestSuite)
BOOST_AUTO_TEST_CASE(testWithinTolerance) {
    SSAGUI ssaVideo("14.mp4");
    frameAnalysis frameAnalysisObj;

    ssaVideo.setFrameAnalysis(frameAnalysisObj);
    ssaVideo.setToleranceX(0.25);
    ssaVideo.setToleranceY(0.25);
    ssaVideo.playVideoTest(false, 130, 134, 140, cv::Point_<float>(300, 275));
    //Lane two - 0.25, 0.25 means within; 0.15 = Y means outside Y

    fileFinder find;
    std::string outputName = "objectCentering.txt";
    std::string gtName;
    outputName = find.absolutePath(outputName);
    gtName = outputName;
    gtName.replace(gtName.end() - 4, gtName.end(), "GT1.txt");
    //results file

    std::ifstream ifs1(outputName);
    std::ifstream ifs2(gtName);

    std::istream_iterator<char> b1(ifs1), e1;
    std::istream_iterator<char> b2(ifs2), e2;

    // compare 
    BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}
BOOST_AUTO_TEST_CASE(testOutsideTolerance) {
    SSAGUI ssaVideo("14.mp4");
    frameAnalysis frameAnalysisObj;

    ssaVideo.setFrameAnalysis(frameAnalysisObj);
    ssaVideo.setToleranceX(0.15);
    ssaVideo.setToleranceY(0.15);
    ssaVideo.playVideoTest(false, 130, 134, 140, cv::Point_<float>(655, 10)); //Lane eight

    fileFinder find;
    std::string outputName = "objectCentering.txt";
    std::string gtName;
    outputName = find.absolutePath(outputName);
    gtName = outputName;
    gtName.replace(gtName.end() - 4, gtName.end(), "GT2.txt");
    //results file

    std::ifstream ifs1(outputName);
    std::ifstream ifs2(gtName);

    std::istream_iterator<char> b1(ifs1), e1;
    std::istream_iterator<char> b2(ifs2), e2;

    // compare 
    BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}
BOOST_AUTO_TEST_SUITE_END() //End objectCentering tests suite
