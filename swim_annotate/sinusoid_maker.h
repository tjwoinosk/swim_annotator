#pragma once

#include <math.h>
#include <vector>


using namespace std;
//using namespace cv;

//Class that makes vector of sinusoids
class sinusoid_maker
{

private:
  
  vector<double> interp;
  
public:

  sinusoid_maker(unsigned int num_points);

  vector<double> get_interp();

  vector<double> get_flat();

};

