#include "swimmer_tracking.h"
#include <random>
#include <chrono>

using namespace std;

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

  fstream results_file(str,std::fstream::out);

  if (results_file.is_open()) {
    for (ii = 0; ii < results.size(); ii++) {
      results_file << results[ii].frame_num << ", "
        << "-1, "
        << results[ii].frame_pos.x << ", "
        << results[ii].frame_pos.y << ", "
        << results[ii].frame_pos.width << ", "
        << results[ii].frame_pos.height << ", "
        << results[ii].conf_score << ", "
        << results[ii].class_id << ", "
        << "-1, "
        << "-1"
        << endl;
    }
  }
  else {
    cout << "Could not open results text file!" << endl;
  }

  results_file.close();
}


//Use SORT algorithum
//Reads the contence of the detection file and saves the tracked
//data into the classes reuslts var
//Needs work as currently the same swimmer get tracked multiple times
void swimmer_tracking::sort_tracking(string text_file_name)
{
  sort_tracker track_swimmers;
  track_data hold_data;
  int ii = 0;
  string temp_str;
  track_swimmers.TestSORT(text_file_name, .05);
  char ch;
  float tpx, tpy, tpw, tph;

  fstream results_file(".\\output\\"+text_file_name, std::fstream::in);
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

  //Need to edit results file so to link objects when fragmentation occures
  //If no detections exist for three entire frames then there is a scene change
  //Give each scene its own data
  //When an object disapears 

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

  for (ii = 0; ii < state_num; ii++) {
    for (jj = 0; jj < state_num; jj++) {
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