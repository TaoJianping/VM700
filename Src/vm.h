//
// Created by tjp56 on 2021/4/10.
//

#ifndef VM700_VM_H
#define VM700_VM_H

#include <stack>
#include <map>

#include "chunk.h"
#include "Debug.h"
#include "VmStack.h"
#include "Call.h"
#include "ObjNative.h"

using std::map;

static Value clockNative(int argCount, Value* args) {
    return Value((double)clock() / CLOCKS_PER_SEC);
}

enum class InterpretResult
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
};

class vm
{
private:
	Debug debugger {};

    CallStack frames {};
	VmStack vmStack {};
    Object* objects = nullptr;

	map<ObjString, Value> globals;

	InterpretResult binaryOp(const string& op);
	InterpretResult run();
	void push(const Value& value);
	Value pop();
	Value top();
	Value peek(int distance);

	bool isFalsey(Value value);
	bool valuesEqual(Value a, Value b);

	string readFile(const string& path);
	ObjFunction* compile(const string& source);
	void runtimeError(const char* format, ...);
	void addObject(Object* object);
    void addObjects(Object* obj);
    void freeObjects();
	void freeObject(Object* obj);
	bool callValue(Value callee, int32_t argCount);
	bool call(ObjFunction* function, int32_t argCount);
    void defineNative(const char* name, NativeFn function);

public:
	InterpretResult interpret(const string& source);
	void execute(int argc, char** argv);
	void repl();
	void runFile(const string& path);
	~vm();
};


#endif //VM700_VM_H
