#ifndef nan_public
#define nan_public

#include <nan.h>

using namespace v8;

NAN_METHOD(has_valid_area);
NAN_METHOD(has_invalid_area);
NAN_METHOD(has_danger_area);
NAN_METHOD(is_obj_detected);
NAN_METHOD(setIsMovingForward);

#endif