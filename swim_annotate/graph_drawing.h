#pragma once

#include <string>
#include <vector>

#include <opencv2/opencv.hpp> 
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


using namespace std;
using namespace cv;


//Custom class for drawing graphs in real time
//Graph domain will take values > 0 
//Graph range will take values [0,1]
class graph_drawing
{


private:

  Mat current_graph;
  bool image_changed;

  string name_of_window;

  vector<double> graph_data;
  double x_max;//time in seconds
  double time_slice;

  const int num_pixels_x = 800;
  const int num_pixels_y = 200;
  const int axis_start = 10;//gives room for tick lables
  const int top_border = 5;
  const int y_grid_lines = 3;

  void make_axis();
  void make_grid(int num_lines);
  void make_y_lables(Point x_start, Point x_finish, double value, double font_mult);
  void make_x_lables(Point x_start, Point x_finish, double value, double font_mult);
  void plot_data();
  void make_position_line(double time_position);

  //Fuction that maps points in graph to image representation
  Point to_graph(double x_point, double y_point);

public:

  graph_drawing();
  graph_drawing(string window_name);
  graph_drawing(string window_name, double time_length, double time_delta);

  void input_varibales(double video_length, double lenth_of_frame) { 
    x_max = video_length; time_slice = lenth_of_frame;
  }

  void change_window_name(string window_name) { name_of_window = window_name; }

  void remove_all_data() { graph_data.clear(); }

  //opens a window so a graph can be drawn
  void start_graph_drawer();

  //draws the graph in the window
  void draw_graph();

  //draws a time position line in the graph
  void draw_graph(double time_position);

  void input_data(vector<double> data);

  void set_graph_length(double total_time_of_vid) { x_max = total_time_of_vid; }

  //closes the graph window
  void kill_graph_drawer();
  
  //looks threw data for ones (signifys a stroke)
  //deletes data upto the point of the number of strokes back
  //returns the position in the data
  int undo_work(unsigned int number_of_strokes_back);

  //returns the number of frames n strokes back is
  int look_back(unsigned int number_of_strokes_back, int current_frame_number);

  vector<double> retreive_work() { return graph_data; }


};

