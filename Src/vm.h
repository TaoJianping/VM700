//
// Created by tjp56 on 2021/4/10.
//

#ifndef VM700_VM_H
#define VM700_VM_H

#include <stack>

#include "chunk.h"
#include "Debug.h"
#include "VmStack.h"


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

	void binaryOp(string op);
	uint8_t readByte();
	Value readConstant();
	InterpretResult run();
	void push(Value value);
	Value pop();
	Value top();
	string readFile(const string& path);
	bool compile(const string& source, Chunk* c);
public:
	InterpretResult interpret(Chunk* chunk);
	InterpretResult interpret(const string& source);
	void execute(int argc, char** argv);
	void repl();
	void runFile(const string& path);
};


#endif //VM700_VM_H
