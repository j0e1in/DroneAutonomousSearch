#include "nan_public.h"

#include <node.h>
// #include <cstring>

using namespace std;
using namespace v8;
using namespace node;

NAN_METHOD(has_valid_area){
	NanScope();
	NanReturnValue(NanNew<Number>(valid_areas.num_area));
}

NAN_METHOD(has_danger_area)
{
  NanScope();
  NanReturnValue(NanNew<Number>(danger_areas.num_area));
}

NAN_METHOD(has_target_obj_found)
{
  NanScope();
  NanReturnValue(NanNew<Number>(has_target_obj_found()));
}

NAN_METHOD(setIntendPos)
{
  NanScope();

  intend_pos._x = args[0]->Uint32Value();
  intend_pos._y = args[1]->Uint32Value();
  intend_pos._z = args[2]->Uint32Value();

  NanReturnUndefined();
}

NAN_METHOD(getTargetPos)
{
  NanScope();

  Local<Object> DICT = NanNew<Object>();
  DICT->Set(NanNew<String>("x"), NanNew<Number>(target_pos._x));
  DICT->Set(NanNew<String>("y"), NanNew<Number>(target_pos._y));
  DICT->Set(NanNew<String>("z"), NanNew<Number>(target_pos._z));

  // Local<Array> ARRAY  = NanNew<Array>(3);
  // ARRAY->Set(0, NanNew<Number>(target_pos._x));
  // ARRAY->Set(1, NanNew<Number>(target_pos._y));
  // ARRAY->Set(2, NanNew<Number>(target_pos._z));

  NanReturnValue(DICT);
}
