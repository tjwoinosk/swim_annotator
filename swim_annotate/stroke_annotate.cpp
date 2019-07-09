#include "stroke_annotate.h"

//compare fuction for sorting list
bool cmp(const swim_data_stroke& a, const swim_data_stroke& b) {
  if (a.cycle < b.cycle) return true;
  return false;
}


stroke_annotate::stroke_annotate()
{
  set_skip_size(1);
  video_speed = 32;
  max_speed = 1;
  min_speed = 500;
  current_class = 1;
}


bool stroke_annotate::load_video_for_stroke_counting(string video_file)
{
  if (load_video(video_file)) {

    ifstream read_strokes;
    ofstream create_stokes;
    size_t pos = 0;
    string header_filename = video_file;
    int number_frames = get_num_frames();
    int n = get_hight();
    int m = get_width();
    double FPS = get_FPS_vid();
    int ii = 0;

    //create stroke file name
    pos = header_filename.find_first_of('.', 0);
    header_filename.erase(pos, 4);
    header_filename.append("_str.txt");


    read_strokes.open(header_filename);//try to open file 
    if (read_strokes.is_open()) {//video has been anotated before read in data
      char line[500];
      string find;
      string num;
      string::size_type sz;

      for (ii = 0; ii < 6; ii++) {
        read_strokes.getline(line, 500);
        if (line[0] != '#') {
          cout << "Missing initial # on line " << ii+1 << " , file open failed!" << endl;
          return false;
        }
      }
      
      read_strokes.getline(line, 500);//get FPS
      find = line;
      pos = find.find_first_of(' ', 0);
      num = find.substr(pos + 1, (find.size() - pos - 1));
      if (abs(stod(num) - get_FPS_vid()) > .01) { //to remove any rounding errors from the text file
        cout << "FPS dont match file, open failed" << endl;
        return false;
      }
      read_strokes.getline(line, 500);//get Hight and width
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
      read_strokes.getline(line, 500);//get #of frames 
      find = line;
      pos = find.find_first_of(' ', 0);
      num = find.substr(pos + 1, (find.size() - pos - 1));
      if (stoi(num) != get_num_frames()) {
        cout << stoi(num) << endl;
        cout << "number of frames dose not match file, open failed" << endl;
        return false;
      }

      //start loading data
      //the data line string is roughly the size of the number of frames because at most there will be
      //2.5 swim cycles per second wich traslates to 2.5 annotaions per 30 frames and if at most 
      //8 chars worth of data make up one annotation then 2.5*8 < 30 => number_frames is sufficent in size 
      char* data_line = new char[number_frames];//create string for holding stroke data

      size_t pos_num = 0;
      size_t pos_num_check = 0;
      size_t pos_num_end = 0;
      int num_val = 0;
      int jj = 0;
      swim_data_stroke temp_insert = { -1, -1 };

      for (ii = 0; ii < 10; ii++) {//look for each lane

        read_strokes.getline(data_line, number_frames);
        
        find = data_line;
        pos_num = 0;
        pos_num_end = 0;
        pos_num_check = find.find_first_of('}',0);

        while (pos_num_check != string::npos) {

          pos_num = find.find_first_of('{', pos_num);
          if (pos_num > pos_num_check) {
            cout << "Error in brakets in lane " << ii << endl;
            delete[] data_line;
            return false;
          }
          pos_num_end = find.find_first_of(',', pos_num);
          if (pos_num_end > pos_num_check) {
            cout << "Error in braket syntax in lane " << ii <<", no \",\" after number." <<  endl;
            delete[] data_line;
            return false;
          }
          //insert first value
          num_val = stoi(find.substr(pos_num + 1, pos_num_end - pos_num - 1));
          temp_insert.cycle = num_val;

          pos_num = find.find_first_of(' ', pos_num);
          if (pos_num > pos_num_check) {
            cout << "Error in braket syntax in lane " << ii << ", no \" \" after number." << endl;
            delete[] data_line;
            return false;
          }
          //insert second val
          num_val = stoi(find.substr(pos_num + 1, pos_num_check - pos_num - 1));
          temp_insert.stroke_spec = num_val;

          stroke_data[ii].push_front(temp_insert);//add text data to memroy
          pos_num = pos_num_check;
          pos_num_check = find.find_first_of('}', pos_num_check+1);
        } 
        if (read_strokes.eof()) { //If not all lanes have annotations stop looking for data
          cout << "out of data at lane " << ii << endl;
          break;
        }
      }
      
      delete[] data_line;
    }
    else { //video has not been anotated before create new one
      create_stokes.open(header_filename);
      if (!create_stokes.is_open()) {
        cout << "Could not create strokes file!" << endl;
        return false;
      }
      create_stokes << "#header{ right_s, left_s, stroke_class }, Frames Per Second, Resolution (hight, width), Total Number Frames\n";
      create_stokes << "#stroke classes fly == 1, back == 2, breast == 3, free == 4 this is number 3\n";
      create_stokes << "#stroke is symmetrical numbers 1 and 2 are same\n";
      create_stokes << "#stroke is asymmetrical numbers of one numbers 1 or 2 will be - 1\n";
      create_stokes << "#lane 0 starts at line 10\n";
      create_stokes << "#lane 9 starts at line 19\n";
      create_stokes << "FPS: " << FPS << endl;
      create_stokes << "RES: " << n << " " << m << endl;
      create_stokes << "TNF: " << number_frames;
    }
    return true;
  }

  return false;
}


void stroke_annotate::print_play_vid_dialog()
{
  cout << "\nAnnotation options" << endl;
  cout << "For left half cycel, press l" << endl;
  cout << "For right half cycel, press r" << endl;
  cout << "To slow video speed, press s" << endl;
  cout << "To speed up video speed, press f" << endl;
  cout << "To toggle pause, press p" << endl;
  cout << "To quit, press esc" << endl;
  cout << "Play>> ";
}


bool stroke_annotate::play_video()
{
  Mat frame;
  int num_frames = get_num_frames();
  int ii = get_current_frame();
  int input = -1;
  int skip_size = get_skip_size();
  char window_text[45];
  print_play_vid_dialog();
  sprintf(window_text, "Current lane number: %i, Current Class: %i", get_current_swimmer(), current_class);

  while (ii < (num_frames - skip_size)) {
    frame = get_current_Mat();
    //dispaly current lane, and current class
    putText(frame, window_text, Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
    imshow(AN_WINDOW_NAME, frame);
    input = waitKey(video_speed);

    switch (input)
    {
    case 144://r
      save_annotation(true);
      cout << char(input) << endl;
      print_play_vid_dialog();
      break;
    case 108://l
      save_annotation(false);
      cout << char(input) << endl;
      print_play_vid_dialog();
      break;
    case 115://s
      change_video_speed(false);
      cout << char(input) << endl;
      print_play_vid_dialog();
      break;
    case 102://f
      change_video_speed(true);
      cout << char(input) << endl;
      print_play_vid_dialog();
      break;
    case 112://p
      pause_video();
      print_play_vid_dialog();
      sprintf(window_text, "Current lane number: %i, Current Class: %i", get_current_swimmer(), current_class);
      break;
    case 27://esc
      cout << char(input) << endl;
      if (quit_and_save_data()) {
        cout << "Quiting app" << endl;
        destroyWindow(AN_WINDOW_NAME);
        return true;
      }
      print_play_vid_dialog();
      break;
    default:
      next_frame();//get the next frame
      break;
    }
    if (ii == (num_frames - 2*skip_size)) {
      cout << "\nwould you like to exit? (y/n)" << endl;
      frame = get_current_Mat();
      imshow(AN_WINDOW_NAME, frame);
      input = waitKey(0);
      if (input != 'y') {
        cout << "\nvideo over, automaticly paused" << endl;
        pause_video();
        last_frame();
      }
    }
    ii = get_current_frame();
  }

  destroyWindow(AN_WINDOW_NAME);
  return true;
}


void stroke_annotate::save_annotation(bool is_right)
{
  int current_frame = get_current_frame();
  swim_data_stroke temp_data = { -1, -1 };
  int current_lane = get_current_swimmer();
  current_lane = get_current_swimmer();
  
  temp_data.cycle = current_frame;
  if (is_right) {
    temp_data.stroke_spec = current_class;
    temp_data.stroke_spec = current_class;
  }
  else {//is left
    if ((current_class == 2) || (current_class == 4)) {
      temp_data.stroke_spec = current_class * 10;
    }
    temp_data.stroke_spec = current_class;
  }
  stroke_data[current_lane].push_back(temp_data);
  return;
}


//changes the current class lable for the box created
void stroke_annotate::change_class()
{
  char class_num = '0';
  int num = -1;
  bool done;
  Mat frame;

  //select lane number
  do {
    cout << "What class are we lableing? Options are..." << endl;
    cout << "Fly (1), Back (2), Brest (3), Free (4)" << endl;
    cout << "Class: ";

    //Get the key from the window
    frame = get_current_Mat();
    imshow(AN_WINDOW_NAME, frame);
    class_num = waitKey(0);
    cout << class_num << endl;

    if (!isdigit(class_num)) {
      num = -1;
    }
    else {
      num = int(class_num) - 48;//convert to int
    }

    if ((num > 4) || (num < 1)) {
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


void stroke_annotate::pause_video()
{
  Mat frame;
  int input = -1;
  char window_text[45];
  int current_swimmer = get_current_swimmer();
  
  //sort the lists into assending order
  for (int ii = 0; ii < 10; ii++) stroke_data[ii].sort(cmp);

  while (input != 112) {//p == 112

    cout << "\nPaused annotation options" << endl;
    cout << "Go foward, press d" << endl;
    cout << "Go back, press a" << endl;
    cout << "For left half cycel, press l" << endl;
    cout << "For right half cycel, press r" << endl;
    cout << "To delete annotation, press b" << endl;
    cout << "To change lane, press c" << endl;
    cout << "To change class, press t" << endl;
    cout << "To toggle pause, press p" << endl;
    cout << "Pause>> ";

    frame = get_current_Mat();
    sprintf(window_text, "Current lane number: %i, Current Class: %i", current_swimmer, current_class);
    putText(frame, window_text, Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
    if (check_for_annotation(current_swimmer, get_current_frame(),false)) {
      putText(frame, "ANNOTATION PRESSENT", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
    }
    imshow(AN_WINDOW_NAME, frame);
    input = waitKey(0);
    cout << char(input) << endl;

    switch(input)
    {
    case 100://d
      next_frame();
      break;
    case 144://r
      save_annotation(true);
      break;
    case 108://l
      save_annotation(false);
      break;
    case 98://b
      if(!check_for_annotation(current_swimmer, get_current_frame(),true)) {
        cout << "Annotation does not exist!" << endl;
      }
      break;
    case 99://c
      select_lane_number();
      current_swimmer = get_current_swimmer();
      go_to_most_recent_annotation();
      break;
    case 97://a
      last_frame();
      break;
    case 116://t
      change_class();
      break;
    default:
      break;
    }
  }

  last_frame();

  update_text_file();
}


void stroke_annotate::change_video_speed(bool increase_speed)
{
  int old_speed = video_speed;

  if (increase_speed) {
    video_speed /= 2;
    if (video_speed < max_speed) {
      video_speed = old_speed;
      cout << "Cant increase speed any more!" << endl;
    }
  }
  else {
    video_speed *= 2;
    if (video_speed > min_speed) {
      video_speed = old_speed;
      cout << "Cant decrease speed any more!" << endl;
    }
  }
}


bool stroke_annotate::check_for_annotation(int input_current_lane, int input_current_frame, bool and_delete)
{
  //check for out of bounds lane number
  if ((input_current_lane > 9) || (input_current_lane < 0)) return false;

  list<swim_data_stroke>::iterator point_data = stroke_data[input_current_lane].begin();
  int ii = 0;

  for (ii = 0; ii < stroke_data[input_current_lane].size(); ii++) {
    if (point_data->cycle == input_current_frame) {
      if (and_delete) {
        stroke_data[input_current_lane].erase(point_data);
      }
      return true;
    }
    point_data++;
  }

  return false;
}


void stroke_annotate::go_to_most_recent_annotation()
{
  int current_lane = get_current_swimmer();

  //sort the lists into assending order
  for (int ii = 0; ii < 10; ii++) stroke_data[ii].sort(cmp);

  list<swim_data_stroke>::iterator point_data;
  point_data = stroke_data[current_lane].end();
  point_data--;//dont know why this is nesasary (must be because the next of the list is pointing at NULL)
  set_current_frame(point_data->cycle);
}


bool stroke_annotate::update_text_file()
{
  ofstream update_header;
  string header_filename = get_video_file();
  char line[500];
  string header_data;
  double FPS = get_FPS_vid();
  int n = get_hight();
  int m = get_width();
  int number_frames = get_num_frames();

  //sort the lists into assending order
  for (int ii = 0; ii < 10; ii++) stroke_data[ii].sort(cmp);

  update_header.open("_app_str.txt");//temp file

  //make a new text file called _app_str.txt and then when created change the file names
  if (update_header.is_open()) {//if file opened start updating 
    int ii = 0;//frame number
    int jj = 0;//lane number

    list<swim_data_stroke>::iterator point_data;
    update_header << "#header{ right_s, left_s, stroke_class }, Frames Per Second, Resolution (hight, width), Total Number Frames\n";
    update_header << "#stroke classes fly == 1, back == 2, breast == 3, free == 4 this is number 3\n";
    update_header << "#stroke is symmetrical numbers 1 and 2 are same\n";
    update_header << "#stroke is asymmetrical numbers of one numbers 1 or 2 will be - 1\n";
    update_header << "#lane 0 starts at line 10\n";
    update_header << "#lane 9 starts at line 19\n";
    update_header << "FPS: " << FPS << endl;
    update_header << "RES: " << n << " " << m << endl;
    update_header << "TNF: " << number_frames << endl;

    for (ii = 0; ii < 10; ii++) {//look at each frame
      point_data = stroke_data[ii].begin();
      for (jj = 0; jj < stroke_data[ii].size(); jj++) {
        update_header << "{" << point_data->cycle << ", " << point_data->stroke_spec << "}";
        point_data++;
      }
      update_header << endl;
    }
    update_header.close();

    //rename the files
    //Change file name to end it .txt
    size_t pos = header_filename.find_first_of('.', 0);
    header_filename.erase(pos, 4);
    header_filename.append("_str.txt");
    std::remove("back_up_str.txt");//remove any file if exists
    if (rename(header_filename.c_str(), "back_up_str.txt") == 0) {//create a backup file
      if (rename("_app_str.txt", header_filename.c_str()) == 0) {
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


void stroke_annotate::start_up()
{
  select_lane_number();
  change_class();
  go_to_most_recent_annotation();
  pause_video();
}


bool stroke_annotate::quit_and_save_data()
{
  if (update_text_file()) {
    return true;
  }
  return false;
}
