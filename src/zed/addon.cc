/**
 *	An interface between c++ and node.
 *	It wraps zed code as a node module.
 */

#include <nan.h>
#include "nan_interface/nan_initzed_async.h"
#include "nan_interface/nan_public.h"
#include "core/depth_info.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;

// APIs provided by this module
void InitAll(Handle<Object> exports) {

  /**
   * nan_initzed.async.h
   */
  exports->Set(NanNew<String>("initZed"),
    NanNew<FunctionTemplate>(initZed)->GetFunction());

  exports->Set(NanNew<String>("is_initZed_ready"),
    NanNew<FunctionTemplate>(is_initZed_ready)->GetFunction());

  /**
   * nan_public.h
   */
  exports->Set(NanNew<String>("has_valid_area"),
    NanNew<FunctionTemplate>(has_valid_area)->GetFunction());

  exports->Set(NanNew<String>("has_danger_area"),
    NanNew<FunctionTemplate>(has_danger_area)->GetFunction());

  exports->Set(NanNew<String>("has_target_obj_found"),
    NanNew<FunctionTemplate>(has_target_obj_found)->GetFunction());

  exports->Set(NanNew<String>("setIntendPos"),
    NanNew<FunctionTemplate>(setIntendPos)->GetFunction());

  exports->Set(NanNew<String>("getTargetPos"),
    NanNew<FunctionTemplate>(getTargetPos)->GetFunction());

  /**
   * depth_info.h
   */
  DepthInfo::Init(exports);

}

NODE_MODULE(zed, InitAll)