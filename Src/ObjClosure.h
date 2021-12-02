//
// Created by tao on 2021/10/13.
//

#ifndef VM700TEST_OBJCLOSURE_H
#define VM700TEST_OBJCLOSURE_H


#include <vector>

#include "ObjFunction.h"
#include "ObjUpvalue.h"


class ObjClosure : public Object
{
public:
    explicit ObjClosure(ObjFunction* objFunction);
    ObjFunction* function;
    std::vector<ObjUpvalue*> upvalues;
};


#endif //VM700TEST_OBJCLOSURE_H
