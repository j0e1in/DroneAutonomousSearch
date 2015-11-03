#include <nan.h>
#include "nan_initzed_async.h"

using namespace v8;

class initZedAsyncWorker : public NanAsyncWorker
{
public:
	initZedAsyncWorker(NanCallback *callback)
		: NanAsyncWorker(callback) {}
	~initZedAsyncWorker() {}

	void Execute()
	{
		int argc = 1;
		char** argv = NULL;
		initZed(argc, argv);
	}

	// void HandleOKCallback(){
	// 	NanScope();

	// 	Local<Value> argv[] = {
	// 		// pass return value here
	// 	};
	// 	int argc = 0;
	// 	callback->Call(argc, argv);
	// }
};

NAN_METHOD(initZed)
{
	NanScope();

	NanCallback *callback = new NanCallback(/*args[1].As<Function>()*/);

	NanAsyncQueueWorker(new initZedAsyncWorker(callback));
  NanReturnUndefined();
}

NAN_METHOD(is_initZed_ready)
{
	NanScope();
  NanReturnValue(NanNew<Number>(is_initZed_ready()));
}