//
// Created by tao on 2021/5/25.
//

#ifndef VM700_OBJECT_H
#define VM700_OBJECT_H

#include "common.h"
#include <string>

using std::string;

enum class ObjType : int32_t
{
	OBJ_STRING,
};

class Object
{
public:
	ObjType type;
	Object* next;

	virtual ~Object();
};

class ObjString: public Object, public string
{
public:
	explicit ObjString(string str);
};

#endif //VM700_OBJECT_H
