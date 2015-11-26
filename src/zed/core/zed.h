#ifndef ZED_H
#define ZED_H

#include <string>
#include <opencv2/opencv.hpp>

int initZed(const std::string arg_str);
bool is_initZed_ready();

extern cv::Mat cvimg;
extern bool refreshed; // for object detection

#endif /* ZED_H */