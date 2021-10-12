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
public:
    int32_t arity;
    Chunk chunk;
    std::string name;

    ObjFunction();
    ~ObjFunction() override;

    Chunk* getChunk();
    const char* _name_();
    void printFunction();
    std::string toString();
};



#endif //VM700TEST_OBJFUNCTION_H
