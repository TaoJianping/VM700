//
// Created by tao on 2021/10/9.
//

#include "ObjNative.h"

#include <utility>

std::string ObjNative::toString() {
    return "<native fn>";
}

ObjNative::ObjNative(NativeFn function)
{
    this->function = std::move(function);
    this->type = ObjType::OBJ_NATIVE;
}
