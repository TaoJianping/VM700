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

class VmStack : public vector<Value>
{
public:
	void push(Value value);
	Value pop();
	Value top();
	string toString();
};


#endif //VM700_VMSTACK_H
