#include "supper_annotator.h"

supper_annotator::supper_annotator() 
{
  video_file = "";
  current_swimmer = -1;
  an_video = NULL;
  current_frame = 0;
  all_data = nullptr;
  number_of_frames = -1;
  current_image = NULL;
  //set box
  current_box.height = 0;
  current_box.width = 0;
  current_box.x = 0;
  current_box.y = 0;
}

bool supper_annotator::load_video(string video_file)
{
  
  an_video.open(video_file); //check if file can be opened befor starting

  if (an_video.isOpened()) {

    //set class member fields
    number_of_frames = int(an_video.get(CAP_PROP_FRAME_COUNT));
    this->video_file = video_file;
    current_frame = 0;
    an_video >> current_image;  
    return true;
  }
  else {
    cout << "Supper annotator could not open video file\n";
    return false;
  }
}

//gets the next acction to be exicuted on the video data
bool supper_annotator::annotation_options()
{
  //I want unbuffered input to speed up annotations however this is difficult to do
  // there is an option to maybe use the OpenCV API setMouseCallback however I just
  // want somthing to work for now
  char answer;
  int ans = -1;

  cout << "\nOptions for annotation editing.\n\n";
  cout << "Change lane number of annotations, press (1)\n";
  cout << "Save current annotation, press (2)\n";
  cout << "Fix predicted annotation, press (3)\n";
  cout << "Go back to last frame, press (4)\n";
  cout << "Go to next frame, press (5)\n";
  cout << "Move to any arbitrary frame, press (6)\n";
  cout << "Stop annotating video, press (7)\n";
  cout << "\nAnnotate> ";

  cin.ignore(1000, '\n');

  cin >> answer;
  
  if (!isdigit(answer)) {
    cout << "An unrecognised value was input\n";
    return false;
  }
  else {
    ans = int(answer) - 48;//convert to int
  }

  switch (ans) {
  case 1://Change lane number of annotations
    select_lane_number();
    break;
  case 2://Save current annotation
    predict_next_frame();
    break;
  case 3://Fix predicted annotation
    break;
  case 4://Go back to last frame
    break;
  case 5://Go to next frame
    next_frame();
    break;
  case 6://Move to any arbitrary frame
    break;
  case 7://Stop annotating video
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

//select the lane number of the swimmer you are annotating
//return prevouse lane number
//return -1 on error
void supper_annotator::select_lane_number() {
  
  char lane_num;
  int num = -1;
  bool done;

  //select lane number
  do {
    cout << "What lane number are we working on? ";
    cin >> lane_num;

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

bool supper_annotator::create_ROI_in_pool()
{
  current_box = selectROI("Annotating Window", current_image, false, false);
  if(current_box.empty()) return false;

  return true;
}

void supper_annotator::predict_next_frame()
{
  //Can use cam shift
  //Can use Optical Flow

  //Current OpenCV APIs
  //Boosting tracker
  //CSRT tracker
  //KCF(Kernelized Correlation Filter) tracke
  //GOTURN(Generic Object Tracking Using Regression Networks) tracker
  //MOSSE (Minimum Output Sum of Squared Error) tracker
  //TLD(Tracking, learning and detection) tracker , good for occlutions



  

  return;
}

//to show box change class object flag
//primary loop control for supper_annotator class
bool supper_annotator::display_current_frame()
{
  namedWindow("Annotating Window", WINDOW_AUTOSIZE);
  Mat temp_frame = current_image;
  bool temp_var = true;

  if (temp_var) {
    rectangle(temp_frame, current_box, Scalar(255, 0, 0), 2, 1);// will create a box in the image frame
    imshow("Annotating Window",temp_frame);
  }
  else {
    imshow("Annotating Window", current_image);
  }
  if (annotation_options()) {
    return true;
  }
  return false;//exiting annotator
}

bool supper_annotator::quit_and_save_data()
{
  char answer = 'n';
  bool keep_asking = true;

  do {
    cout << "Are you sure you are done? (y/n)" << endl;

    cin.ignore(1000, '\n');//remove extra stuff 

    cin >> answer;
    if (answer == 'n') {
      keep_asking = false;
      return false;
    }
    else if (answer == 'y') {
      keep_asking = false;
      return true;
    }
    else {
      keep_asking = true;
    }
  } while (keep_asking);

  return false;
}


//Annotate next frame 
//We are going to skip every other frame
void supper_annotator::next_frame()
{
  current_frame += 2;
  if (current_frame > number_of_frames) {
    current_frame -= 2;
    cout << "This is the last frame in this video" << endl;
    return;
  }

  an_video.retrieve(current_image, current_frame);//This is not grabing a new frame
 

  return;
}

void supper_annotator::last_frame()
{
  return;
}

bool supper_annotator::go_to_frame(int frame_num)
{
  return false;
}

swim_data* supper_annotator::get_swim_data()
{
  return nullptr;
}


supper_annotator::~supper_annotator()
{

}



