//
// Created by tjp56 on 2021/4/9.
//

#include "value.h"

void ValueArray::write(Value value)
{
	this->push_back(value);
}

bool Value::isNumber()
{
	return this->index() == static_cast<int32_t>(ValueType::NUMBER);
}

double Value::asNumber()
{
	return std::get<double>(*this);
}

Value::Value(double num): LoxType(num)
{

}

Value::Value(const Value& v): LoxType(v)
{

}

Value::Value(std::monostate num):LoxType(num)
{

}

Value::Value(Object* obj): LoxType(obj)
{

}

ValueType Value::type()
{
	return static_cast<ValueType>(this->index());
}

bool Value::asBool()
{
	return std::get<bool>(*this);
}

bool Value::isNil()
{
	return this->type() == ValueType::NIL;
}

bool Value::isBool()
{
	return this->type() == ValueType::BOOL;
}

string Value::toString()
{
	if (this->isNil())
	{
		return std::string("Nil");
	}
	if (this->isBool())
	{
		if (this->asBool())
		{
			return std::string("true");
		}
		else
		{
			return std::string("false");
		}
	}
	if (this->isNumber())
	{
		std::string s = absl::StrFormat("[ %g ]", this->asNumber());
		return s;
	}

	LOG(ERROR) << "NOT SUPPORT TOSTRING TYPE!!!";
	return "";
}
