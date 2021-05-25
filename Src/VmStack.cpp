//
// Created by tjp56 on 2021/4/10.
//

#include "VmStack.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_format.h"

void VmStack::push(Value value)
{
	this->push_back(value);
}

Value VmStack::pop()
{
	auto value = this->back();
	this->pop_back();
	return value;
}

Value VmStack::top()
{
	return this->back();
}

string VmStack::toString()
{
	using absl::StrFormat;

	string ret {};
	for (auto value : *this)
	{
		if (value.type() == ValueType::NUMBER)
		{
			ret += StrFormat("[ %g ]", value.asNumber());
		}
		else if (value.type() == ValueType::NIL)
		{
			ret += "[ Nil ]";
		}
		else if (value.type() == ValueType::BOOL)
		{
			if (value.asBool())
			{
				ret  += "[ true ]";
			} else {
				ret  += "[ false ]";
			}
		}
	}

	return ret;
}
