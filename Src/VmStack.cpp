//
// Created by tjp56 on 2021/4/10.
//

#include "VmStack.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_format.h"
#include "ObjFunction.h"

void VmStack::push(const Value& value)
{
    *stackTop = value;
    stackTop++;
}

Value VmStack::pop()
{
    stackTop--;
    return *stackTop;
}

Value VmStack::top() const
{
	return *(stackTop - 1);
}

string VmStack::toString()
{
	using absl::StrFormat;

	string ret {};
    for (Value* slot = this->data(); slot < stackTop; slot++)
	{
        auto value = *slot;
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
		else if (value.type() == ValueType::Object)
		{
			auto obj = value.asObject();
			if (obj->type == ObjType::OBJ_STRING)
			{
				auto str = dynamic_cast<ObjString*>(obj);
				ret += StrFormat("[ %s ]", *str);
			} else if (obj->type == ObjType::OBJ_FUNCTION)
            {
                auto str = dynamic_cast<ObjFunction*>(obj);
                ret += StrFormat("[ fn<%s> ]", str->name);
            }
            else
            {
                ret += "[ NOT SUPPORT EXPR ]";
            }
		}
        else
        {
            ret += "[ CANNOT EXPR ]";
        }
	}

	return ret;
}

VmStack::VmStack() : vector<Value>(STACK_MAX)
{
    this->stackTop = this->data();
}
