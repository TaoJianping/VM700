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

using std::map;

enum class InterpretResult
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
};


class vm
{
private:
	Chunk* chunk = nullptr;
	size_t ip;
	Debug debugger {};
	VmStack _stack{};
	map<ObjString, Value> globals;

	InterpretResult binaryOp(const string& op);
	uint8_t readByte();
	Value readConstant();
	InterpretResult run();
	void push(const Value& value);
	Value pop();
	Value top();
	Value peek(int distance);

	void addObject(Object* object);
	bool isFalsey(Value value);
	bool valuesEqual(Value a, Value b);

	string readFile(const string& path);
	bool compile(const string& source, Chunk* c);
	void runtimeError(const char* format, ...);
	void freeObjects();
	void freeObject(Object* obj);

public:
	InterpretResult interpret(Chunk* chunk);
	InterpretResult interpret(const string& source);
	void execute(int argc, char** argv);
	void repl();
	void runFile(const string& path);
	~vm();
};


#endif //VM700_VM_H
