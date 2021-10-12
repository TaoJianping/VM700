//
// Created by tao on 2021/10/9.
//

#ifndef VM700TEST_OBJNATIVE_H
#define VM700TEST_OBJNATIVE_H
#include "chunk.h"
#include "Object.h"

//typedef Value (*NativeFn)(int argCount, Value* args);
using NativeFn = std::function<Value(int argCount, Value* args)>;

class ObjNative: public Object
{
public:
    explicit ObjNative(NativeFn function);
    NativeFn function;
    std::string toString();
};


#endif //VM700TEST_OBJNATIVE_H
