#include <nan.h>
#include "core/depth_info.h"

using namespace v8;


void DepthInfo::Init(Handle<Object> exports) {
	NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew("DepthInfo"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Methods added to the object
  NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);

  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew("DepthInfo"), tpl->GetFunction());
}


NAN_METHOD(DepthInfo::New) {
  NanScope();

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new DepthInfo(...)`
    float value = args[0]->IsUndefined() ? 0 : (float)args[0]->NumberValue();
    DepthInfo* obj = new DepthInfo();
    obj->Wrap(args.This());
    NanReturnValue(args.This());
  } else {
    // Invoked as plain function `DepthInfo(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = NanNew<Function>(constructor);
    NanReturnValue(cons->NewInstance(argc, argv));
  }
}


NAN_METHOD(DepthInfo::PlusOne) {
  NanScope();

  DepthInfo* obj = ObjectWrap::Unwrap<DepthInfo>(args.Holder());
  obj->plus_one();

  NanReturnValue(NanNew(obj->value_));
}
