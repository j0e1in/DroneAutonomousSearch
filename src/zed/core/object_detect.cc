// OpenCV includes
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/ml/ml.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include "utils.h"

using namespace std;
using namespace cv;
// using namespace cv::gpu;


// cv::gpu::CascadeClassifier_GPU cascade_gpu;
CascadeClassifier classifiers[2];
string window_name = "Object detection";


/** @function main */
int objdetectMain()
{
  int i = 0;
  // Load the cascade classifiers
  for (const string &it : cascade_files){
    if(!classifiers[i].load(it)){
      cerr << "ERR: Error loading classifiers\n";
      return -1;
    };
    i++;
  }

  // Interrupt if no at least 2 classifiers are specified
  if (i < 2){
    cerr << "ERR: two cascade classifiers must be specified.\n";
    return -1;
  }
  return 0;
}

/** @function detectAndDisplay */
bool detectAndDisplay(Mat frame, bool show)
{
  std::vector<Rect> faces;
  Mat frame_gray;
  bool isObjFound = false;

  cvtColor(frame, frame_gray, CV_BGR2GRAY);
  equalizeHist(frame_gray, frame_gray);

  //-- Detect faces
  classifiers[0].detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

  for( size_t i = 0; i < faces.size(); i++ )
  {
    Point center(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2);
    ellipse(frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0);

    Mat faceROI = frame_gray(faces[i]);
    std::vector<Rect> eyes;

    //-- In each face, detect eyes
    classifiers[1].detectMultiScale(faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30));

    for(size_t j = 0; j < eyes.size(); j++)
    {
      isObjFound = true;
      Point center(faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2);
      int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
      circle(frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0);
    }
  }

  if (show) imshow(window_name, frame); // draw the image if show option is true
  return isObjFound;
}

int objdetect_test(const string arg_str){

  parseArgs(arg_str);

  VideoCapture capture;
  Mat frame;

  int i = 0;
  // Load the cascade classifiers
  for (const string &it : cascade_files){
    if(!classifiers[i].load(it)){
      cerr << "ERR: Error loading classifiers\n";
      return -1;
    };
    i++;
  }

  // Interrupt if no at least 2 classifiers are specified
  if (i < 2){
    cerr << "ERR: two cascade classifiers must be specified.\n";
    return -1;
  }

  //-- 2. Read the video stream
  capture.open(0);
  if (!capture.isOpened()) { printf("--(!)Error opening video capture\n"); return -1; }

  while (capture.read(frame))
  {
    if(frame.empty()){
      printf(" --(!) No captured frame -- Break!");
      break;
    }

    //-- 3. Apply the classifiers to the frame
    detectAndDisplay(frame, true);

    int c = waitKey(10);
    if((char)c == 27) { break; } // escape
  }
  return 0;
}