#include <nan.h>
#include "depth_info.h"

using namespace v8;

Persistent<Function> DepthInfo::constructor;


DepthInfo::DepthInfo() {
	value_ = 0;

	if (this->image_classify.area_grid != NULL)
		free(this->image_classify.area_grid);

	this->image_classify.area_grid = valid_classfied.area_grid;
}

DepthInfo::~DepthInfo() {
}


void DepthInfo::plus_one()
{
	this->value_ += 1;
}




