#include "sub_video.h"

sub_video::sub_video()
{
}

void sub_video::make_subvideo()
{
  int ii = 0;
  
  for (ii = 0; ii < 10; ii++) {
    //Find all tracked swimmers from one lane (object ID) - make sure they are in order
    make_res_values(ii);
    //Decided best hight and width to take from the origanl video

    //Scale the video and save
    
  }
  return;
}
