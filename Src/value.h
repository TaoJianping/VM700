//
// Created by tjp56 on 2021/4/9.
//

#ifndef VM700_VALUE_H
#define VM700_VALUE_H

#include <vector>

using std::vector;
using Value = double;


struct ValueArray : public vector<Value>
{
	void write(Value value);
};



#endif //VM700_VALUE_H
