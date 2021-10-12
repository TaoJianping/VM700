//
// Created by tao on 2021/5/25.
//
#pragma once
#ifndef VM700_OBJECT_H
#define VM700_OBJECT_H

#include <string>

using std::string;

enum class ObjType : int32_t
{
    OBJ_FUNCTION,
    OBJ_NATIVE,
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
