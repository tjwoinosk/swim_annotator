#include "swimmer_tracking.h"

swimmer_tracking::swimmer_tracking()
{
  int ii;
  for (ii = 0; ii < MAX_SORT; ii++) sorted_data[ii] = nullptr;
}

//Returns an in order array of pointers that point to the lane number requested 
void swimmer_tracking::make_res_values(int lane_number)
{
  int ii = 0;
  int jj = 0;
  //clear data
  for (ii = 0; ii < MAX_SORT; ii++) sorted_data[ii] = nullptr;

  //Get all the indecies of the objects holding the lane number
  ii = 0;
  if (results[ii].object_ID == lane_number) {
    if (jj < MAX_SORT) {
      sorted_data[jj] = &(results[ii]);
    }
    else {
      cout << "Data overflow cant make subvideo for lane " << lane_number << endl;
      return;
    }
  }

  //Sort to make sure they are in correct order... 
  //maby already in correct order... so no need

  return;
}


//Do tracking using annotations
//Fill the vector call results with structs representing objects for each swimmer in each frame
//This fuction will try to ignore swimmers who are not in the race even when
// they were annotated. The racer should be in vector position 0.
void swimmer_tracking::annotation_tracking(string file_name)
{
  int ii;
  int jj;
  int kk;

  //for predictions
  int hight_step = 0;
  int width_step = 0;
  int x_step = 0;
  int y_step = 0;

  track_data temp;
  vector<swim_data>* frame_data;
  vector<swim_data>* next_frame_data;
  int skip_size = get_skip_size();
  int num_frames = get_num_frames();


  if (load_video_for_boxing(file_name)) {
    destroyWindow(AN_WINDOW_NAME);

    //Create objects
    for (ii = 0; ii < int(num_frames / skip_size); ii++) {

      //Each swimmer, ie. lane number
      for (jj = 0; jj < 10; jj++) {
        
        //ii is a psudo frame number used for boxing and a skip size
        frame_data = get_swim_data(ii, jj);
        next_frame_data = get_swim_data(ii + 1, jj);

        //Calcualtions for box predictions
        //predictions use linear aproximation between annotated frame befor and after
        if ((next_frame_data != nullptr) && (frame_data != nullptr)) {

          hight_step = next_frame_data->at(0).swimmer_box.height - frame_data->at(0).swimmer_box.height;
          width_step = next_frame_data->at(0).swimmer_box.width - frame_data->at(0).swimmer_box.width;
          x_step = next_frame_data->at(0).swimmer_box.x - frame_data->at(0).swimmer_box.x;
          y_step = next_frame_data->at(0).swimmer_box.y - frame_data->at(0).swimmer_box.y;

          hight_step /= (skip_size + 1);
          width_step /= (skip_size + 1);
          x_step /= (skip_size + 1);
          y_step /= (skip_size + 1);

        }

        //To account for frame skipping
        for (kk = 0; kk < skip_size; kk++) {
          if (kk == 0) {
            //When swimmer position does not have to be infered
            if (frame_data != nullptr) {
              temp.frame_num = ii*skip_size;//must traslate frame number to acctual frame
              temp.object_ID = jj;
              temp.class_id = frame_data->at(0).box_class;//swimmer class, see fuction header for the use of index 0
              temp.conf_score = 100;
              temp.frame_pos = frame_data->at(0).swimmer_box;
              //Add to vector
              results.push_back(temp);
            }
          }
          else {
            //When swimmer position must be predicted
            if (next_frame_data != nullptr) {
              temp.frame_num = ii * skip_size + kk;
              //temp.object_ID = jj;
              //temp.class_id = frame_data->at(0).box_class;
              //temp.conf_score = 100;
              temp.frame_pos.height += hight_step;
              temp.frame_pos.width += width_step;
              temp.frame_pos.x += x_step;
              temp.frame_pos.y += y_step;
              results.push_back(temp);
            }
            else if ((ii * skip_size + kk) < num_frames) {
              //Case when there are still frames but no next frame annotations to predict with
              results.push_back(temp);
            }
          }
        }
      }
    }
  }
  else {
    cout << "Could not load video file for annotation tracking!" << endl;
  }

  return;
}


