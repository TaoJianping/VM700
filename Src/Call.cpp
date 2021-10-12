//
// Created by tao on 2021/9/15.
//

#include "Call.h"

CallFrame* CallStack::top() {
    return &this->back();
}

int32_t CallStack::frameCount() {
    return this->size();
}

CallFrame *CallStack::newFrame() {
    this->push_back(CallFrame{});
    return this->top();
}
