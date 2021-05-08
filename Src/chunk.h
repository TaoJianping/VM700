//
// Created by tao on 2021/4/9.
//

#ifndef VM700_CHUNK_H
#define VM700_CHUNK_H

#include "common.h"
#include <vector>
#include "value.h"

using std::vector;


enum class OpCode : uint8_t
{
	OP_CONSTANT,
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NOT,
	OP_NEGATE,
	OP_RETURN,
};

struct Chunk : public vector<uint8_t>
{
	ValueArray constants {};
	vector<size_t> lines {};

	void write(uint8_t byte, size_t line);
	void write(OpCode instruction, size_t line);
	size_t addConstant(Value value);
};


#endif //VM700_CHUNK_H
