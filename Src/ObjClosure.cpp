//
// Created by tao on 2021/10/13.
//

#include "ObjClosure.h"

ObjClosure::ObjClosure(ObjFunction *objFunction)
{
    type = ObjType::OBJ_CLOSURE;
    function = objFunction;
}
