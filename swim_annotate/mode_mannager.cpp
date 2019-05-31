#include "mode_mannager.h"
#include <string>
#include <iostream>

using namespace std;

mode_mannager::mode_mannager() {
  //make up a defalt root directory for inizalization 
  root_file_location = "blah blah blah";
  mode_val = startup_mode;
  prep_veiw_flag = false;
}

mode_mannager::mode_mannager(curent_mode start, string root_project_file) {
  root_file_location = root_project_file;
  mode_val = start;
  prep_veiw_flag = false;
}

//Waits for the users next request 
void mode_mannager::service_next_request() {

  switch (mode_val) {
  case startup_mode: 
    mode_val = print_start_up_options();
    if (prep_veiw_flag) {
      //start video player

    }
    break;
  case prep_mode: 
    mode_val = print_prep_options();
    break;
  case lab_mode: 
    mode_val = print_lab_options();
    break;
  case store_mode: 
    mode_val = print_store_options();
    break;
  case exit_opt: 
    mode_val = exit_opt;// not sure if this is requried
    break;
  default:
    cout << "An unrecognised value was input\nError occured!!";
    mode_val = err_val;
    break;
  }

}

//Checks if app requested to shut down
bool mode_mannager::is_app_finished() {
  if (mode_val == exit_opt) {
    return true;
  }
  
  return false;
}

//Save all application things
void mode_mannager::kill_app() {
  cout << "Killing app!" << endl;
}

//Returns the current mode
curent_mode mode_mannager::get_current_mode() {
  return mode_val;
}


curent_mode mode_mannager::print_start_up_options() {
  string answer;
  int ans = 0;
  curent_mode res;

  cout << "\nYou are in startup mode please select the following mode options:" << endl << endl;
  cout << "Go to prepossessing mode, press (1)" << endl;
  cout << "Go to labelling mode, press (2)" << endl;
  cout << "Go to storage mode, press (3)" << endl;
  cout << "Exit application, press (4)" << endl;
  cout << "\nstartup>> ";

  cin >> answer;
  ans = stoi(answer, nullptr, 10);

  switch (ans) {
  case 1: res = prep_mode;
    break;
  case 2: res = lab_mode;
    break;
  case 3: res = store_mode;
    break;
  case 4: res = exit_opt;
    break;
  default: res = err_val;
    cout << "An unrecognised value was input\n";
    break;
  }

  return res;
}

//prints the user options when in prep mode
curent_mode mode_mannager::print_prep_options() {
  string answer;
  int ans = 0;
  curent_mode res;
 
 /*
-	view video options
-	Mark as finished for labelling
-	Save current progress and exit mode
-	Delete current video
-	Load a video in the prepossessing file
-	Leave preprocessing mode
*/

  cout << "\nYou are in preprocessing mode please select the following mode options:" << endl << endl;
  cout << "View video options, press (1)" << endl;
  cout << "Mark as finished for labelling, press (2)" << endl;
  cout << "Delete current video, press (3)" << endl;
  cout << "Load a video in the prepossessing file, press (4)" << endl;
  cout << "Leave preprocessing mode, press (5)" << endl;
  cout << "\nprep>> ";

  cin >> answer;
  ans = stoi(answer, nullptr, 10);

  switch (ans) {
  case 1: res = prep_mode;
    prep_veiw_flag = true;
    break;
  case 2: res = prep_mode;
    break;
  case 3: res = prep_mode;
    break;
  case 4: res = prep_mode;
    break;
  case 5: res = startup_mode;
    break;
  default: res = err_val;
    cout << "An unrecognised value was input\n";
    break;
  }

  return res;
}

//prints the user options when in lab mode
curent_mode mode_mannager::print_lab_options() {
  string answer;
  int ans = 0;
  curent_mode res;

/*
- Start annotating video
-	Quit labelling mode
-	Load new video
-	Mark video as finished for storage
*/

  cout << "\nYou are in labelling mode please select the following mode options:" << endl << endl;
  cout << "Start annotating video, press (1)" << endl;
  cout << "Mark video as finished for storage, press (2)" << endl;
  cout << "Load new video, press (3)" << endl;
  cout << "Quit labelling mode, press (4)" << endl;
  cout << "\nlab>> ";

  cin >> answer;
  ans = stoi(answer, nullptr, 10);

  switch (ans) {
  case 1: res = lab_mode;
    break;
  case 2: res = lab_mode;
    break;
  case 3: res = lab_mode;
    break;
  case 4: res = startup_mode;
    break;
  default: res = err_val;
    cout << "An unrecognised value was input\n";
    break;
  }

  return res;
}

//prints the user options when in store mode
curent_mode mode_mannager::print_store_options() {
  string answer;
  int ans = 0;
  curent_mode res;

  /*
-	Store a new labeling mode file
-	Refile a video and its data or finish a file that has not been finished
-	Exit storage mode to start up mode and save current progress
  */

  cout << "\nYou are in storage mode mode please select the following mode options:" << endl << endl;
  cout << "Store a new labeling mode file, press (1)" << endl;
  cout << "Refile a video and its data or finish a file that has not been finished, press (2)" << endl;
  cout << "Exit storage mode, press (3)" << endl;
  cout << "\nstore>> ";

  cin >> answer;
  ans = stoi(answer, nullptr, 10);

  switch (ans) {
  case 1: res = store_mode;
    break;
  case 2: res = store_mode;
    break;
  case 3: res = startup_mode;
    break;
  default: res = err_val;
    cout << "An unrecognised value was input\n";
    break;
  }

  return res;
}
