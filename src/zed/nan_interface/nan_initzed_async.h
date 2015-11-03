#ifndef NAN_ASYNC_WORKER_H
#define NAN_ASYNC_WORKER_H

#include "core/zed.h"
#include <nan.h>

using namespace v8;

NAN_METHOD(initZed);
NAN_METHOD(is_initZed_ready);

#endif