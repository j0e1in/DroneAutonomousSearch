/**
 *	An interface between c++ and node.
 *	It wraps zed code as a node module.
 */

#include <nan.h>
#include "nan_interface/nan_async_worker.h"
#include "nan_interface/nan_public.h"
// #include "core/depth_info.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;

// APIs provided by this module
void InitAll(Handle<Object> exports) {

  /**
   * nan_objdetect_async.h
   */
  exports->Set(NanNew<String>("objdetect"),
    NanNew<FunctionTemplate>(obj_detect)->GetFunction());

  /**
   * nan_initzed_async.h
   */
  exports->Set(NanNew<String>("initZed"),
    NanNew<FunctionTemplate>(init_Zed)->GetFunction());

  exports->Set(NanNew<String>("is_initZed_ready"),
    NanNew<FunctionTemplate>(is_initZed_ready)->GetFunction());

  /**
   * nan_public.h
   */
  exports->Set(NanNew<String>("has_valid_area"),
    NanNew<FunctionTemplate>(has_valid_area)->GetFunction());

  exports->Set(NanNew<String>("has_invalid_area"),
    NanNew<FunctionTemplate>(has_invalid_area)->GetFunction());

  exports->Set(NanNew<String>("has_danger_area"),
    NanNew<FunctionTemplate>(has_danger_area)->GetFunction());

  exports->Set(NanNew<String>("is_obj_detected"),
    NanNew<FunctionTemplate>(is_obj_detected)->GetFunction());

  exports->Set(NanNew<String>("setIsMovingForward"),
    NanNew<FunctionTemplate>(setIsMovingForward)->GetFunction());

  // exports->Set(NanNew<String>("setIntendPos"),
  //   NanNew<FunctionTemplate>(setIntendPos)->GetFunction());

  // exports->Set(NanNew<String>("getTargetPos"),
  //   NanNew<FunctionTemplate>(getTargetPos)->GetFunction());

  /**
   * depth_info.h
   */
  // DepthInfo::Init(exports);

}

NODE_MODULE(zed, InitAll)