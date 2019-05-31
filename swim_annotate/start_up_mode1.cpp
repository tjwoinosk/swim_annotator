//Tim Woinoski
#include <iostream>
#include <string>

#include "start_up_mode.h"


using namespace std;


start_up_options print_start_up_options() {
  
  string answer;
  int ans = 0;
  start_up_options res;

  cout << "\nYou are in startup mode please select the following mode options:" << endl << endl;
  cout << "Go to prepossessing mode, press (1)" << endl;
  cout << "Go to labelling mode, press (2)" << endl;
  cout << "Go to storage mode, press (3)" << endl;
  cout << "Exit application, press (4)" << endl;
  
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

//Shows Statistics on the amount and type of each labled data
void get_data_sats() {


}