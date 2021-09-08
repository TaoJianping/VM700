//
// Created by tjp56 on 2021/4/9.
//

#ifndef VM700_VALUE_H
#define VM700_VALUE_H

#include <vector>
#include <variant>
#include <cstdint>
#include <string>

#include "Object.h"
#include "absl/strings/str_format.h"
#include "glog/logging.h"

using std::vector;
using std::variant;
using std::string;
using Nil = std::monostate;

enum class ValueType : int32_t
{
	NIL,
	BOOL,
	NUMBER,
	Object,
};

using LoxType = variant<Nil, bool, double, Object*>;

class Value : public LoxType
{
public:
	bool isNumber();
	double asNumber();

	bool isNil();

	bool isBool();
	bool asBool();

	Object* asObject();
	bool isObject();

    Value(double num);
    Value(Nil nil);
	Value(const Value& v);
    Value(Object* obj);
    Value(bool boolean);

	Value() = default;

	ValueType type();
	string toString();
};

struct ValueArray : public vector<Value>
{
	void write(Value value);
};






#endif //VM700_VALUE_H
