#define  _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <direct.h>
#include "fstream"
#include "iostream"
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/core/gpumat.hpp"
#include "opencv2/core/opengl_interop.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;
using namespace cv::gpu;

cv::gpu::CascadeClassifier_GPU cascade_gpu;

//-------------------------------------------------------------------------------------------------------------
vector<Rect> detect_faces(Mat& image)
{
  vector<Rect> res;
  bool findLargestObject = true;
  bool filterRects = true;
  int detections_num;
  Mat faces_downloaded;
  Mat im(image.size(),CV_8UC1);
  GpuMat facesBuf_gpu;
  if(image.channels()==3)
  {
    cvtColor(image,im,CV_BGR2GRAY);
  }
  else
  {
    image.copyTo(im);
  }
  GpuMat gray_gpu(im);

  cascade_gpu.visualizeInPlace = false;
  cascade_gpu.findLargestObject = findLargestObject;
  detections_num = cascade_gpu.detectMultiScale(gray_gpu, facesBuf_gpu, 1.2,(filterRects || findLargestObject) ? 4 : 0,Size(image.cols/4,image.rows/4));


  if(detections_num==0){return res;}

  facesBuf_gpu.colRange(0, detections_num).download(faces_downloaded);
  Rect *faceRects = faces_downloaded.ptr<Rect>();

  for(int i=0;i<detections_num;i++)
  {
    res.push_back(faceRects[i]);
  }
  gray_gpu.release();
  facesBuf_gpu.release();
  return res;
}
//-----------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------
// MAIN
//----------------------------------------------------------------------
int main(int argc, char * argv[])
{
  cv::gpu::printShortCudaDeviceInfo(cv::gpu::getDevice());
  cascade_gpu.load("haarcascade_frontalface_alt2.xml");
  Mat frame,img;
  namedWindow("frame");
  VideoCapture capture(0);
  capture >> frame;
  vector<Rect> rects;
  if (capture.isOpened())
  {
    while(waitKey(20)!=27) // Exit by escape press
    {
      capture >> frame;
      cvtColor(frame,img,CV_BGR2GRAY);
      rects=detect_faces(img);
      if(rects.size()>0)
      {
        cv::rectangle(frame,rects[0],CV_RGB(255,0,0));
      }
      imshow("frame",frame);
    }
  }

  return 0;
}