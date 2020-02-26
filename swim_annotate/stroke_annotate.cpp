#include "stroke_annotate.h"


void stroke_annotate::graph_example()
{

  char end = '0';
  char responce = '0';
  int cntr_start = 0, cntr_end = 0;
  double time_slice = double(1) / double(30);
  double vid_time = 22.5;

  //stroke_annotate stroke_work;
  graph_drawing tester("test window", vid_time, time_slice);

  tester.start_graph_drawer();

  while (end == '0') {
    responce = waitKey(25);
    if (responce == 't') {
      sinusoid_maker swim_data(cntr_end - cntr_start);
      tester.input_data(swim_data.get_interp());
      cntr_start = cntr_end;
    }
    else if (responce == 27) {
      end++;//break
    }
    else if (cntr_end == int(vid_time / time_slice)) {
      end++;//break
    }
    tester.draw_graph(double(cntr_end) * time_slice);
    cntr_end++;
  }

  tester.kill_graph_drawer();
}


void stroke_annotate::file_example()
{
  string test_file_name = "2_str.txt";

  SA_file_mannager tester(test_file_name);

  if (!tester.read_file()) {
    cout << "An error occured, could not read stroke annotation file in stroke_annoate.cpp" << endl;
  }

  if (!tester.save_file()) {
    cout << "An error occured, could not save stroke annotation file in stroke_annoate.cpp" << endl;
  }

}


//loads and opens all objects required for stroke annotation 
//Display all windows
void stroke_annotate::start_stroke_counting(string intput_video_file)
{

  video_file = intput_video_file;
  video_window_name = video_file + " video player";
  double FPS = 1;
  int frame_count = 0;
  int hight  = 0;
  int width = 0;
  Mat frame;

  string vf_text = video_file;
  char resp = '0';


  //Video stuff
  cap.open(video_file);
  if (!cap.isOpened()) {
    cout << "Could not open video file for stroke counting" << endl;
    return;
  }
  FPS = cap.get(CAP_PROP_FPS); frame_count = cap.get(CAP_PROP_FRAME_COUNT);
  hight = cap.get(CAP_PROP_FRAME_HEIGHT); width = cap.get(CAP_PROP_FRAME_WIDTH);
  waitKey(0);
  cap >> frame;
  if (frame.empty()) {
    cout << "Error, Could not read first frame of video" << endl;
    return;
  }
  namedWindow(video_window_name, WINDOW_NORMAL);
  imshow(video_window_name, frame);


  //File stuff
  vf_text.replace(vf_text.end() - 4, vf_text.end(), "_str.txt");
  //Get video paramiters
  man_file.input_info(vf_text, FPS, frame_count, hight, width);
  //look for files related to video file name
  if (!man_file.read_file()) {
    cout << "An error occured, when reading a stroke annotation file" << endl;
    return;
  }

  //Grapher stuff
  grapher.change_window_name(intput_video_file + " Stroke Visualization");
  grapher.input_data(man_file.return_y_values());
  grapher.input_varibales(double(frame_count)/FPS, double(1)/FPS);
  grapher.start_graph_drawer();

  print_vid_dialog();

  return;
}


//Show options for vid once
//wait for user to choose one
//Also display in video options
void stroke_annotate::print_vid_dialog()
{
  char resp = '0';

  while (1) {

    cout << "\nAnnotation options:" << endl;
    cout << "To start annotating (edit mode), press 1" << endl;
    cout << "To view any annotations and/or video (view mode), press 2" << endl;
    cout << "To quit, press 3" << endl << endl;

    cin.clear();
    cin.ignore(INT_MAX, '\n');
    cin >> resp;
    if (resp == '1') {
      //start annotating 
      annotate_video(true);
    }
    else if (resp == '2') {
      //view annotations
      annotate_video(false);
    }
    else if (resp == '3') {
      //quit
      quit_stroke_annotator();
      break;
    }
  }
}


//Start annotating video (edit mode)
//View any annotations and/or watch video (view mode)
//Fist ask if swimming is swimming or not, set swimmer_is_swimming accordingly
void stroke_annotate::annotate_video(bool is_edit_mode)
{
  Mat frame;
  char resp = '0';
  int cntr_end = 0, cntr_start = 0;
  double frame_rate = cap.get(CAP_PROP_FPS);
  vector<stroke_data> temp;
  stroke_data element;

  //Find out what swimmer is doing
  if(is_edit_mode) get_swimmer_stait();

  print_video_options(is_edit_mode);

  cout << "Video paused, press any key to start..." << endl;
  while (waitKey(0) < 0);

  //Main loop for annotation
  while (1) {

    resp = waitKey(int(frame_rate * video_speed));//video speed controle

    if (resp == 'p') {
      cout << "video paused" << endl;
      toggel_pause();
    }
    else if (resp == 't') {
      if(is_edit_mode) mark_stroke(cntr_start, cntr_end);
    }
    else if (resp == 'b') {
      skip_back(num_skip_back, cntr_end, cntr_start, is_edit_mode);
    }
    else if (resp == 'd') {//speed down
      change_speed(false);
    }
    else if (resp == 'u') {//speed up
      change_speed(true);
    }
    else if(resp == 'm') {//mark swimming
      if (is_edit_mode) {
        mark_stroke(cntr_start, cntr_end);//you must start and finish swimming on a stroke
        toggel_swimming();
      }
    }
    else if (resp == 27) {
      break;
    }
    
    cntr_end++;

    element.frame_num = cntr_end;
    element.is_swimming = swimmer_is_swimming;

    temp.push_back(element);
    
    //display next window frames;
    grapher.draw_graph(double(cap.get(CAP_PROP_POS_MSEC)) / double(1000));
    cap >> frame;
    if (frame.empty()) {
      cout << "Finished video" << endl;
      return;
    }
    imshow(video_window_name, frame);
  }

  return;
}


//prints options when editing or viewing stroke annotations
void stroke_annotate::print_video_options(bool is_edit_mode)
{
  cout << "In video options:" << endl << endl;
  
  //t option (mark stroke occuring)
  if(is_edit_mode) cout << "To mark a stroke, press t" << endl;

  //p option (pause)
  cout << "To toggle pause, press p" << endl;

  //b option (move back n strokes)
  if (!is_edit_mode) {
    cout << "To go back " << num_skip_back << " strokes without changing annotations, press b" << endl;
  }
  else {
    cout << "To go back " << num_skip_back << " strokes to change annotations, press b" << endl;
  }

  //m option (mark when swimmer is swimming and when they are not)
  if (is_edit_mode) cout << "To toggal swimmer swimming, press m" << endl;

  //d option (slow Down video playback)
  cout << "To slow down video, press d" << endl;
  
  //u option (speed Up video playback)
  cout << "To speed up video, press u" << endl;
  
  //quit option 
  cout << "To quit, press esc" << endl << endl;
}


//asks user to input if swimmer is swimming or not
void stroke_annotate::get_swimmer_stait()
{
  char resp = '0';

  while (1) {
    cout << "What is the swimmer doing right now? (swimming (s) or not (n))" << endl;
    cin.clear();
    cin.ignore(INT_MAX, '\n');
    cin >> resp;
    if (resp == 's') {
      swimmer_is_swimming = true;
    }
    else if (resp == 'n') {
      swimmer_is_swimming = false;
    }
  }
}


//double or half the speed every time (to a limit)
void stroke_annotate::change_speed(bool speed_up)
{
  double conv = log2(video_speed) + 2;

  if (speed_up) {
    if(conv > max_speed) {
      video_speed /= 2;
    }
    else {
      cout << "Cant speed up video any more!" << endl;
    }
  }
  else {
    if (conv < min_speed) {
      video_speed *= 2;
    }
    else {
      cout << "Cant slow down video any more!" << endl;
    }
  }
  return;
}


//flips the swimmer_is_swimming flag
void stroke_annotate::toggel_swimming()
{
  if (swimmer_is_swimming) {
    cout << "Swimmer stoped swimming!" << endl;
    swimmer_is_swimming = !swimmer_is_swimming;
  }
  else {
    cout << "Swimmer started swimming!" << endl;
    swimmer_is_swimming = !swimmer_is_swimming;
  }
}


//Changes cntr_end and cntr_star aproprately
//Modifies the data in the grapher object
void stroke_annotate::skip_back(int n_strokes, int &cntr_end, int &cntr_start, bool in_edit_mode)
{
  Mat frame;
  
  if (in_edit_mode) {
    cntr_end = cap.get(CAP_PROP_POS_FRAMES) - grapher.undo_work(n_strokes);
    cap.set(CAP_PROP_POS_FRAMES, cntr_end - 1);
    cntr_start = cntr_end;
    cap >> frame;

    imshow(video_window_name, frame);
    grapher.draw_graph(double(cap.get(CAP_PROP_POS_MSEC)) / double(1000));
    cout << "Video paused, press any key to start" << endl;
    while (waitKey(0) < 0);
  }
  else {
    cntr_end = grapher.look_back(n_strokes, cntr_end);
    cntr_start = cntr_end;
    cap.set(CAP_PROP_POS_FRAMES, cntr_end);
    grapher.draw_graph(double(cap.get(CAP_PROP_POS_MSEC)) / double(1000));
  }

  return;
}


//Tells the grapher object that a stroke occured
//needs two integers that will be subtracked
//Upates the grapher accordingly
void stroke_annotate::mark_stroke(int &cntr_start, int &cntr_end)
{
  //need to accout for the situation when swimmer is not swimming
  sinusoid_maker swim_data(cntr_end - cntr_start);
  if (!swimmer_is_swimming) {
    grapher.input_data(swim_data.get_interp());
  }
  else {
    grapher.input_data(swim_data.get_flat());
    
  }
  cntr_start = cntr_end;
}



//Specify stroke being prefomed in video (If not already spcifed ask to change)
//Save all work in file
//kill all windows
void stroke_annotate::quit_stroke_annotator()
{
  
  destroyWindow(video_window_name);
  man_file.save_file();
  grapher.kill_graph_drawer();
}

