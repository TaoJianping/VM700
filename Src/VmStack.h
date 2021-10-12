//
// Created by tjp56 on 2021/4/10.
//

#ifndef VM700_VMSTACK_H
#define VM700_VMSTACK_H

#include <vector>
#include "value.h"
#include <string>

using std::vector;
using std::string;

constexpr const int STACK_MAX = 256;

class VmStack : public vector<Value>
{
public:
    VmStack();
    Value* stackTop;
	void push(const Value& value);
	Value pop();
	[[nodiscard]] Value top() const;
	string toString();
};


#endif //VM700_VMSTACK_H
