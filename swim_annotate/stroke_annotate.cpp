#include "stroke_annotate.h"

stroke_annotate::stroke_annotate()
{
  set_skip_size(1);
  video_speed = 1;
  all_data = nullptr;
}

bool stroke_annotate::load_video_for_stroke_counting(string video_file)
{
  if (load_video(video_file)) {
    return true;
  }

  return false;
}

bool stroke_annotate::play_video()
{
  Mat frame;
  int num_frames = get_num_frames();
  int ii = get_current_frame();
  int input = -1;
  int skip_size = get_skip_size();

  cout << "Annotation options" << endl;
  cout << "For left half cycel, press l" << endl;
  cout << "For right half cycel, press r" << endl;
  cout << "To slow video speed, press s" << endl;
  cout << "To speed up video speed, press f" << endl;
  cout << "To toggle pause, press p" << endl;
  cout << "To quit, press esc" << endl;

  while (ii < (num_frames - skip_size)) {
    frame = get_current_Mat();
    imshow(AN_WINDOW_NAME, frame);
    input = waitKey(10);

    switch (input)
    {
    case 144://r
      break;
    case 108://l
      break;
    case 112://p
      pause_video();
      break;
    default:
      next_frame();//get the next frame
      break;
    }
    if (ii == (num_frames - 2*skip_size)) {
      cout << "would you like to exit? (y/n)" << endl;
      frame = get_current_Mat();
      imshow(AN_WINDOW_NAME, frame);
      input = waitKey(0);
      if (input != 'y') {
        cout << "video over, automaticly paused" << endl;
        pause_video();
        last_frame();
      }
    }
    ii = get_current_frame();
  }

  destroyWindow(AN_WINDOW_NAME);
  return true;
}

bool stroke_annotate::quit_and_save_data()
{
  return false;
}

bool stroke_annotate::annotation_options(char reply)
{


  return false;
}

bool stroke_annotate::save_annotation()
{
  return false;
}

bool stroke_annotate::update_text_file()
{
  return false;
}

void stroke_annotate::start_up()
{
}

void stroke_annotate::pause_video()
{
  Mat frame;
  int input = -1;

  cout << "Paused annotation options" << endl;
  cout << "Go foward, press d" << endl;
  cout << "Go back, press a" << endl;
  cout << "For left half cycel, press l" << endl;
  cout << "For right half cycel, press r" << endl;
  cout << "to delete annotation, press b" << endl;
  cout << "To toggle pause, press p" << endl;

  while (input != 112) {//p == 112
    frame = get_current_Mat();
    imshow(AN_WINDOW_NAME, frame);
    input = waitKey(0);
    switch(input)
    {
    case 100://d
      next_frame();
      break;
    case 144://r
      break;
    case 108://l
      break;
    case 97:
      last_frame();
      break;//a
    default:
      break;
    }
  }
}

void stroke_annotate::change_video_speed()
{

}

