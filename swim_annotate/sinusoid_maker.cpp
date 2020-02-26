#include "sinusoid_maker.h"

//Assume that the sin starts at one and ends at one and drops to zero
sinusoid_maker::sinusoid_maker(unsigned int num_points)
{
  vector<double> func(num_points, 0);
  int ii = 0;
  const double start = 0;
  const double end = 4 * asin(1);//2pi

  double skip = (end - start) / (double(num_points) + 1);
  double temp = 0;
  for (ii = 0; ii < num_points; ii++) {
    func[ii] = (cos(temp)+1)/2 ;
    temp += skip;
  }
  interp = func;
}

vector<double> sinusoid_maker::get_interp()
{
  return interp;
}

vector<double> sinusoid_maker::get_flat()
{
  vector<double> flat(interp.size(), 0.5);
  return flat;
}
