//
// Created by tao on 2021/12/1.
//

#ifndef VM700TEST_OBJUPVALUE_H
#define VM700TEST_OBJUPVALUE_H


#include "Object.h"
#include "value.h"

class ObjUpvalue: public Object
{
public:
    std::string toString();
    Value* location;
    ObjUpvalue* next = nullptr;
    Value closed = Value(Nil{});
};


#endif //VM700TEST_OBJUPVALUE_H
