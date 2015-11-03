#include "nan_glob_var.h"

#include <node.h>
#include <cstring>

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

//##########################################################
//##########################################################


// NAN_METHOD(getValidArea){
// 	NanScope();
	// Local<Object> buffer = args[0]->ToObject();
 //  int len = args[1]->Uint32Value();
 //  unsigned char* msg = Buffer::Data(buffer);
 // 	char c = msg[0];

	// int axis = args[0]->
	// printf("%s\n", msg);
  // NanReturnValue(NanNewBufferHandle(msg, len));
  /**

  	TODO:
  	- Choose best valid position to move on
  	- Calculate distance in 3 coord moving from middle of image to valid area (use min_avg_dist to calculate z-axis)
  	- getTargetPosition(int axis) // 1->x 2->y 3->z to read target position (relative) drone should move to

  **/
// 	NanReturnValue(NanNew<Number>(0));
// }


// NAN_METHOD(getDangerArea)
// {
//   NanScope();

//   // Local<Array> ARRAY  = NanNew<Array>(3);
//   // for (int i = 0; i < len; i++)   {
//   //   ARRAY->Set(i, NanNew<Number>(val[i]));
//   // }
//   Local<Object> PARAMS = NanNew<Object>();
//   PARAMS->Set(NanNew<String>("x"), NanNew<Integer>());

//    NanReturnValue(ARRAY);
// }


// NAN_METHOD(setIntendPos){
//  NanScope();
//  String::Utf8Value s(args[0]);
//   string axis = string(*s);
//   unsigned int val = args[1]->Uint32Value();
//   int succ = 0;

//   if (axis.compare(0, 1, "x") == 0){
//    setIntendPos('x', val);
//    succ = 1;
//   }
//   if (axis.compare(0, 1, "y") == 0){
//    setIntendPos('y', val);
//    succ = 1;
//   }
//   if (axis.compare(0, 1, "z") == 0){
//    setIntendPos('z', val);
//    succ = 1;
//   }

//  NanReturnValue(NanNew<Number>(succ));
// }


