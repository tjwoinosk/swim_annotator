#pragma once
#include "mode_mannager.h"


class start_up_mode :
  public mode_mannager
{

  //Print the startup options for the mode
  //returns the input value of the user to change the current mode  
  curent_mode print_start_up_options();

  //Shows Statistics on the amount and type of each labled data
  void get_data_sats();

};

