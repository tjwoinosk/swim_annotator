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
            rectangle(canvas, buttonStart, Scalar(0, 0, 255), 2);
        }
        else if (buttonStop.contains(Point(x, y)))
        {
            cout << "--STOP button clicked!" << endl;
            rectangle(canvas, buttonStop, Scalar(0, 0, 255), 2);
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
    Mat3b img(400, 400, Vec3b(100, 300, 150));

    // Your button
    buttonStart = Rect(0, 0, img.cols/2, 50);
    buttonStop = Rect(img.cols / 2,0, img.cols/2, 50);

    // The canvas
    canvas = Mat3b(img.rows + buttonStart.height, img.cols, Vec3b(0, 0, 0));
    //vertical height, width, colour

    // Draw the button
    canvas(buttonStart) = Vec3b(200, 200, 200); //Colour
    canvas(buttonStop) = Vec3b(200, 200, 200); //Colour
    //I think the operator () on canvas will take the input (buttonStart) and adjust the matrix at that
    //location (in canvas) to match buttonStart based on the x,y coordinates and height, width of buttonstart

    // Draw the image
    img.copyTo(canvas(Rect(0, buttonStart.height, img.cols, img.rows)));

    //Draw the text on the buttons - Draw last so they show up above all the other drawings
    putText(canvas, buttonTextStart, Point(buttonStart.width * 0.35, buttonStart.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
    putText(canvas, buttonTextStop, Point(buttonStop.width + (buttonStop.width * 0.35), buttonStop.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));

    // Setup callback function
    namedWindow(winName);
    setMouseCallback(winName, callBackFunc,this);

    imshow(winName, canvas);
    waitKey();
}


