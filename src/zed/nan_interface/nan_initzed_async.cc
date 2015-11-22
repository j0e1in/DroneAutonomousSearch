#include <iostream>

//For argument parsing
#include <string>
#include <vector>
#include <sstream>

#include <nan.h>
#include "nan_initzed_async.h"

using namespace v8;
using namespace std;

class initZedAsyncWorker : public NanAsyncWorker
{
public:
	initZedAsyncWorker(string args_str, NanCallback *callback)
		: NanAsyncWorker(callback), args_str(args_str) {}
	~initZedAsyncWorker() {}

	void Execute()
	{
		if (initZed(args_str) < 0){ // main method
			cerr << "ERR: Initializing ZED failed.";
			exit(0);
		}
	}

	// void HandleOKCallback(){
	// 	NanScope();

	// 	Local<Value> argv[] = {
	// 		// pass return value here
	// 	};
	// 	int argc = 0;
	// 	callback->Call(argc, argv);
	// }
private:
	string args_str; // arguments for the program
};



NAN_METHOD(initZed)
{
	NanScope();

	//parameters passed from nodejs
	string arg_str(*NanAsciiString(args[0]));
	/**
	 * Another method:
	 * 	v8::String::Utf8Value tmp_var(args[0]->ToString());
	 * 	string foo = string(*tmp_var);
	 */

	NanCallback *callback = new NanCallback(/*args[1].As<Function>()*/);
	NanAsyncQueueWorker(new initZedAsyncWorker(arg_str, callback)); // new a worker instance here
  NanReturnUndefined();
}

NAN_METHOD(is_initZed_ready)
{
	NanScope();
  NanReturnValue(NanNew<Number>(is_initZed_ready()));
}