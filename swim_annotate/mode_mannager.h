#pragma once

#include <string> 

using namespace std;

enum curent_mode {prep_mode, lab_mode, store_mode, startup_mode, exit_opt, err_val};

class mode_mannager {

  curent_mode mode_val;
  string root_file_location;

public:

  //constructors
  mode_mannager();
  mode_mannager(curent_mode start, string root_project_file);

  //Waits for the users next request 
  void service_next_request();

  //Checks if app requested to shut down
  bool is_app_finished();

  //Save all application things
  void kill_app();

  //Returns the current mode
  curent_mode get_current_mode();

  //prints the user options when in start up mode
  curent_mode print_start_up_options();

  //prints the user options when in prep mode
  curent_mode print_prep_options();

  //prints the user options when in lab mode
  curent_mode print_lab_options();

  //prints the user options when in store mode
  curent_mode print_store_options();

};

