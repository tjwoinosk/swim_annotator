#include "buttonGUI.h"

buttonGUI::buttonGUI()
{
}

buttonGUI::~buttonGUI()
{
}

void buttonGUI::callBackFunc(int event, int x, int y, int, void* userdata)
{
    buttonGUI* buttongui = reinterpret_cast<buttonGUI*>(userdata);
    buttongui->secondCall(event, x, y);
}

void buttonGUI::secondCall(int event, int x, int y)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        if (buttonStart.contains(Point(x, y)))
        {
            cout << "**START button clicked!" << endl;
            rectangle(canvas(buttonStart), buttonStart, Scalar(0, 0, 255), 2);
        }
        else if (buttonStop.contains(Point(x, y)))
        {
            cout << "--STOP button clicked!" << endl;
            rectangle(canvas(buttonStop), buttonStop, Scalar(0, 0, 255), 2);
        }
    }
    if (event == EVENT_LBUTTONUP)
    {
        rectangle(canvas, buttonStart, Scalar(200, 200, 200), 2);
        rectangle(canvas, buttonStop, Scalar(200, 200, 200), 2);
    }

    imshow(winName, canvas);
    waitKey(1);
}

void buttonGUI::runButton()
{
    // An image
    Mat3b img(400, 400, Vec3b(100, 200, 150));

    // Your button
    buttonStart = Rect(0, 0, img.cols/2, 50);
    buttonStop = Rect(img.cols / 2,0, img.cols/2, 50);

    // The canvas
    canvas = Mat3b(img.rows + buttonStart.height, img.cols, Vec3b(0, 0, 0));

    // Draw the button
    canvas(buttonStart) = Vec3b(200, 200, 200); //Colour
    putText(canvas(buttonStart), buttonTextStart, Point(buttonStart.width * 0.35, buttonStart.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));

    canvas(buttonStop) = Vec3b(200, 200, 200); //Colour
    putText(canvas(buttonStop), buttonTextStop, Point(buttonStop.width  + (buttonStop.width * 0.35), buttonStop.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));

    // Draw the image
    img.copyTo(canvas(Rect(0, buttonStart.height, img.cols, img.rows)));

    // Setup callback function
    namedWindow(winName);
    setMouseCallback(winName, callBackFunc, this);

    imshow(winName, canvas);
    waitKey();
}

void buttonGUI::runButtonTWO()
{
    // An image
    Mat3b img(400, 400, Vec3b(100, 200, 150));
    //vertical height, width, colour

    // Your button
    buttonStart = Rect(0, 0, 180, 50);
    buttonStop = Rect(0, 60, 180, 50);

    // The canvas - what everything is placed on
    canvas = Mat3b(img.rows + buttonStart.height*2, img.cols, Vec3b(3, 10, 50));
    //vertical height, width, colour

    // Draw the button
    canvas(buttonStart) = Vec3b(200, 200, 200); //Colour
    //I think the operator () on canvas will take the input (buttonStart) and adjust the matrix at that
    //location (in canvas) to match buttonStart based on the x,y coordinates and height, width of buttonstart
    //putText(canvas(buttonStart), buttonTextStart, Point(100, 40), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 3);
    putText(canvas, buttonTextStart, Point(100, 40), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 3);

    canvas(buttonStop) = Vec3b(120, 240, 100); //Colour
    ////putText(canvas(buttonStop), buttonTextStop, Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 3);

    ////Testing:
    //putText(canvas, "JUST TRYING THINGS", Point(200, 200), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 3);

    // Draw the image
    img.copyTo(canvas(Rect(0, buttonStart.height, img.cols, img.rows)));
    putText(canvas, buttonTextStop, Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 3);

    // Setup callback function
    namedWindow(winName);
    setMouseCallback(winName, callBackFunc, this);

    imshow(winName, canvas);
    waitKey();
    
}

void buttonGUI::runButtonThree()
{
    Mat3b img(400, 400, Vec3b(100, 200, 150));
    canvas = Mat3b(img.rows + 10, img.cols + 10, Vec3b(3, 10, 50));

   // putText(canvas, "JUST TRYING THINGS", Point(200, 200), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 3);
    //In the above you won't see it because the image is drawn over it...

    img.copyTo(canvas(Rect(0, buttonStart.height, img.cols, img.rows)));
    
    putText(canvas, "JUST TRYING THINGS", Point(200, 200), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 3);

    namedWindow(winName);
    imshow(winName, canvas);
    waitKey();
}


