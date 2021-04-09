//
// Created by tao on 2021/4/9.
//

#ifndef VM700_CHUNK_H
#define VM700_CHUNK_H

#include "common.h"


enum class OpCode
{
	OP_RETURN
};

struct Chunk
{
	int count;
	int capacity;
	uint8_t* code;
};


#endif //VM700_CHUNK_H
