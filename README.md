# swim_annotator
Swimming Video Annotator

Methods of creating boxes
A semi automated box making application needs to be made to increase speed of swimmer labeling! I have created a GitHub account and I will be developing the project, swimming_data_maker, on it. Ideas planning will follow in the next section. 
Preliminary research:
After some research I found a function selectROI() that produces a window of an image that can have rectangles dawn on it, the function returns the rectangle object that was dawn in the window. This will be the brut force function of the labeling mode system.  
Box Project Goal
Main goal of box project is to take a raw video downloaded from YouTube and create a video or sets of videos (depends on if there are camera angel changes in the video), with boxes around the swimmers.  
Specifics of what needs to be done
Based on the requirements set beforehand, there will be 4 main modes that allows for the completion of this task.
1) Preprocessing mode 
2) Labelling mode
3) Storage mode
4) Start up mode 

Preprocessing mode
In this mode the raw video will be loaded from a file and added to the program. 

•	The video will be able to be played by the program and reviewed frame by frame. In this step the original video can be trimmed and cut into sub videos if necessary. 
•	This mode can also be used to look at annotated video that has been saved and completed. 
•	The resolution will not be adjusted. 
•	When sub videos are made, they will be labelled by camera angel and order of camera angel, that is if 5 sub videos at camera angel “x” occurred they will be stored as x_1, x_2, … and so on. The program will keep track of each video to be labelled for the next section.
•	When the video cutting and trimming section is complete header files for each sub video will be created containing the specified information from the documentation above.
•	At all times the frame number of the current image will be available. 
•	All videos that have not been finished and packaged for labelling will be saved in a preprocessing folder for later use. 

In this mode the following options will be available
-	Play video from frame x
-	Show annotations, remove annotations
-	Go to next frame
-	Go to last frame
-	Trim video (and remove the excess)
-	Make sub video
-	Mark as finished for labelling
-	Save current progress and exit mode
-	Delete current video
-	Load a video in the prepossessing file

Labelling mode
In this step the trimmed and cut video will be annotated using a variety of methods. 

•	The labelling process will skip every other frame to save space and because there is not much new information from frame to frame.

•	For the annotation of a swimmer, a ROI (region of interest) will be defined, then next fames (this would be frame 3 as we are skipping every other frame) ROI will also be defined. Using these frames, the third frames annotation will be predicted for the same swimmer. This prediction will be confirmed by the user or redefined if it is incorrect. Then the recently confirmed frame and its previous frame will be used to predict the next frame, and so on.
•	A lane number will also be assigned to the swimmer and every frame annotation that corresponds to that swimmer. 
•	Each time an annotation is saved to its corresponding text file and will be updated every time a save current annotation command is executed. 
•	Every video that has not been filed to the final folder will be saved in a labeling mode folder unfinished folder. 

In this mode the following options will be available.
-	Save current annotation and move to next frame. Will create a new annotation if one does not exist or update one that already exists  
-	Modify current annotation 
-	Create better prediction/try harder to make a better annotation?
-	Go back to last frame 
-	Go to next frame
-	Quit creating annotations and save spot in that video. Go to start up mode. 
-	Move to any arbitrary frame
-	Switch annotations to a swimmer in another lane 
-	Load new video
-	Mark as finished for storage

Storage mode 
In the last part of the labelling process the work that was completed is filed correctly into a database, based on what is in the video. 

•	When the name of pool is requested the user will be prompted to first choose from the already available pools before creating a new one to limit pool repeats. 
•	There will be a text file that holds general statistics of the total amount of labelled data in each category in the file system.  
•	At any point if the use quits the information that was entered will be saved in a temporary file 

The following information will be requested
-	Indoor or outdoor pool
-	Number of lanes
-	Pool name
-	SCM or LCM
-	Direction of the dive
-	Gender in race
-	Race name

In this mode the following options will be available
-	Refile a video and its data or finish a file that has not been finished
-	Store a labeling mode file
-	Exit storage mode to start up mode and save current progress 

Start up mode
At start up there will be a main screen that allows the user to move to any one of the other modes. 

In this mode the following options will be available
-	Go to prepossessing mode
-	Go to labelling mode
-	Go to storage mode 
-	Exit application

General Requirements
General design of system and system requirements

•	At all times progress needs to be saved
•	When things are deleted, ask before continuing
•	Things should be able to be edited at any point in this process
•	The application will be shell based but will a single window that allows for video viewing 
•	Any work that is repetitive needs to be optimized for optimal output speed

Code Architecture 
