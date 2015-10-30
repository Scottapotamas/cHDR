
#include <iostream>
#include <stdio.h>

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN 1
    #define NOMINMAX 1
    #include <windows.h>
#endif
#if defined(_WIN64)
    #include <windows.h>
#endif

#if defined(__APPLE__)
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/core/opengl.hpp"
#include "opencv2/core/cuda.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
 
using namespace std;
using namespace cv;






int countCameras()
{
   cv::VideoCapture temp_camera;
   int maxTested = 3;
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

    // cout << "Counting cameras" << endl;
    // cout << countCameras() << endl;

    // VideoCapture cap0(0); // open the default camera
    // if(!cap0.isOpened()) {// check if we succeeded
    //     cout << "Cannot find camera 0" << endl;
    //     return -1;
    // } else {
    //     cout << "Camera 0 has no issue. Setting resolution..." << endl; 
    //     cap0.set(CV_CAP_PROP_FRAME_WIDTH,480);
    //     cap0.set(CV_CAP_PROP_FRAME_HEIGHT,320);
    // }

    // VideoCapture cap1(1); 
    // if(!cap1.isOpened()) {
    //     cout << "Cannot find camera 1" << endl;
    //     return -1;
    // } else {
    //     cout << "Camera 1 has no issue. Setting resolution..." << endl;
    //     cap1.set(CV_CAP_PROP_FRAME_WIDTH,480);
    //     cap1.set(CV_CAP_PROP_FRAME_HEIGHT,320);
    //     //cap1.set(CV_CAP_PROP_FOURCC);
    // }

    // VideoCapture cap2(0); 
    // if(!cap2.isOpened()) {
    //     cout << "Cannot find camera 2" << endl;
    //     return -1;
    // } else {
    //     cout << "Camera 2 has no issue. Setting resolution..." << endl;
    //     cap2.set(CV_CAP_PROP_FRAME_WIDTH,480);
    //     cap2.set(CV_CAP_PROP_FRAME_HEIGHT,320);
    // }    

    VideoCapture cap0("dark.jpg");
    VideoCapture cap1("normal.jpg");
    VideoCapture cap2("light.jpg");


    Mat image1;
    Mat image2;
    Mat image3;
    Mat hdr;
    Mat gamma;

    cout << "About to loop..." << endl;

        cap0 >> image1;
        cap1 >> image2;  
        cap2 >> image3;         
        cap0 >> hdr;
        cap0 >> gamma;  

    while(1)   {


        putText(image1, "RAW INPUT 1",  Point(image1.cols/2 - 100,image1.rows-30), CV_FONT_NORMAL, 1, Scalar(255,255,255),1,1);
        putText(image2, "RAW INPUT 2",  Point(image1.cols/2 - 100,image1.rows-30), CV_FONT_NORMAL, 1, Scalar(255,255,255),1,1);
        putText(image3, "RAW INPUT 3",  Point(image1.cols/2 - 100,image1.rows-30), CV_FONT_NORMAL, 1, Scalar(255,255,255),1,1);
        putText(hdr,    "MERGED",       Point(image1.cols/2 - 100,image1.rows-30), CV_FONT_NORMAL, 1, Scalar(255,255,255),1,1);
        putText(gamma,  "GAMMA",        Point(image1.cols/2 - 100,image1.rows-30), CV_FONT_NORMAL, 1, Scalar(255,255,255),1,1);

        int margin = 5;
        int dstWidth = margin + image1.cols + margin + image2.cols + margin + image3.cols + margin;
        int dstHeight = margin + image1.rows + margin*2 + hdr.rows + margin;

        Mat dst = Mat(dstHeight, dstWidth, CV_8UC3, Scalar(0,0,0));


        Mat targetROI = dst(Rect(margin, margin, image1.cols, image1.rows));
        image1.copyTo(targetROI);

        targetROI = dst(Rect(image1.cols + (margin*2), margin, image2.cols, image2.rows));
        image2.copyTo(targetROI);

        targetROI = dst(Rect(image2.cols + image3.cols + (margin*3), margin, image3.cols, image3.rows));
        image3.copyTo(targetROI);

        targetROI = dst(Rect(margin, image1.rows + margin*2, hdr.cols, hdr.rows));
        GaussianBlur( image1, hdr, Size( 15, 15 ), 0, 0 );
        hdr.copyTo(targetROI);

        
        namedWindow("cHDR Test Window"); // create image window named "My Image"
        imshow("cHDR Test Window", dst); // show the image on window

        if(waitKey(30) >= 0)   //esc key is pressed for 30msec.
            break;
    }

    return 0;
}