// OpenCV includes
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/ml/ml.hpp"

#include <iostream>
#include <thread>
#include <chrono>

// #include "zed.h"

using namespace std;
using namespace cv;
// using namespace cv::gpu;


/** Global variables */
String face_cascade_name = "D:\\Graduation Project\\DroneSearch\\src\\haarcascades\\haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "D:\\Graduation Project\\DroneSearch\\src\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
// cv::gpu::CascadeClassifier_GPU cascade_gpu;
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Object detection";
RNG rng(12345);


/** @function main */
int objdetectMain()
{
  // Load the cascade classifiers
  if( !face_cascade.load( face_cascade_name ) ){ cerr << "(!)Error loading face classifier\n"; return -1; };
  if( !eyes_cascade.load( eyes_cascade_name ) ){ cerr << "(!)Error loading eyes classifier\n"; return -1; };

  return 0;
}

/** @function detectAndDisplay */
bool detectAndDisplay(Mat frame, bool show)
{
  std::vector<Rect> faces;
  Mat frame_gray;

  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  //-- Detect faces
  face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

  for( size_t i = 0; i < faces.size(); i++ )
  {
    Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
    ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

    Mat faceROI = frame_gray( faces[i] );
    std::vector<Rect> eyes;

    //-- In each face, detect eyes
    eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    for( size_t j = 0; j < eyes.size(); j++ )
    {
      cout << "face & eyes found\n";
      return true;
      Point center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
      int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
      circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
    }
  }

  if (show) imshow(window_name, frame); // draw the image if show option is true
 }