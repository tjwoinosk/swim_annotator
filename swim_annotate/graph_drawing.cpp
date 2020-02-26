#include "graph_drawing.h"

graph_drawing::graph_drawing()
{
  image_changed = true;
  time_slice = double(1)/double(30);
  name_of_window = "defalt constructor";
  x_max = 1;//time in seconds
  current_graph = Mat::zeros(num_pixels_x, num_pixels_y, CV_8U);
}


graph_drawing::graph_drawing(string window_name)
{
  image_changed = true;
  time_slice = 1 / 30;
  name_of_window = window_name;
  x_max = 1;//time in seconds
  current_graph = Mat::zeros(num_pixels_x, num_pixels_y, CV_8U);
}


graph_drawing::graph_drawing(string window_name, double time_length, double time_delta)
{
  image_changed = true;
  time_slice = time_delta;
  name_of_window = window_name;
  x_max = time_length;//time in seconds
  current_graph = Mat::zeros(num_pixels_x, num_pixels_y, CV_8U);
}


//Create a window for graph
void graph_drawing::start_graph_drawer()
{
  current_graph = Mat::zeros(num_pixels_y, num_pixels_x, CV_8U);
  namedWindow(name_of_window, WINDOW_NORMAL);
  make_axis();
  make_grid(y_grid_lines);
  plot_data();
  imshow(name_of_window, current_graph);
}


//draws the x and y axis
void graph_drawing::make_axis()
{
  Point x_start = to_graph(0,0);
  Point x_finish = to_graph(x_max, 0);
  Point y_start = to_graph(0, 0);
  Point y_finish = to_graph(0, 1);
  arrowedLine(current_graph, x_start, x_finish, Scalar(255, 255, 255),1,8,0,.01);
  arrowedLine(current_graph, y_start, y_finish, Scalar(255, 255, 255),1,8,0,.01);
}


//automaticly puts grid lines for every second
void graph_drawing::make_grid(int num_lines_y)
{
  int ii = 0;
  const double font_mult = 0.25;
  vector<double> y_points(num_lines_y, 0);
  vector<double> x_points(int(x_max), 0);
  double y_val = 0, temp = 0;
  y_val = 1 / (double(num_lines_y) + 1);
  Point x_start, x_finish;

  if (num_lines_y < 5 && num_lines_y > 0) {
    temp = y_val;
    for (ii = 0; ii < num_lines_y; ii++) {
      y_points[ii] = temp;
      temp += y_val;
    }
    temp = 1;//starts at one second
    for (ii = 0; ii < int(x_max); ii++) {
      x_points[ii] = temp;
      temp += 1;
    }
  }
  else {
    cout << "No grid can be made" << endl;
    return;
  }

  //y axis lines 
  for (ii = 0; ii < num_lines_y; ii++) {
    x_start = to_graph(0, y_points[ii]);
    x_start.x -= axis_start / 4;
    x_finish = to_graph(x_max, y_points[ii]);
    make_y_lables(x_start, x_finish, y_points[ii], font_mult);
  }
  //x axis lines
  for (ii = 0; ii < int(x_max); ii++) {
    x_start = to_graph(x_points[ii], 0);
    x_start.y += axis_start / 4;
    x_finish = to_graph(x_points[ii], 1);
    make_x_lables(x_start, x_finish, x_points[ii], font_mult);
  }

  return;
}


void graph_drawing::make_y_lables(Point x_start, Point x_finish, double value, double font_mult)
{
  char buf[10];
  int jj = 0;

  LineIterator it(current_graph, x_start, x_finish, 8);//dash line
  for (jj = 0; jj < it.count; jj++, it++) if (jj % 10 < 4) { (*it)[0] = 255; }
  sprintf(buf, "%2.2f", value);
  x_start.x -= axis_start / 2;
  x_start.y -= axis_start / 2;
  putText(current_graph, string(buf), x_start, FONT_HERSHEY_SIMPLEX, font_mult, Scalar(255));
}


void graph_drawing::make_x_lables(Point x_start, Point x_finish, double value, double font_mult)
{
  char buf[10];
  int jj = 0;

  LineIterator it(current_graph, x_start, x_finish, 8);//dash line
  for (jj = 0; jj < it.count; jj++, it++) if (jj % 10 < 4) { (*it)[0] = 255; }
  sprintf(buf, "%2.2f", value);
  x_start.x += axis_start / 2;
  x_start.y += axis_start / 2;
  putText(current_graph, string(buf), x_start, FONT_HERSHEY_SIMPLEX, font_mult, Scalar(255));
}


void graph_drawing::plot_data()
{
  Point line_start, line_end;
  int ii = 0;
  double step = time_slice;
  double temp = 0;
  if (graph_data.size() > 0) {
    for (ii = 0; ii < (graph_data.size() - 1); ii++) {
      line_start = to_graph(temp, graph_data[ii]);
      temp += step;
      line_end = to_graph(temp, graph_data[ii + 1]);
      line(current_graph, line_start, line_end, Scalar(225), 1);
    }
  }
}


//draws top to bottom time position
void graph_drawing::make_position_line(double time_position)
{
  Point start_p = to_graph(time_position, 1);
  Point end_p = to_graph(time_position, 0);
  line(current_graph, start_p, end_p, Scalar(225));
}


Point graph_drawing::to_graph(double x_point, double y_point)
{
  Point map_point(0, 0);
  
  if ((x_point <= double(x_max)) && (x_point >= 0) && (y_point >= 0) && (y_point <= 1)) {
    map_point.x = int(x_point/double(x_max)*double(num_pixels_x-double(axis_start)-double(top_border))) + axis_start;
    map_point.y = int(double(num_pixels_y - axis_start - top_border)*(1-y_point)) + top_border;
  }
  else {
    cout << "Out of range point!" << endl;
  }

  return map_point;
}


void graph_drawing::draw_graph()
{
  if (image_changed) {
    current_graph = Mat::zeros(num_pixels_y, num_pixels_x, CV_8U);
    make_axis();
    make_grid(y_grid_lines);
    plot_data();
    imshow(name_of_window, current_graph);
    image_changed = false;
  }
}


void graph_drawing::draw_graph(double time_position)
{
  current_graph = Mat::zeros(num_pixels_y, num_pixels_x, CV_8U);
  make_axis();
  make_grid(y_grid_lines);
  plot_data();
  make_position_line(time_position);
  imshow(name_of_window, current_graph);
}


void graph_drawing::input_data(vector<double> data)
{
  image_changed = true;
  int ii = 0;
  
  for (ii = 0; ii < data.size(); ii++) {
    graph_data.push_back(data[ii]);
  }

}


void graph_drawing::kill_graph_drawer()
{
  destroyWindow(name_of_window);
}


//reuturs the number of data points reomved
//lookes for values of 1 
int graph_drawing::undo_work(unsigned int number_of_strokes_back)
{
  //graph_data
  int ii = 0, cntr = 0;
  vector<double> new_data;
  
  vector<double>::iterator look;
  vector<double>::iterator temp;

  for (look = graph_data.end(); graph_data.begin() != look; look--) {
    if (*look == 1) ii++;

    if (ii == number_of_strokes_back) {
      temp = look;
      break;
    }
    cntr++;
  }

  for (look = graph_data.begin(); look != temp; look++) {
    new_data.push_back(*look);
  }

  graph_data.clear();
  graph_data = new_data;
  
  
  return cntr;
}


//returns the number of frames n strokes back is
int graph_drawing::look_back(unsigned int number_of_strokes_back, int current_frame_number)
{
  //graph_data
  int ii = 0, jj = 0, cntr = 0;
  int start = current_frame_number;

  vector<double>::iterator look;
  vector<double>::iterator temp;

  if (current_frame_number > (graph_data.size()-1)) {
    start = graph_data.size()-1;
  }

  for (ii = start; ii >= 0; look--) {
    if (graph_data[ii] == 1) jj++;

    if (jj == number_of_strokes_back) {
      break;
    }
    cntr++;
  }

  return cntr;
}
