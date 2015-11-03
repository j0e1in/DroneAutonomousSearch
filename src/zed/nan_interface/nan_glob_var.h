#ifndef NAN_GLOB_VAR
#define NAN_GLOB_VAR

#include <nan.h>
#include "core/glob_var.h"

using namespace v8;

// NAN_METHOD(getValidArea);
NAN_METHOD(has_valid_area);
NAN_METHOD(has_danger_area);
NAN_METHOD(has_target_obj_found);
NAN_METHOD(setIntendPos);
NAN_METHOD(getTargetPos);

#endif