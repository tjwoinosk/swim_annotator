#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream> 

using namespace std;

enum strokes { fly, back, brest, freestyle, mixed };//mixed is an other catigory

struct stroke_data {
  int frame_num;
  double y_val;
  bool is_swimming;
  strokes stroke_spec;
};

//for stroke spec
ostream& operator << (ostream& out, strokes const& c);
istream& operator >> (istream& in, strokes& c);

class SA_file_mannager
{

private:

  string private_file_name = "not_init.txt";
  vector<stroke_data> file_stroke_data;
  bool is_new_file = false;

  double frame_rate = 0;
  int num_frames = 0;
  int hight = 0;
  int width = 0;

public:

  SA_file_mannager();

  SA_file_mannager(string file_name);

  void input_info(string file_name, double fr, int num_fr, int h, int w) {
    private_file_name = file_name; frame_rate = fr; num_frames = num_fr; hight = h; width = w;
  }

  bool read_file();

  bool save_file();

  vector<stroke_data> return_data() { return file_stroke_data; }

  vector<double> return_y_values();

  //add data so that it can be saved
  void add_data(vector<stroke_data> data_in) {
    file_stroke_data.clear();
    file_stroke_data = data_in;
  }

};

