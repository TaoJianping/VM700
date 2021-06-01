//
// Created by tao on 2021/5/25.
//

#include "Object.h"

#include <utility>

ObjString::ObjString(string str): Object(), string(std::move(str))
{
	this->type = ObjType::OBJ_STRING;
}
