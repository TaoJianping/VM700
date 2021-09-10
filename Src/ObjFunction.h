//
// Created by tao on 2021/9/10.
//

#ifndef VM700TEST_OBJFUNCTION_H
#define VM700TEST_OBJFUNCTION_H

#include "chunk.h"
#include "Object.h"
#include <string>

using std::string;

enum class FunctionType
{
    TYPE_FUNCTION,
    TYPE_SCRIPT
};

class ObjFunction: public Object
{
private:
    int32_t arity;
    Chunk chunk;
    ObjString* name;

public:
    ObjFunction();
    ~ObjFunction() override;

    Chunk* getChunk();
    const char* _name_();
};



#endif //VM700TEST_OBJFUNCTION_H
