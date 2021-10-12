//
// Created by tao on 2021/9/10.
//

#include "ObjFunction.h"

ObjFunction::ObjFunction()
{
    this->type = ObjType::OBJ_FUNCTION;
    this->arity = 0;
    this->name = std::string{};
}

ObjFunction::~ObjFunction() {

}

const char *ObjFunction::_name_() {
    return this->name.c_str();
}

Chunk *ObjFunction::getChunk() {
    return &this->chunk;
}

void ObjFunction::printFunction() {
    if (this->name.empty()) {
        printf("<script>");
        return;
    }
    printf("<fn %s>", this->name.data());
}

std::string ObjFunction::toString() {
    if (this->name.empty()) {
        return "<script>";
    }
    return absl::StrFormat("<fn %s>", this->name.data());
}
