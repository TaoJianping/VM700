//
// Created by tjp56 on 2021/4/9.
//

#include "chunk.h"


void Chunk::write(uint8_t byte, size_t line)
{
	this->lines.push_back(line);
	this->push_back(byte);
}

void Chunk::write(OpCode instruction, size_t line)
{
	this->lines.push_back(line);
	this->push_back(static_cast<uint8_t>(instruction));
}

size_t Chunk::addConstant(Value value)
{
	this->constants.push_back(value);
	return this->constants.size() - 1;
}