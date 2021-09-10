//
// Created by tao on 2021/9/10.
//

#include "ObjFunction.h"

ObjFunction::ObjFunction()
{
    this->type = ObjType::OBJ_FUNCTION;
    this->arity = 0;
    this->name = nullptr;
}

ObjFunction::~ObjFunction() {
    delete this->name;
}

const char *ObjFunction::_name_() {
    return this->name->c_str();
}

Chunk *ObjFunction::getChunk() {
    return &this->chunk;
}
