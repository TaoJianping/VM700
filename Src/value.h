//
// Created by tjp56 on 2021/4/9.
//

#ifndef VM700_VALUE_H
#define VM700_VALUE_H

#include <vector>
#include <variant>
#include <cstdint>

using std::vector;
using std::variant;

enum class ValueType : int32_t
{
	NIL,
	BOOL,
	NUMBER,
};

class Value : public variant<std::monostate, bool, double>
{
public:
	bool isNumber();
	double asNumber();

	bool isNil();

	bool isBool();
	bool asBool();

	Value(double num);
	Value(std::monostate nil);
	Value(const Value& v);

	ValueType type();
};

struct ValueArray : public vector<Value>
{
	void write(Value value);
};






#endif //VM700_VALUE_H
