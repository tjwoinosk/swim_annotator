#include "supper_annotator.h"

supper_annotator::supper_annotator()
{
  video_file_name = "";
  an_video = NULL;
  current_frame = 0;
  number_of_frames = -1;
  skip_size = 3;//how many frames to skip every new frame
  current_swimmer = -1;//lane number of swimmer
  video_file_open = false;
  FPS_vid = 0;
  hight = 0;
  width = 0;
}


supper_annotator::supper_annotator(int class_skip_size) {
  video_file_name = "";
  an_video = NULL;
  current_frame = 0;
  number_of_frames = -1;
  skip_size = class_skip_size;//how many frames to skip every new frame
  current_swimmer = -1;//lane number of swimmer
  video_file_open = false;
  FPS_vid = 0;
  hight = 0;
  width = 0;
}


bool supper_annotator::load_video(string video_file)
{
  video_file_name = "";
  an_video = NULL;
  current_frame = 0;
  number_of_frames = -1;
  skip_size = 3;//how many frames to skip every new frame
  current_swimmer = -1;//lane number of swimmer
  video_file_open = false;
  FPS_vid = 0;
  hight = 0;
  width = 0;

  video_file_name = video_file;
  if (an_video.open(video_file)) {
    number_of_frames = int(an_video.get(CAP_PROP_FRAME_COUNT));
    FPS_vid = an_video.get(CAP_PROP_FPS);
    hight = an_video.get(CAP_PROP_FRAME_HEIGHT);
    width = an_video.get(CAP_PROP_FRAME_WIDTH);
    namedWindow(AN_WINDOW_NAME, WINDOW_NORMAL);
    video_file_open = true;
    return true;
  }
  else {
    video_file_open = false;
    return false;
  }
}


//select the lane number of the swimmer you are annotating
//return prevouse lane number
//return -1 on error
void supper_annotator::select_lane_number() {
  
  char lane_num;
  int num = -1;
  bool done;
  Mat frame;

  //select lane number
  do {
    cout << "What lane number are we working on? ";

    //Get the key from the window
    an_video.set(CAP_PROP_POS_FRAMES, current_frame);//CV_CAP_PROP_POS_FRAMES
    an_video >> frame;
    imshow(AN_WINDOW_NAME, frame);
    lane_num = waitKey(0);
    cout << lane_num << endl;

    if (!isdigit(lane_num)) {
      num = -1;
    }
    else {
      num = int(lane_num) - 48;//convert to int
    }

    if ((num > 9) || (num < 0)) {
      cout << "\nAn invalid lane number was selected" << endl;
      done = false;
    }
    else {
      current_swimmer = num;
      done = true;
    }
  } while (!done);

  return;
}


//Annotate next frame 
//We are going to skip every other frame
void supper_annotator::next_frame()
{
  current_frame += skip_size;

  if (current_frame > (number_of_frames-1)) {
    current_frame -= skip_size;
    cout << "This is the last frame in this video" << endl;
    return;
  } 

  return;
}


void supper_annotator::last_frame()
{
  current_frame -= skip_size;

  if (current_frame < 0) {
    current_frame += skip_size;
    cout << "Cant got farther back, this is the frist frame in this video" << endl;
    return;
  }

  return;
}


//go to a spesified frame 
void supper_annotator::go_to_frame()
{
  int frame_num = -1;
  string answer;
  string nums("1234567890");
  int right_frame=0;

  cout << "Input the frame number: ";

  cin.ignore(1000, '\n');
  cin >> answer;
  if (answer.find_first_not_of(nums) != string::npos) {
    cout << "Invalid input for frame number" << endl;
    return;
  }

  frame_num = stoi(answer);

  //Move the input frame number so that it is a multiple of the frame skip size
  right_frame = frame_num % skip_size;
  frame_num -= right_frame;

  if ((frame_num < 0) || (frame_num > (number_of_frames-1))) {
    cout << "This is not a vaid frame number for this video" << endl;
    return;
  }
  current_frame = frame_num;

  return;
}


bool supper_annotator::create_ROI_in_pool(Rect *current_box)
{
  Mat frame;
  an_video.set(CAP_PROP_POS_FRAMES, current_frame);//CV_CAP_PROP_POS_FRAMES
  an_video >> frame;
  *current_box = selectROI(AN_WINDOW_NAME, frame, false, false);

  if (current_box->empty()) {
    return false;
  }
  else {
    return true;
  }
}


Mat supper_annotator::get_current_Mat()
{
  Mat frame;
  //get the frist frame
  an_video.set(CAP_PROP_POS_FRAMES, current_frame);//CV_CAP_PROP_POS_FRAMES
  an_video >> frame;
  return frame;
}


void supper_annotator::quit_app()
{
  video_file_open = false;
  destroyWindow(AN_WINDOW_NAME);
}


//Ask user for the skip size to use
//can only skip up to 9 frames
void supper_annotator::prompt_skip_size() {
  
  char num_skip;
  int num = -1;
  bool done;
  Mat frame;

  //select lane number
  do {
    cout << "What skip size would you like? ";

    //Get the key from the window
    an_video.set(CAP_PROP_POS_FRAMES, current_frame);//CV_CAP_PROP_POS_FRAMES
    an_video >> frame;
    imshow(AN_WINDOW_NAME, frame);
    num_skip = waitKey(0);
    cout << num_skip << endl;

    if (!isdigit(num_skip)) {
      num = -1;
    }
    else {
      num = int(num_skip) - 48;//convert to int
    }

    if ((num > 9) || (num < 1)) {
      cout << "\nAn invalid skip size was selected" << endl;
      done = false;
    }
    else {
      skip_size = num;
      done = true;
    }
  } while (!done);

  return;
}

