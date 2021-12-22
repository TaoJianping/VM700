//
// Created by tao on 2021/10/13.
//

#include "ObjClosure.h"

ObjClosure::ObjClosure(ObjFunction *objFunction)
{
    upvalues = std::vector<ObjUpvalue*>(objFunction->upValueCount, nullptr);
    type = ObjType::OBJ_CLOSURE;
    function = objFunction;
}
