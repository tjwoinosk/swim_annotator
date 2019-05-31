//Tim Woinoski
//2019-05-30
// swim_annotate.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This application labels swimming videos quicker

#include <iostream>
#include "mode_mannager.h"

using namespace std;

int main(int argc, char* argv[])
{
  cout << "\n\n\n";
  cout << "***************************************" << endl;
  cout << "*  Welcome to the swiming annotator!  *" << endl;
  cout << "***************************************" << "\n\n\n";

  string location = "not yet valid";
  

  //Use the comand line inputs 
  if (argc < 2) { // by defalt if no location is specified 
    //location = "C:\Users\tim_w\Documents\SFU\SU19_research\automated_swimming_analytics\data";
    cout << "Defalt memory location used" << endl;
  }
  else {
    location = argv[2];
    //Check if location is valid
    cout << "Location given at startup used" << endl;

  }

  mode_mannager app(startup_mode,location);

  while (!app.is_app_finished()) {
    app.service_next_request();
  }

  app.kill_app();

  return 0;
}




