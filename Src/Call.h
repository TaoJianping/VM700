//
// Created by tao on 2021/9/15.
//

#ifndef VM700TEST_CALL_H
#define VM700TEST_CALL_H


#include "ObjFunction.h"

constexpr const int FRAMES_MAX = 64;

class CallFrame
{
public:
    ObjFunction* function;
    uint8_t* ip;
    Value* slots;
};

class CallStack: public std::vector<CallFrame>
{
public:
    CallFrame* top();
    int32_t frameCount();
    CallFrame* newFrame();

private:
};


#endif //VM700TEST_CALL_H
