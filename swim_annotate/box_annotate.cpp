#include "box_annotate.h"

box_annotate::box_annotate()
{
  all_data = nullptr;
  good_track = false;
  fast_ROI_mode = false;
  num_possible_data_lines = 0;
  current_class = 1;
  //set box
  current_box.height = 0;
  current_box.width = 0;
  current_box.x = 0;
  current_box.y = 0;
}


box_annotate::~box_annotate()
{
  if (all_data != nullptr) {
    int ii = 0;
    for (ii = 0; ii < num_possible_data_lines; ii++) {
      delete[] all_data[ii];
    }
    delete[] all_data;
    all_data = nullptr;
  }
}


bool box_annotate::load_video_for_boxing(string video_file)
{
  Mat frame;
  int ii = 0;//frame number
  int jj = 0;//lane number
  int kk = 0;//number type

  if (load_video(video_file)) {

    int n = get_hight();
    int m = get_width();

    ifstream get_from_header;
    ofstream write_to_header;
    string header_filename = video_file;

    //set class member fields
    num_possible_data_lines = get_num_frames() / get_skip_size();

    //init data holder
    all_data = new swim_data * [num_possible_data_lines]; //memory allocated, needs to be deleted (on line... )
    for (ii = 0; ii < num_possible_data_lines; ii++) {
      all_data[ii] = new swim_data[10];
      for (jj = 0; jj < 10; jj++) {//init to -1
        all_data[ii][jj].box_class = -1;
        all_data[ii][jj].swimmer_box.x = -1;
        all_data[ii][jj].swimmer_box.y = -1;
        all_data[ii][jj].swimmer_box.height = -1;
        all_data[ii][jj].swimmer_box.width = -1;
        all_data[ii][jj].lane_num = -1;
      }
    }

    //Change file name to end it .txt
    size_t pos = header_filename.find_first_of('.', 0);//no need to be carfull as the file was already opened
    header_filename.erase(pos + 1, 3);
    header_filename.append("txt");

    //Check if header already has been created
    //A data file will be mapped to its name, this data file will initially contain header with, 
    //video frame rate, video resolution, the frame skip size, and total number of frames in video. 
    get_from_header.open(header_filename);
    if (get_from_header.is_open()) {
      char line[500];
      string find;
      string num;
      string::size_type sz;

      get_from_header.getline(line, 500);
      if (line[0] != '#') {
        cout << "header missing initial # on frist line, file open failed!" << endl;
        return false;
      }
      get_from_header.getline(line, 500);//get FPS
      find = line;
      pos = find.find_first_of(' ', 0);
      num = find.substr(pos + 1, (find.size() - pos - 1));
      if (abs(stod(num) - get_FPS_vid()) > .01) { //to remove any rounding errors from the text file
        cout << "FPS dont match file, open failed" << endl;
        return false;
      }
      get_from_header.getline(line, 500);//get Hight and width
      find = line;
      pos = find.find_first_of(' ', 0);
      num = find.substr(pos + 1, (find.size() - pos - 1));
      if (stoi(num, &sz) != n) {
        cout << stoi(num, &sz) << endl;
        cout << "hight dose not match file, open failed" << endl;
        return false;
      }
      else if (stoi(num.substr(sz)) != m) {
        cout << stoi(num.substr(sz)) << endl;
        cout << "width dose not match file, open failed" << endl;
        return false;
      }
      get_from_header.getline(line, 500);//get #of frames 
      find = line;
      pos = find.find_first_of(' ', 0);
      num = find.substr(pos + 1, (find.size() - pos - 1));
      if (stoi(num) != get_num_frames()) {
        cout << stoi(num) << endl;
        cout << "number of frames dose not match file, open failed" << endl;
        return false;
      }

      //load data...
      get_from_header.getline(line, 500);//read in "#data"

      size_t pos_lane = 0;
      size_t pos_num = 0;
      size_t pos_num_check = 0;
      size_t pos_num_end = 0;
      int num_val = 0;

      for (ii = 0; ii < num_possible_data_lines; ii++) {//get #of frames 
        //frame loop
        if (get_from_header.getline(line, 500)) {//should hold all data at max
          find = line;
          pos_lane = 0;
          //start looking threw the frame
          for (jj = 0; jj < 10; jj++) {
            //lane loop
            pos_lane = find.find_first_of('{', pos_lane);//look for the next lane
            if (pos_lane != string::npos) {//if there is a lane
              //start looking through numbers for that lane
              pos_num_check = find.find_first_of('}', pos_lane);//check if there are 6 numbers
              if (pos_num_check == string::npos) {//if there is not an end to the other braket
                cout << "Braket error in frame " << ii << endl;
                return false;
              }
              pos_num = pos_lane;//pos_num holds the position of the current number in a lane
              pos_num_end = pos_lane;
              for (kk = 0; kk < 6; kk++) {
                //number loop
                pos_num_end = find.find_first_of(",", pos_num);//find end of number
                if ((pos_num_end > pos_num_check) && (kk < 5)) {//check if there are 6 numbers
                  cout << "Error in frame " << jj << " lane index " << pos_lane << ". not enough numbers." << endl;
                  return false;
                }
                else if (pos_num_end < pos_num_check && (kk == 5)) {//if in here then more than 6 number are in the lane 
                  cout << "Error in frame, too many numbers." << jj << endl;
                  return false;
                }
                else if (pos_num_end > pos_num_check) {//get the final number
                  pos_num_end = pos_num_check;
                }

                num_val = stoi(find.substr(pos_num + 1, pos_num_end - pos_num));//get the found number
                //cout << num_val << endl;
                //this is where the lane numbers are inserted into the sturcture
                if (kk == 0) all_data[ii][jj].swimmer_box.x = num_val;
                if (kk == 1) all_data[ii][jj].swimmer_box.y = num_val;
                if (kk == 2) all_data[ii][jj].swimmer_box.height = num_val;
                if (kk == 3) all_data[ii][jj].swimmer_box.width = num_val;
                if (kk == 4) all_data[ii][jj].box_class = num_val;
                if (kk == 5) all_data[ii][jj].lane_num = num_val;

                pos_num = pos_num_end + 1;//reset the pos_num to look for the next number
              }
            }
            else {//if there are no more lanes go to the next line
              break;
            }
            pos_lane = pos_num_check;
            //end of lane loop
          }
        }
        else {//if the video data is incomplete, stop looking for data that does not exist
          break;
        }
        //end of frame loop
      }
      get_from_header.close();
    }
    else {//Create new header 
      write_to_header.open(header_filename);
      write_to_header << "#Header, frames per second, video resolution(hight width), total number of frames in video." << endl;
      write_to_header << "FPS: " << get_FPS_vid() << endl;
      write_to_header << "RES: " << n << " " << m << endl;
      write_to_header << "TNF: " << get_num_frames() << endl;
      write_to_header << "#Data x, y, h, w, c, and l always in this order, for each lane. Sorted in order." << endl;
      write_to_header.close();
    }

    return true;
  }
  else {
    cout << "Supper annotator could not open video file\n";
    return false;
  }
}


//changes the current class lable for the box created
void box_annotate::change_class()
{
  char class_num = '0';
  int num = -1;
  bool done;
  Mat frame;

  //select lane number
  do {
    cout << "What class are we lableing? Options are..." << endl;
    cout << "on_block (1), diving (2), swimming (3), underwater (4), turning (5), finishing (6)" << endl;
    cout << "Class: ";

    //Get the key from the window
    frame = get_current_Mat();
    imshow(AN_WINDOW_NAME, frame);
    class_num = waitKey(0);
    cout << class_num;

    if (!isdigit(class_num)) {
      num = -1;
    }
    else {
      num = int(class_num) - 48;//convert to int
    }

    if ((num > 6) || (num < 1)) {
      cout << "\nAn invalid class number was selected" << endl;
      done = false;
    }
    else {
      current_class = num;
      done = true;
    }
  } while (!done);

  return;
}


//gets the next acction to be exicuted on the video data
bool box_annotate::annotation_options(char reply)
{
  cout << "\nOptions for annotation editing.\n\n";
  cout << "Change lane number of annotations, press (l)\n";
  cout << "Predict next frame and save current frame, press (w)\n";//change to right mouse button click
  cout << "Create ROI, press (r)\n";//change to left mouse button click
  cout << "Go back to last frame, press (a)\n";
  cout << "Go to next frame, press (d)\n";
  cout << "Move to any arbitrary frame, press (m)\n";
  cout << "Change annotation class, press (c)\n";
  cout << "Mark as absent, press (k)\n";//says the swimmer is not in the frame 
  cout << "Check for unfinished work, press (y)\n";//looks at each lane to see if a frame was skipped or a lane has not been done
  cout << "Togel fast ROI mode, press (t)\n";//Allows used to just select ROI continuesly
  cout << "Stop annotating video, press (esc)\n";
  //cout << "\nAnnotate F:"<<current_frame<<" L:"<<current_swimmer<< " c:" <<current_class<< "> ";

  switch (reply) {
  case 'l'://Change lane number of annotations
    select_lane_number();
    find_latest_annotation(true);
    reset_tracker();
    update_text_file();
    break;
  case 'w'://Predict next frame and save current frame, up arrow
    predict_next_frame();
    break;
  case 'r'://Create ROI
    while (create_ROI_in_pool(&current_box) && fast_ROI_mode) {//loop for faster ROI creation
      save_annotation();
      reset_tracker();
      if (get_current_frame() >= (get_num_frames() - 3)) {
        break;
      }
      cout << "ROI saved!" << endl;
      next_frame();
    }
    reset_tracker();
    cout << "ROI failed to save" << endl;
    break;
  case 'a'://Go back to last frame, left arrow
    last_frame();
    reset_tracker();
    break;
  case 'd'://Go to next frame, right arrow
    next_frame();
    reset_tracker();
    break;
  case 'm'://Move to any arbitrary frame
    go_to_frame();
    reset_tracker();
    break;
  case 'c'://Change annotation class
    change_class();
    reset_tracker();
    update_text_file();
    break;
  case 'k'://if swimmer is simply not in frame at all, remove ROI from frame and mark as absent
    mark_as_absent();
    next_frame();
    break;
  case 'y'://Check for unfinished work
    check_for_completion();
    break;
  case 't'://togel fast_ROI_mode
    fast_ROI_mode = !fast_ROI_mode;
    break;
  case 27://Stop annotating video
    if (quit_and_save_data()) {
      return false;//exit the annotator
    }
    break;
  default:
    cout << "An unrecognised value was input\n";
    break;
  }
  return true;
}


//saves the current_box rect object in the class to the all_data and the text file 
bool box_annotate::save_annotation()
{
  swim_data* lane_data;
  int current_swimmer = get_current_swimmer();

  lane_data = get_swim_data(int(get_current_frame() / get_skip_size()), current_swimmer);//checks for out of bounds errors

  if (lane_data) {
    //replace the data
    lane_data->box_class = current_class;
    lane_data->lane_num = current_swimmer;
    lane_data->swimmer_box = current_box;
    return true;
  }
  else {//if there was out of bound input
    return false;
  }

}


//loads data from all data into the current text file
bool box_annotate::update_text_file()
{
  ofstream update_header;
  string header_filename = get_video_file();
  char line[500];
  string header_data;
  double FPS_vid = get_FPS_vid();
  int n = get_hight();
  int m = get_width();
  int number_of_frames = get_num_frames();

  update_header.open("_app.txt");//temp file

  //make a new text file called _app.txt and then when created change the file names
  if (update_header.is_open()) {//if file opened start updating 
    int ii = 0;//frame number
    int jj = 0;//lane number
    int kk = 0;//lane contence

    swim_data* a_lane;
    update_header << "#Header, frames per second, video resolution(hight width), total number of frames in video." << endl;
    update_header << "FPS: " << FPS_vid << endl;
    update_header << "RES: " << n << " " << m << endl;
    update_header << "TNF: " << number_of_frames << endl;
    update_header << "#Data x, y, h, w, c, and l always in this order, for each lane. Sorted in order." << endl;

    for (ii = 0; ii < num_possible_data_lines; ii++) {//look at each frame 
      for (jj = 0; jj < 10; jj++) {//look at each lane
        a_lane = get_swim_data(ii, jj);
        if ((a_lane != nullptr) && (a_lane->lane_num == jj)) { //add the lane to the text file
          update_header << "{";
          for (kk = 0; kk < 6; kk++) {
            if (kk == 0) { update_header << a_lane->swimmer_box.x << ", "; }
            if (kk == 1) { update_header << a_lane->swimmer_box.y << ", "; }
            if (kk == 2) { update_header << a_lane->swimmer_box.height << ", "; }
            if (kk == 3) { update_header << a_lane->swimmer_box.width << ", "; }
            if (kk == 4) { update_header << a_lane->box_class << ", "; }
            if (kk == 5) { update_header << a_lane->lane_num << "}"; }
          }
        }
        //if last frame dont end line else end line
        if ((ii < (num_possible_data_lines - 1)) && (jj == 9)) update_header << endl;
      }
    }
    update_header.close();

    //rename the files
    //Change file name to end it .txt
    size_t pos = header_filename.find_first_of('.', 0);
    header_filename.erase(pos, 4);
    header_filename.append(".txt");
    remove("back_up.txt");//remove any file if exists
    if (rename(header_filename.c_str(), "back_up.txt") == 0) {//create a backup file
      if (rename("_app.txt", header_filename.c_str()) == 0) {
        return true;
      }
      else {
        perror("Error renaming file");
        return false;
      }
    }
    else {
      perror("Error renaming file");
      return false;
    }
  }
  else {//if file does not open
    return false;
  }
}


//Use the KCF algorithum to find swimmer in next frame
//Every new frame, update the referance frame
void box_annotate::predict_next_frame()
{
  //TrackerKCF
  Mat old_frame, frame;
  Rect2d new_current_box;
  bool results = false;

  if (!good_track) { //Recreate the tracker to account for changes in angle or body position
  //create the tracker for box prediction
    TrackerKCF::Params param;
    param.desc_pca = TrackerKCF::CN; // TrackerKCF::CN | TrackerKCF::GRAY
    param.desc_npca = 0;
    // param.compress_feature = true;
    // param.compressed_size = 2;

    tracker = TrackerKCF::create(param);

    frame = get_current_Mat();
    old_frame = frame.clone();

    if (!tracker->init(old_frame, Rect2d(current_box))) {
      cout << "Could not initalize tracker" << endl;
      return;
    }
    //tracker->setFeatureExtractor();//If you want to define a custom feature extractor 
    good_track = true;
  }

  next_frame();
  frame = get_current_Mat();
  tracker->update(frame, new_current_box);
  current_box = new_current_box;
  save_annotation();

  return;
}


//primary loop control for box_annotator class
bool box_annotate::display_current_frame()
{
  int blue = 0;
  int green = 0;
  int red = 0;
  int current_class_temp = 0;

  Mat frame;
  bool display_box = false;
  swim_data* lane;
  char reply_from_vid = '1';//set to select ROI

  int current_frame = get_current_frame();
  int skip_size = get_skip_size();
  int current_swimmer = get_current_swimmer();

  //Get the frame
  frame = get_current_Mat();

  //Update current box
  lane = get_swim_data(int(current_frame / skip_size), current_swimmer);//could return an empty lane!!
  if (lane != nullptr) {
    if (lane->lane_num == -1) {//if empty lane 
      display_box = false; //dont display box
    }
    else {
      display_box = true;//display box
      current_box = lane->swimmer_box;
      current_class_temp = lane->box_class;
    }
  }

  if (display_box) {

    switch (current_class_temp) {
    case 1://Brown
      blue = 0; red = 100; green = 50;
      break;
    case 2://red
      blue = 0; red = 255; green = 0;
      break;
    case 3://green
      blue = 0; red = 0; green = 255;
      break;
    case 4://orange
      blue = 0; red = 255; green = 127;
      break;
    case 5://yellow
      blue = 0; red = 255; green = 255;
      break;
    case 6://purpuel
      blue = 255; red = 128; green = 0;
      break;
    default:
      break;
    }

    rectangle(frame, current_box, Scalar(blue, green, red), 2, 1);// will create a box in the image frame
    imshow(AN_WINDOW_NAME, frame);
    for (int ii = 0; ii < 10; ii++) reply_from_vid = waitKey(1);//might clear window buffer
    cout << "\nAnnotate F:" << current_frame << " L:" << current_swimmer << " c:" << current_class << "> ";
    reply_from_vid = waitKey(0);
    cout << reply_from_vid << endl;
  }
  else {
    imshow("Annotating Window", frame);
    for (int ii = 0; ii < 10; ii++) reply_from_vid = waitKey(1);//might clear window buffer
    cout << "\nAnnotate F:" << current_frame << " L:" << current_swimmer << " c:" << current_class << "> ";
    reply_from_vid = waitKey(0);
    cout << reply_from_vid << endl;
  }
  if (annotation_options(reply_from_vid)) {
    return true;
  }
  return false;//exiting annotator
}


bool box_annotate::quit_and_save_data()
{
  //must destroy windows!!!
  char answer = 'n';
  bool keep_asking = true;
  Mat frame;

  do {
    cout << "Are you sure you are done? (y/n)" << endl;

    //Get the key from the window
    frame = get_current_Mat();
    imshow("Annotating Window", frame);
    answer = waitKey(0);
    cout << answer << endl;

    if (answer == 'n') {
      keep_asking = false;
      return false;
    }
    else if (answer == 'y') {
      keep_asking = false;

      //save data
      if (!update_text_file()) {
        cout << "could not save work!! look at text file for problem or data will be lost" << endl;
        return false;
      }
      quit_app();//tells supper annotator that the video object might not be valid anymore
      return true;
    }
    else {
      keep_asking = true;
    }
  } while (keep_asking);

  return false;
}


//return a pointer to the data in all_data
swim_data* box_annotate::get_swim_data(int frame_no, int lane_no)
{

  if ((frame_no > num_possible_data_lines - 1) || (frame_no < 0)) {// out of range frames
    cout << "Invalid frame number was requested from data" << endl;
    return nullptr;//empty_lane;
  }
  if ((lane_no > 9) || (lane_no < 0)) {//out of range lanes
    cout << "Invalid lane number was requested from data" << endl;
    return nullptr;//empty_lane;
  }

  //A reminder that frames are skipped every skip_size frames 
  swim_data* frame = all_data[frame_no];
  int ii = 0;
  for (ii = 0; ii < 10; ii++) {
    if (frame[ii].lane_num == lane_no) {//check if lane number exists
      return &frame[ii];//return the location of that lane
    }
    if (frame[ii].lane_num == -1) {//If no lane number exists then return pointer to first empty lane
      return &frame[ii];
    }
  }
  return nullptr;//empty_lane;
}


//sets up the app for start
void box_annotate::start_up() {
  select_lane_number();
  find_latest_annotation(true);
}


//finds the latest annoation for the current lane number
void box_annotate::find_latest_annotation(bool noise) {

  int ii = 0;//frame number
  swim_data* frame_of_lane;
  int number_of_frames = get_num_frames();
  int skip_size = get_skip_size();
  int current_swimmer = get_current_swimmer();
  int current_frame = get_current_frame();

  for (ii = 0; ii < int(number_of_frames / skip_size); ii++) {
    frame_of_lane = get_swim_data(ii, current_swimmer);
    if (frame_of_lane->lane_num == -1) {
      break;
    }
  }
  if ((ii < int(number_of_frames / skip_size)) && ii > 0) {//If ii 
    current_frame = (ii - 1) * skip_size;
    set_current_frame(current_frame);
  }
  else {
    if (noise) {
      cout << "All or no lanes have annotaions!" << endl;
    }
    current_frame = 0;
    set_current_frame(current_frame);
  }
}


//looks at each lane to see if there is a missed frame
//if the lane has not been started then it will also not show anything
void box_annotate::check_for_completion()
{
  bool incomplete_lanes[10] = { 1,1,1,1,1,1,1,1,1,1 };
  int ii = 0;
  int current_swimmer = get_current_swimmer();
  int current_frame = get_current_frame();
  int old_current_swimmer = current_swimmer;
  int old_current_frame = current_frame;

  for (ii = 0; ii < 10; ii++) {
    current_swimmer = ii;
    current_frame = 0;
    find_latest_annotation(false);
    if (current_frame == 0) {//If find_latest changes the frame number then the lane is incomplete 
      incomplete_lanes[ii] = false;
    }
    else {
      incomplete_lanes[ii] = true;//true if lane is complete or not started
    }
  }

  for (ii = 0; ii < 10; ii++) {
    if (incomplete_lanes[ii] == true) {
      cout << "Lane " << ii << " is incomplete" << endl;
    }
  }

  return;
}


//Tells the program that the swimmer is not in the frame
void box_annotate::mark_as_absent()
{
  swim_data* empty_frame;

  int current_swimmer = get_current_swimmer();
  int current_frame = get_current_frame();
  int skip_size = get_skip_size();

  empty_frame = get_swim_data(int(current_frame / skip_size), current_swimmer);

  if (empty_frame != nullptr) {
    empty_frame->box_class = current_class;
    empty_frame->lane_num = current_swimmer;
    empty_frame->swimmer_box.x = 0;
    empty_frame->swimmer_box.y = 0;
    empty_frame->swimmer_box.height = 0;
    empty_frame->swimmer_box.width = 0;
  }
  else {
    //out of bounds error
  }

  //reset the tracker
  reset_tracker();

}


//reset the tracker
void box_annotate::reset_tracker()
{
  //reset the tracker
  if (good_track) {
    tracker->clear();
    good_track = false;
  }
}


bool box_annotate::create_training_set(int* picture_num, bool update_text, bool update_JPEG)
{
  ofstream frame_data;
  Mat out_picture;
  int ii = 0;
  int jj = 0;
  int kk = 0;
  int number_pics = int(get_num_frames() / get_skip_size());
  float frame_hight = float(get_hight());
  float frame_width = float(get_width());
  float box_x = 0;//position of center of box in x
  float box_y = 0;//position of center of box in y
  float box_width = 0;
  float box_hight = 0;
  char num_str[] = "0000";//if a diget is added then sprintf(num_str, "%4.4u", *picture_num); needs to be changed
  string pic_path = "C:/Users/tim_w/Downloads/yolo_swim/JPEGImages/";
  string lab_path = "C:\\Users\\tim_w\\Downloads\\yolo_swim\\labels\\";

  typedef std::numeric_limits< float > fl;

  vector<int> compression_params;
  compression_params.push_back(IMWRITE_JPEG_QUALITY);
  compression_params.push_back(100);

  for (ii = 0; ii < number_pics; ii++) {

    sprintf(num_str, "%4.4u", *picture_num);

    if (update_text) {//save data
      frame_data.open(lab_path + string(num_str) + ".txt");
      if (frame_data.is_open()) {
        //<object-class-id> <center-x> <center-y> <width> <height>
        jj = 0;
        while (all_data[ii][jj].lane_num != -1) {
          if (all_data[ii][jj].swimmer_box.area() != 0) {//For situlation where in my application I noted that the swimmer was not visable
            //do convertion calculations
            box_width = float(all_data[ii][jj].swimmer_box.width - ((1 + all_data[ii][jj].swimmer_box.width) % 2));
            box_hight = float(all_data[ii][jj].swimmer_box.height - ((1 + all_data[ii][jj].swimmer_box.height) % 2));
            box_x = float(all_data[ii][jj].swimmer_box.x + floor(box_width / 2));
            box_y = float(all_data[ii][jj].swimmer_box.y + floor(box_hight / 2));

            for (kk = 0; kk < 5; kk++) {
              switch (kk)
              {
              case 0://<object-class-id>
                frame_data << (all_data[ii][jj].box_class - 1) << " ";//class must be zero indexed 
                break;
              case 1://center - x
                frame_data << setprecision(fl::digits) << box_x / frame_width << " ";
                break;
              case 2://center-y
                frame_data << setprecision(fl::digits) << box_y / frame_hight << " ";
                break;
              case 3://width
                frame_data << setprecision(fl::digits) << box_width / frame_width << " ";
                break;
              case 4://height
                frame_data << setprecision(fl::digits) << box_hight / frame_hight << endl;
                break;
              }
            }
            
          }
          jj++;
        }
        frame_data.close();
      }
      else {
        perror("Error");
        return false;
      }
    }

    if (update_JPEG) {//save image
      out_picture = get_current_Mat();
      if (!imwrite(pic_path + string(num_str) + ".jpg", out_picture, compression_params)) {
        perror("Error");
      }
    }
    
    //increment the name
    (*picture_num)++;
    next_frame();
  }
  return true;

}
