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

//select the lane number of the swimmer you are annotating
//return prevouse lane number
//return -1 on error
bool supper_annotator::select_lane_number(int lane_num)
{
  if ((lane_num > 9) || (lane_num < 0)) {
    cout << "\nAn invalid lane number was selected" << endl;
    return false;
  }
  else {
    current_swimmer = lane_num;
    return true;
  }
}

bool supper_annotator::create_ROI_in_pool()
{
  Rect selected_box;
  namedWindow("Annotating Window", 0);
  selected_box = selectROI("Annotating Window", current_image, false, true);
}

swim_data* supper_annotator::get_swim_data()
{
  return nullptr;
}

bool supper_annotator::next_frame()
{
  return false;
}

bool supper_annotator::last_frame()
{
  return false;
}

bool supper_annotator::go_to_frame(int frame_num)
{
  return false;
}

bool supper_annotator::quit_and_save_data()
{
  return false;
}



supper_annotator::~supper_annotator()
{

}



