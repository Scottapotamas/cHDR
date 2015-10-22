#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
 
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int countCameras()
{
   cv::VideoCapture temp_camera;
   int maxTested = 10;
   for (int i = 0; i < maxTested; i++){
     cv::VideoCapture temp_camera(i);
     bool res = (!temp_camera.isOpened());
     temp_camera.release();
     if (res)
     {
       return i;
     }
   }
   return maxTested;
}

int main( int argc, const char** argv )
{

    cout << "Counting cameras" << endl;
    cout << countCameras() << endl;


    VideoCapture cap0(0); // open the default camera
    if(!cap0.isOpened()) {// check if we succeeded
        cout << "Cannot find camera 0" << endl;
        return -1;
    } else {
        cout << "Camera 0 has no issue" << endl;
    }

    VideoCapture cap1(1); 
    if(!cap1.isOpened()) {
        cout << "Cannot find camera 1" << endl;
        return -1;
    } else {
        cout << "Camera 1 has no issue" << endl;
    }
    // VideoCapture cap2(0); 
    // if(!cap2.isOpened()) {
    //     cout << "Cannot find camera 2" << endl;
    //     return -1;
    // } else {
    //     cout << "Camera 2 has no issue" << endl;
    // }    


    Mat edges;
    namedWindow("edges",1);
     
    cout << "About to loop..." << endl;

   while(1)
   {
    Mat frame0;
    cap0 >> frame0; 
    Mat frame1;
    cap1 >> frame1; 

    //imshow("webcam", frame0);
    imshow("webcam1", frame1);

     if(waitKey(30) >= 0)   //esc key is pressed for 30 seconds.
        break;
    
    }
}

