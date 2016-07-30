#ifndef OBJDETECT_H
#define OBJDETECT_H

int objdetectMain();
bool detectAndDisplay(cv::Mat frame, bool show);
int objdetect_test(const std::string arg_str);

#endif