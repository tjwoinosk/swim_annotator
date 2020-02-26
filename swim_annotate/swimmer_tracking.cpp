#include "swimmer_tracking.h"
#include <random>
#include <chrono>
#include <math.h>
#include <algorithm>// std::sort

#include "sort_tracker.h"//For the iouMatrix fuction
#include "HungarianAlgorithm.h"

using namespace std;

bool res_sort_rule(track_data i, track_data j) { return (i.frame_num < j.frame_num); }

swimmer_tracking::swimmer_tracking()
{

}


//!! need to acount for when swimmer gets occulded
//Do tracking using annotations
//Fill the vector call results with structs representing objects for each swimmer in each frame
//This fuction will try to ignore swimmers who are not in the race even when
// they were annotated. The racer should be in vector position 0.
bool swimmer_tracking::annotation_tracking(string file_name)
{
  int ii;
  int jj;
  int kk;

  //for predictions
  int hight_step = 0;
  int width_step = 0;
  int x_step = 0;
  int y_step = 0;

  track_data temp{0,0,Rect(0,0,0,0),0.0,0};
  vector<swim_data>* frame_data;
  vector<swim_data>* next_frame_data;

  if (load_video_for_boxing(file_name)) {
    destroyWindow(AN_WINDOW_NAME);

    int skip_size = get_skip_size();
    int num_frames = get_num_frames();

    //Create objects
    for (ii = 0; ii < int(num_frames / skip_size); ii++) {

      //Each swimmer, ie. lane number
      for (jj = 0; jj < 10; jj++) {
        
        //ii is a psudo frame number used for boxing and a skip size
        frame_data = get_swim_data(ii, jj);
        next_frame_data = get_swim_data(ii + 1, jj);
        //Calcualtions for box predictions
        //predictions use linear aproximation between annotated frame befor and after
        if ((next_frame_data != nullptr) && (frame_data != nullptr)) {
          //if ((next_frame_data->at(0).swimmer_box.area() != 0) && (frame_data->at(0).swimmer_box.area() != 0)) {
          hight_step = next_frame_data->at(0).swimmer_box.height - frame_data->at(0).swimmer_box.height;
          width_step = next_frame_data->at(0).swimmer_box.width - frame_data->at(0).swimmer_box.width;
          x_step = next_frame_data->at(0).swimmer_box.x - frame_data->at(0).swimmer_box.x;
          y_step = next_frame_data->at(0).swimmer_box.y - frame_data->at(0).swimmer_box.y;

          hight_step /= (skip_size + 1);
          width_step /= (skip_size + 1);
          x_step /= (skip_size + 1);
          y_step /= (skip_size + 1);
        }

        //To account for frame skipping
        for (kk = 0; kk < skip_size; kk++) {
          if (kk == 0) {
            //When swimmer position does not have to be infered
            if (frame_data != nullptr) {
              temp.frame_num = ii*skip_size;//must traslate frame number to acctual frame
              temp.object_ID = jj;
              temp.class_id = frame_data->at(0).box_class;//swimmer class, see fuction header for the use of index 0
              temp.conf_score = 100;
              temp.frame_pos = frame_data->at(0).swimmer_box;
              //Add to vector
              results.push_back(temp);
            }
          }
          else {
            //When swimmer position must be predicted
            if (((ii * skip_size + kk) < num_frames) && (next_frame_data != nullptr)) {
              temp.frame_num = ii * skip_size + kk;
              //temp.object_ID = jj;
              //temp.class_id = frame_data->at(0).box_class;
              //temp.conf_score = 100;
              temp.frame_pos.height += hight_step;
              temp.frame_pos.width += width_step;
              temp.frame_pos.x += x_step;
              temp.frame_pos.y += y_step;
              results.push_back(temp);
            }
            else if((ii * skip_size + kk) < num_frames && (next_frame_data == nullptr)) {
              //Case when there are still frames but no next frame annotations to predict with
              temp.conf_score = 50;
              temp.frame_num = ii * skip_size + kk;
              results.push_back(temp);
            }
            else {
              //push back nothing
              //results.push_back(temp)
            }
          }
        }
      }
    }
  }
  else {
    cout << "Could not load video file for annotation tracking!" << endl;
    return false;
  }


  return true;
}


//Use SORT algorithum
//Reads the contence of the detection file and saves the tracked
//data into the classes reuslts var
//Needs work as currently the same swimmer get tracked multiple times
//Requires an output file to read 
void swimmer_tracking::sort_tracking(string text_file_name)
{
  sort_tracker track_swimmers;
  track_data hold_data;
  int ii = 0;
  string temp_str;
  char ch;
  float tpx, tpy, tpw, tph;

  string detection_file = text_file_name;
  detection_file.replace(detection_file.end() - 4, detection_file.end(), "_detection_data.txt");
  track_swimmers.TestSORT(detection_file, .05);

 
  detection_file = ".\\output\\" + detection_file;
  fstream results_file(detection_file, std::fstream::in);
  istringstream ss;

  if (!results_file.is_open()) {
    cout << "Could not open sort tracking results file" << endl;
    return;
  }
  
  results.clear();

  while (getline(results_file, temp_str)) {
    
    ss.str(temp_str);
    ss >> hold_data.frame_num >> ch;
    ss >> hold_data.object_ID >> ch;
    ss >> tpx >> ch;
    ss >> tpy >> ch;
    ss >> tpw >> ch;
    ss >> tph >> ch;
    hold_data.conf_score = -1;

    hold_data.frame_pos = Rect(int(tpx),int(tpy),int(tpw),int(tph));

    results.push_back(hold_data);

  }

  //Ensure the results are in order by frame 
  //sort(results.begin(), results.end(), res_sort_rule);

  return;
}


//Used for testing and evaluating the tracking algorithum
 //Will produce video of all tracked swimmers results
void swimmer_tracking::show_video_of_tracking(string file_name)
{

  // Open a video file or an image file or a camera stream.
  struct box_color {
    int blue;
    int red;
    int green;
  };

  string str, outputFile;
  VideoCapture cap;
  VideoWriter video;
  Mat frame;
  int frame_num = 0;
  int number_objects = 0;
  int red = 0, green = 0, blue = 0;
  int ii = 0;

  vector<box_color> box_colors;
  box_color temp_color;
  unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
  minstd_rand0 g1(seed1);

  str = file_name;

  // Open the video file
  cap.open(str);
  str.replace(str.end() - 4, str.end(), "_show_tracking_results.avi");
  outputFile = str;
    
 
  // Get the video writer initialized to save the output video
  video.open(outputFile, VideoWriter::fourcc('M', 'J', 'P', 'G'), 28, Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)));

  //Count the number of objects found when tracking
  for (ii = 0; ii < results.size(); ii++) {
    if (results[ii].object_ID > number_objects) number_objects = results[ii].object_ID;
  }
  //randomly assinge colors to the objects 
  for (ii = 0; ii < number_objects; ii++) {
    temp_color.blue = (g1() % (128 + 64)) + 32;
    temp_color.red = (g1() % (128 + 64)) + 32;
    temp_color.green = (g1() % (128 + 64)) + 32;
    box_colors.push_back(temp_color);
  }

  // Create a window
  static const string kWinName = "Tracking Testing Results Window";
  namedWindow(kWinName, WINDOW_NORMAL);

  //Set the starting frame to be analized 
  cap.set(CAP_PROP_POS_FRAMES, 0);


  //Process frames.
  while (waitKey(1) < 0)
  {
    // get frame from the video
    cap >> frame;

    // Stop the program if reached end of video
    if (frame.empty()) {
      cout << "Output file is stored as " << outputFile << endl;
      waitKey(3000);
      break;
    }

    frame_num = (cap.get(CAP_PROP_POS_FRAMES));//(cap.get(CAP_PROP_FRAME_COUNT));
    for (ii = 0; ii < results.size(); ii++) {
      if (results[ii].frame_num == frame_num) {
        temp_color = box_colors[results[ii].object_ID - 1];
        blue = temp_color.blue; 
        green = temp_color.green; 
        red = temp_color.red; 
        if (results[ii].object_ID == 20) {//isolate an object ID
          blue = 0;
          green = 0;
          red = 0;
        }
        rectangle(frame, results[ii].frame_pos, Scalar(blue, green, red), 2, 2);
      }
    }

    // Write the frame with the detection boxes
    Mat detectedFrame;
    frame.convertTo(detectedFrame, CV_8U);
    video.write(detectedFrame);
    imshow(kWinName, frame);


    //Ends work early for debuging
    /*
    int stop = 5;
    if ((frame_num % stop) == (stop - 1)) {
      break;
    }
    //*/
  }

  cap.release();
  video.release();
  destroyWindow(kWinName);

  return;

}


//Find the cov mats for kalman filter from ground truth
//Mat 1 is the cov of the prosess noise denoted as Q
//Mat 2 is the cov of the observation noise denote
Mat_<float> swimmer_tracking::calculate_proc_noise_covs()
{
  string ground = ".\\data\\ground\\all_vids.mp4";
  string temp;
  int ii = 0, jj = 0, kk = 0, ff = 0;
  const int state_num = 7;//number of variables
  Matrix<float, state_num, state_num> cov_mat;
  Matrix<float, Dynamic, 1> ones_vec;
  Matrix<float, Dynamic, state_num> eig_data;
  float t = float(1) / float(30);
  int num_lanes = 10;
  vector<swim_data>* frame_data;
  swim_data holder, last_frame;
  bool occultion = false;
  float iou = 0;
  float tol = 1e-2;
  const float init = 0;

  Mat_<float> final(state_num, state_num, init);

  //Check if ground file is there
  temp = ground;
  temp.replace(temp.end() - 4, temp.end(), ".txt");
  fstream checker(temp);
  
  if (!checker.is_open()) {
    cout << "\nGround not open!" << endl;
    cov_mat.setZero();
    return final;
  }

  if (load_video_for_boxing(ground)) {
    destroyWindow(AN_WINDOW_NAME);
    //results should now contain ground
    int num_frames = get_num_frames()/get_skip_size() + 1;
    t = t * float(get_skip_size());
    
    //initilize data
    vector<vector<float>> data;
    vector<vector<float>> accel;
    vector<float> data_point(state_num,0);
    cov_mat.setZero();

    //init last_frame
    frame_data = get_swim_data(0, 0);
    last_frame.swimmer_box = frame_data->at(0).swimmer_box;
    last_frame.lane_num = frame_data->at(0).lane_num;
    last_frame.box_class = frame_data->at(0).box_class;

    //loop threw each tracked swimmer and fill data with posisions
    //if a end of video occurs, calculate accelerations on data and save them
    //if an occultion occurs, calculate accelerations on data and save them
    //clear data and start again
    for (ii = 0; ii < num_lanes; ii++) {
      for (jj = 0; jj < num_frames; jj++) {

        frame_data = get_swim_data(jj, ii);
      
        if (frame_data != nullptr) {
          holder = frame_data->at(0);//use the frist swimmer in the lane, they are the swimmer racing
          occultion = false;

          //look for sene change
          iou = float((holder.swimmer_box | last_frame.swimmer_box).area());
          if (iou == 0) {
            occultion = true;
          }
          else {
            iou = float((holder.swimmer_box & last_frame.swimmer_box).area()) / iou;
            if (iou < tol) occultion = true;
          }
          
          for (kk = 0; kk < state_num; kk++) {
            switch (kk)
            {
            case 0: //u
              data_point[kk] = float(holder.swimmer_box.x);
              break;
            case 1: //v
              data_point[kk] = float(holder.swimmer_box.y);
              break;
            case 2: //s
              data_point[kk] = float(holder.swimmer_box.area());
              if (holder.swimmer_box.area() < 2) {//occultion occured
                occultion = true;
              }
              break;
            case 3: //r
              data_point[kk] = float(holder.swimmer_box.width) / float(holder.swimmer_box.height);
              break;
            case 4://u_dot
              data_point[kk] = float(holder.swimmer_box.x);
              break;
            case 5://v_dot
              data_point[kk] = float(holder.swimmer_box.y);
              break;
            case 6://s_dot
              data_point[kk] = float(holder.swimmer_box.area());
              break;
            default:
              break;
            }
          }
          if (occultion) {//If occultion occurs
            //calculate accellerations
            calc_accelerations(data, state_num, t);
            //save data filled with accellerations to accel
            for (ff = 0; ff < data.size(); ff++) accel.push_back(data[ff]);
            //clear data
            data.clear();
            last_frame = holder;
          }
          else {
            data.push_back(data_point);
            last_frame = holder;
          }
        }
      }
      //When the swimmer in the lane has its last frame
      data.push_back(data_point);
      calc_accelerations(data, state_num, t);
      for (ff = 0; ff < data.size(); ff++) accel.push_back(data[ff]);
      data.clear();
    }

    eig_data.resize(accel.size(), NoChange);

    for (ii = 0; ii < accel.size(); ii++) {
      for (jj = 0; jj < state_num; jj++) {
        eig_data(ii, jj) = accel[ii][jj];
      }
    }

    //sanity check for the covmat
    /*
    vector<float> tester(accel.size(), 0);
    long float test_mean = 0, test_var = 0;
    const int vec_pos = 2;
    tester.resize(accel.size(), NoChange);

    for (ii = 0; ii < accel.size(); ii++) {
      test_mean += accel[ii][vec_pos];
    }
    test_mean = test_mean / float(accel.size());
    for (ii = 0; ii < accel.size(); ii++) {
      test_var += pow((accel[ii][vec_pos] - test_mean), 2);
    }
    test_var =  test_var / float(accel.size());
    cout << "Checking Variance... frist go" << endl;
    cout << test_var << endl;
    //*/

    //calculate covariant matrix of process Q
    ones_vec.resize(accel.size());
    ones_vec.setOnes();
    eig_data = eig_data - ((ones_vec * (ones_vec.transpose() * eig_data)) / float(accel.size()));
    if (accel.size() > 1) {
      cov_mat = (eig_data.transpose() * eig_data) / (float(accel.size()) - 1);
    }
    else {
      cout << "Could not calculate cov mat Q for kalman filter, div by zero" << endl;
    }
  }
  //cout << cov_mat << endl;

  for (ii = 0; ii < state_num; ii++) {
    for (jj = 0; jj < state_num; jj++) {
      final.at<float>(ii, jj) = cov_mat(ii, jj)/float(get_skip_size());
    }
  }
 
  return final;
  
}


//Run detection on ground truth calculate error in
// detection vs ground truth
//The cov of the observation noise denoted as R
Mat_<float> swimmer_tracking::calculate_obser_noise()
{
  string ground = ".\\data\\ground\\all_vids.mp4";
  const int mesure_num = 4;
  string str;
  track_data hold_data;
  int ii = 0, jj = 0, kk = 0;
  string temp_str;
  char ch;
  float tpx, tpy, tpw, tph;
  vector<track_data> detect_data;
  vector<track_data> ground_frame;
  vector<track_data> detect_frame;
  Mat_<float> final(mesure_num, mesure_num, float(0));
  char resp = '0', temp = '0';
  vector<vector<double>> iouMatrix;
  vector<int> assignment;
  unsigned int trkNum = 0;
  unsigned int detNum = 0;
  sort_tracker func_needed;
  vector<swim_data>* frame_data;
  track_data temp_swimmer;

  vector<float> cov_rows(4, 0);
  vector<vector<float>> cov_data;

  str = ground;
  str.replace(str.end() - 4, str.end(), "_detection_data.txt");

  //Save time, ask if user would like to update detection file
  fstream results_file(str, std::fstream::in);
  if (results_file.is_open()) {
    do
    {
      cout << "Would you like to update the detection file? (y/n)" << endl;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      cin >> temp;
      if ((temp == 'n') || (temp == 'y')) {
        resp = temp;
      }
    } while (resp == '0');
    if (resp == 'y') make_detection_file(ground);
  }
  else {
    make_detection_file(ground);
    results_file.open(str, std::fstream::in);
  }

  //read in detection file
  istringstream ss;
  if (!results_file.is_open()) {
    cout << "Could not open detection results file" << endl;
    return final;
  }

  while (getline(results_file, temp_str)) {
    ss.str(temp_str);
    ss >> hold_data.frame_num >> ch;
    ss >> hold_data.object_ID >> ch;
    ss >> tpx >> ch;
    ss >> tpy >> ch;
    ss >> tpw >> ch;
    ss >> tph >> ch;
    hold_data.conf_score = -1;
    
    hold_data.frame_pos = Rect(int(tpx), int(tpy), int(tpw), int(tph));

    detect_data.push_back(hold_data);
  }


  //Get ground truth data
  if (!load_video_for_boxing(ground)) {
    cout << "Could not open ground truth data for detection analysis" << endl;
    return final;
  }
  int skip_size = get_skip_size();
  int num_frames = get_num_frames();

  /*for testing... three things to comment
  int blue = 0, green = 0, red = 0;
  Mat test_image;
  int match_cnt = 0;
  VideoCapture cap(ground);
  if (!cap.isOpened()) {
    cout << "Could not open video capture object for testing observation mat" << endl;
    return final;
  }
  // Create a window
  static const string kWinName = "Tracking Testing Results Window";
  namedWindow(kWinName, WINDOW_NORMAL);
  //*/

  //loop threw the annotated frames
  for (ii = 0; ii < num_frames / skip_size; ii++) {
    
    ground_frame.clear();
    //loop threw the annotated lanes to collect them in the frame
    for (jj = 0; jj < 10; jj++) {
      frame_data = get_swim_data(ii, jj);
      if ((frame_data != nullptr) && (frame_data->at(0).swimmer_box.area() > 2)) {
        
        temp_swimmer.frame_pos = frame_data->at(0).swimmer_box;//use the swimmer racing in the pool
        temp_swimmer.frame_num = ii;
        ground_frame.push_back(temp_swimmer);
      }
    }
    
    //loop threw the detections in this frame to collect them
    //remember that the annotated frame is different then the actual frame number
    // this is becuase frames are skipped when annotating
    detect_frame.clear();
    vector<track_data>::iterator location = detect_data.begin();
    while ((location != detect_data.end()) && (location->frame_num <= (ii*skip_size))) {
      if (location->frame_num == (ii * skip_size)) {
        detect_frame.push_back(*location);
      }
      location++;
    }

    //Associate detections to tracked object (both represented as bounding boxes)
    // for this frame
    trkNum = ground_frame.size();
    detNum = detect_frame.size();
    iouMatrix.clear();
    iouMatrix.resize(trkNum, vector<double>(detNum, 0));

    for (kk = 0; kk < trkNum; kk++) // compute iou matrix as a distance matrix
    {
      for (jj = 0; jj < detNum; jj++)
      {
        // use 1-iou because the hungarian algorithm computes a minimum-cost assignment.
        iouMatrix[kk][jj] = 1 - func_needed.GetIOU(ground_frame[kk].frame_pos, detect_frame[jj].frame_pos);
      }
    }

    // solve the assignment problem using hungarian algorithm.
    // the resulting assignment is [track(prediction) : detection], with len=preNum
    HungarianAlgorithm HungAlgo;
    assignment.clear();

    //Assignment is a vector of int where in assignment[i] is the mapping from a
    // ground_frame item to a detected_frame item
    //The size of assignment is the number of ground_frame items
    // if a ground frame has no assignment it is skipped
    HungAlgo.Solve(iouMatrix, assignment);
    float holder = 0;

    //Fill cov_data for the observation matrix 
    for (jj = 0; jj < trkNum; jj++) {
      if ((assignment[jj] < 0) || (assignment[jj] >= detNum)) {
        continue;
      }
      if (1 - iouMatrix[jj][assignment[jj]] < 0.001) {//something small
        continue;
      }
      cov_rows[0] = float(ground_frame[jj].frame_pos.x - detect_frame[assignment[jj]].frame_pos.x);
      cov_rows[1] = float(ground_frame[jj].frame_pos.y - detect_frame[assignment[jj]].frame_pos.y);
      cov_rows[2] = float(ground_frame[jj].frame_pos.area() - detect_frame[assignment[jj]].frame_pos.area());
      holder = float(ground_frame[jj].frame_pos.width) / float(ground_frame[jj].frame_pos.height);
      cov_rows[3] =  holder - float(detect_frame[assignment[jj]].frame_pos.width) / float(detect_frame[assignment[jj]].frame_pos.height);
      cov_data.push_back(cov_rows);
    }

    /*for testing...
    int bigger_num;
    if (waitKey(70) >= 0) {
      break;
    }
    cap.set(CAP_PROP_POS_FRAMES, ii * skip_size);
    cap >> test_image;
    //Get all the boxes
    if (detNum < trkNum) bigger_num = trkNum;
    else bigger_num = detNum;
    match_cnt = 0;
    char corn_text[20];
    for (jj = 0; jj < bigger_num; jj++) {
      blue = 0; green = 0; red = 0;
      //put in all boxes
      if(trkNum > jj) rectangle(test_image, ground_frame[jj].frame_pos, Scalar(blue, green, red), 1, 1);
      if(detNum > jj) rectangle(test_image, detect_frame[jj].frame_pos, Scalar(blue, green, red), 1, 1);

      //Put boxes that have been matched on frame 
      if (jj < trkNum) {//range of assignments
        //put in matched boxes
        blue = 50; green = 10; red = 200;
        if ((assignment[jj] < 0) || (assignment[jj] >= detNum)) {
          continue;
        }
        if (1 - iouMatrix[jj][assignment[jj]] < 0.001) {//something small
          continue;
        }
        sprintf(corn_text, "Match number %d", match_cnt);
        rectangle(test_image, ground_frame[jj].frame_pos, Scalar(blue, green, red), 2, 1);
        //putText(test_image,string(corn_text), Point(ground_frame[jj].frame_pos.x+2, ground_frame[jj].frame_pos.y+2), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 1);
        rectangle(test_image, detect_frame[assignment[jj]].frame_pos, Scalar(blue, green, red), 2, 1);
        //putText(test_image, string(corn_text), Point(detect_frame[assignment[jj]].frame_pos.x + 2, detect_frame[assignment[jj]].frame_pos.y + 2), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 1);
        match_cnt++; 
      }
    }
    imshow(kWinName, test_image);
    //*/

  }

  /*for testing
  cap.release();
  destroyWindow(kWinName);
  //*/
  
  //calc covariance mat
  Matrix<float, mesure_num, mesure_num> cov_mat;
  Matrix<float, Dynamic, 1> ones_vec;
  Matrix<float, Dynamic, mesure_num> eig_data;
  
  eig_data.resize(cov_data.size(), NoChange);

  for (ii = 0; ii < cov_data.size(); ii++) {
    for (jj = 0; jj < mesure_num; jj++) {
      eig_data(ii, jj) = cov_data[ii][jj];
    }
  }


  //cout << cov_data.size() << endl;
  //cout << eig_data << endl << endl;

  //calculate covariant matrix of detection R
  ones_vec.resize(cov_data.size());
  ones_vec.setOnes();
  eig_data = eig_data - ((ones_vec * (ones_vec.transpose() * eig_data)) / float(cov_data.size()));
  if (cov_data.size() > 1) {
    cov_mat = (eig_data.transpose() * eig_data) / (float(cov_data.size()) - 1);
  }
  else {
    cout << "Could not calculate cov mat R for kalman filter, div by zero" << endl;
  }

  //cout << cov_mat << endl;

  for (ii = 0; ii < mesure_num; ii++) {
    for (jj = 0; jj < mesure_num; jj++) {
      final.at<float>(ii, jj) = cov_mat(ii, jj);
    }
  }

  return final;
}


//Take data and replace all values with accellerations
//this is used in the noise covs fuction
//state_num, number of values in the vector of data to find the covariance mat of
//t, finite difference value for calculating acceleration
void swimmer_tracking::calc_accelerations(vector<vector<float>>& data, int state_num, float t)
{
  vector<float> data_point(state_num, 0);
  vector<vector<float>> accel;

  int ii = 0, jj = 0;
  if (data.size() > 2) {
    //Get the acceleration for each point
    //fill eigan Matrix for calculating covariance
    for (ii = 0; ii < data.size(); ii++) {

      if (ii == 0) {//forward difference
        for (jj = 0; jj < state_num; jj++) {
          data_point[jj] = (data[ii + 2][jj] - 2 * (data[ii + 1][jj]) + data[ii][jj]) / 2;
          if (jj >= 4) {
            data_point[jj] = data_point[jj] * 2 / t;
          }
        }
      }
      else if (ii == (data.size() - 1)) {//backward difference 
        for (jj = 0; jj < state_num; jj++) {
          data_point[jj] = (data[ii][jj] - 2 * (data[ii - 1][jj]) + data[ii - 2][jj]) / 2;
          if (jj >= 4) {
            data_point[jj] = data_point[jj] * 2 / t;
          }
        }
      }
      else {//Central difference
        for (jj = 0; jj < state_num; jj++) {
          data_point[jj] = (data[ii - 1][jj] - 2 * (data[ii][jj]) + data[ii + 1][jj]) / 2;
          if (jj >= 4) {
            data_point[jj] = data_point[jj] * 2 / t;
          }
        }
      }

      //save data point
      accel.push_back(data_point);
    }
    //change data
    data = accel;
  }
  else {//if data.size() > 2
    data.clear();
  }

}


//Makes a detection file for tracking algorithums to work with
//file contains detection information on each frame
//see MOT2016 challenge for detection file details
void swimmer_tracking::make_detection_file(string file_name)
{
  // Load names of classes
  string classesFile = "classes.names";
  ifstream ifs(classesFile.c_str());
  string line;
  vector<string> classes;

  while (getline(ifs, line)) classes.push_back(line);
  int iou_val_user = 0;

  // Give the configuration and weight files for the model
  String modelConfiguration = "yolov3.cfg";
  String modelWeights = "yolov3.weights";

  // Load the network
  Net net = readNetFromDarknet(modelConfiguration, modelWeights);
  net.setPreferableBackend(DNN_BACKEND_OPENCV);
  net.setPreferableTarget(DNN_TARGET_CPU);

  // Open a video file or an image file or a camera stream.
  string str;
  VideoCapture cap;
  VideoWriter video;
  Mat frame, blob;
  int frame_num = 1;

  str = file_name;
  cap.open(str);
  str.replace(str.end() - 4, str.end(), "_detection_data.txt");

  int finaly = cap.get(CAP_PROP_FRAME_COUNT) / 10;

  //Set the starting frame to be analized 
  //cap.set(CAP_PROP_POS_FRAMES, 0);

  //Process frames.
  while (waitKey(1) < 0)
  {
    // get frame from the video
    cap >> frame;

    // Stop the program if reached end of video
    if (frame.empty()) {
      cout << "Done creating detection file !!!" << endl;
      cout << "Output file is stored as " << str << endl;
      waitKey(3000);
      break;
    }

    // Create a 4D blob from a frame.
    blobFromImage(frame, blob, 1 / 255.0, Size(inpWidth, inpHeight), Scalar(0, 0, 0), true, false);

    //Sets the input to the network
    net.setInput(blob);

    // Runs the forward pass to get output of the output layers
    vector<Mat> outs;
    net.forward(outs, getOutputsNames(net));


    // Remove the bounding boxes with low confidence
    // Also saves the results in results
    postprocess(frame, outs, frame_num);

    if ((frame_num % finaly) == 0) {
      cout << "Another 10% completed... " << endl;
    }

    //Ends work early for debuging
    /*
    if ((frame_num % 10) == 9) {
      break;
    }
    //*/
    frame_num++;
  }

  cap.release();

  //Save results in text file...
  save_results_in_text_file(str);

  return;

}


//Save the contents of results into a text file with input name
//Used by the make detection file fuction to save the contence of results into text file name
void swimmer_tracking::save_results_in_text_file(string text_file_name)
{

  //file line ex...
  //1, -1, 794.2, 47.5, 71.2, 174.8, 67.5, -1, -1
  //frame#, Object_ID, box_x, box_y, width, hight, detection_confidence, -1, -1
  string str = ".\\data\\";
  int ii = 0;
  str.append(text_file_name);

  fstream results_file(str, std::fstream::out);
  if (!results_file.is_open()) {
    results_file.open(text_file_name, std::fstream::out);
  }

  if (results_file.is_open()) {
    for (ii = 0; ii < results.size(); ii++) {
      results_file << results[ii].frame_num << ","
        << "-1,"
        << results[ii].frame_pos.x << ","
        << results[ii].frame_pos.y << ","
        << results[ii].frame_pos.width << ","
        << results[ii].frame_pos.height << ","
        << results[ii].conf_score << ","
        << results[ii].class_id << ","
        << "-1,"
        << "-1"
        << endl;
    }
  }
  else {
    cout << "Could not open results text file!" << endl;
  }

  results_file.close();
}


//Get the names of the output layers
//This fuction is used in the make detection file system
vector<String> swimmer_tracking::getOutputsNames(const Net& net)
{
  static vector<String> names;
  if (names.empty())
  {
    //Get the indices of the output layers, i.e. the layers with unconnected outputs
    vector<int> outLayers = net.getUnconnectedOutLayers();

    //get the names of all the layers in the network
    vector<String> layersNames = net.getLayerNames();

    // Get the names of the output layers in names
    names.resize(outLayers.size());
    for (size_t i = 0; i < outLayers.size(); ++i)
      names[i] = layersNames[outLayers[i] - 1];
  }
  return names;
}


//Remove the bounding boxes with low confidence using non-maxima suppression
//saves the detection results into the classes reuslts var
//This fuction is used in the maek detection file system 
void swimmer_tracking::postprocess(Mat& frame, const vector<Mat>& outs, int frame_num) {
  vector<int> classIds;
  vector<float> confidences;
  vector<Rect> boxes;

  for (size_t i = 0; i < outs.size(); ++i)
  {
    // Scan through all the bounding boxes output from the network and keep only the
    // ones with high confidence scores. Assign the box's class label as the class
    // with the highest score for the box.
    float* data = (float*)outs[i].data;
    for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
    {
      Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
      Point classIdPoint;
      double confidence;
      // Get the value and location of the maximum score
      minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
      if (confidence > confThreshold)
      {
        int centerX = (int)(data[0] * frame.cols);
        int centerY = (int)(data[1] * frame.rows);
        int width = (int)(data[2] * frame.cols);
        int height = (int)(data[3] * frame.rows);
        int left = centerX - width / 2;
        int top = centerY - height / 2;

        classIds.push_back(classIdPoint.x);
        confidences.push_back((float)confidence);
        boxes.push_back(Rect(left, top, width, height));
      }
    }
  }

  // Perform non maximum suppression to eliminate redundant overlapping boxes with
  // lower confidences
  vector<int> indices;
  track_data temp;

  NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
  //clear the containers for network preformance
  for (size_t i = 0; i < indices.size(); ++i)
  {
    int idx = indices[i];
    Rect box = boxes[idx];

    //save results for later processing
    temp.frame_pos = box;
    temp.class_id = classIds[idx];
    temp.conf_score = confidences[idx];
    temp.frame_num = frame_num;
    temp.object_ID = -1;
    results.push_back(temp);
  }
}