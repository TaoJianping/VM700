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

Value::Value(double num): variant<std::monostate, bool, double>(num)
{

}

Value::Value(const Value& v): variant<std::monostate, bool, double>(v)
{

}

Value::Value(std::monostate num)
{
	*this = std::monostate();
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
