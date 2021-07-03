#include "SA_file_mannager.h"

ostream& operator<<(ostream& out, strokes const& c)
{
  if (c == fly) { out << "fly"; }
  else if (c == back) { out << "back"; }
  else if (c == brest) { out << "brest"; }
  else if (c == freestyle) { out << "freestyle"; }
  else if (c == mixed) { out << "mixed"; }
  return out;
}


istream& operator>>(istream& in, strokes& c)
{
  const int buf_size = 10;
  char input[buf_size] = "";
  int ii = 0;
  
  in.getline(input, buf_size+1,'\n');
  if ((input[0] != '\0') && (cin.gcount() > buf_size)) {
    in.ignore(numeric_limits<streamsize>::max(), '\n');
  }

  if (strncmp(input, "fly\0", 4) == 0) { c = fly; }
  else if (strncmp(input, "back\0", 5) == 0) { c = back; }
  else if (strncmp(input, "brest\0", 6) == 0) { c = brest; }
  else if (strncmp(input, "freestyle\0", 10) == 0) { c = freestyle; }
  else if (strncmp(input, "mixed\0", 6) == 0) { c = mixed; }
  else {
    cout << "unrecogized stroke" << endl;
    c = mixed;
    cin.setstate(cin.failbit, true);
  }

  return in;
}


SA_file_mannager::SA_file_mannager() {

}


SA_file_mannager::SA_file_mannager(string file_name)
{
  private_file_name = file_name;
}


bool SA_file_mannager::read_file()
{
  fstream file_to_read;
  istringstream reader;
  string line;
  int ii = 0;
  string stroke_val;
  stroke_data input;
  char ch = ',';
  double FR_read = 0;
  int h_read = 0, w_read = 0, nf_read = 0;

  file_stroke_data.clear();
  file_to_read.clear();

  file_to_read.open(private_file_name);
  if (file_to_read.is_open()) {
    getline(file_to_read, line);
    if (!file_to_read.eof()) {
      getline(file_to_read, line);
      reader.str(line);
      reader >> FR_read >> ch >> h_read >> ch >> w_read >> ch >> nf_read;
      //check file matches video
      if ((FR_read != frame_rate) || (h_read != hight) || (w_read != width) || (nf_read != num_frames)) {
        cout << "Stroke annotaion file does not match video file" << endl;
        file_to_read.close();
        return false;
      }
    
      //read file
      while (getline(file_to_read, line)) {
        if ((line.empty()) || (line[0] == ' ') || (line[0] == '#')) continue;
        reader.clear();//if read error occurs rest wil still work
        reader.str(line);
        reader >> input.frame_num >> ch >> input.y_val >> ch >> input.is_swimming >> ch >> input.stroke_spec;
        file_stroke_data.push_back(input);
      }
    }
    file_to_read.close();
    return true;
  } else{
    file_to_read.open(private_file_name, fstream::out); 
    if (file_to_read.is_open()) {
      is_new_file = false;
      file_to_read.close();
      return true;
    }
  }
  cout << "Could not open " << private_file_name << endl;
  file_to_read.close();
  return false;
}


bool SA_file_mannager::save_file()
{
  fstream file_to_save;
  char ch = ',';
  int ii = 0;

  file_to_save.open(private_file_name,fstream::out);

  if (file_to_save.is_open()) {
    if (is_new_file) {
      cout << "Creating new file for stroke annotation" << endl;
      is_new_file = false;
    }
    file_to_save << "#FPS, RES_x, RES_y, TNF" << endl;
    file_to_save << frame_rate << ch << hight << ch << width << ch << num_frames << endl << endl;
    file_to_save << "#Frame Num, y_val, is_swimming, stroke_spec(fly, back, brest, freestyle)" << endl;

    for (ii = 0; ii < file_stroke_data.size(); ii++) {
      file_to_save << file_stroke_data[ii].frame_num << ch << file_stroke_data[ii].y_val << ch;
      file_to_save << file_stroke_data[ii].is_swimming << ch << file_stroke_data[ii].stroke_spec << endl;
    }
    return true;
  }
  else {
    cout << "Could not open stoke annotation file to save data" << endl;
    return false;
  }
}


vector<double> SA_file_mannager::return_y_values()
{
  vector<double> data(file_stroke_data.size());
  int ii = 0;
  
  for (ii = 0; ii < file_stroke_data.size(); ii++) {
    data[ii] = file_stroke_data[ii].y_val;
  }

  return data;
}

