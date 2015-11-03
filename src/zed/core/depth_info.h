#ifndef DEPTH_INFO_H
#define DEPTH_INFO_H

#include <nan.h>
#include "utils.h"

using namespace v8;

class DepthInfo : public node::ObjectWrap {

public:

  static void Init(Handle<Object> exports);

  // Functions does the real work
	void plus_one();


private:

  static Persistent<Function> constructor;

  /*explicit*/ DepthInfo();
  ~DepthInfo();

  // Nan interface, calling methods above
  static NAN_METHOD(New);
  static NAN_METHOD(PlusOne);
  static NAN_METHOD(get_image_classify);

  // Private variables
  float value_;
  area_struct_t image_classify;
};


#endif