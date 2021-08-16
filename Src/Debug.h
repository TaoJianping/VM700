//
// Created by tjp56 on 2021/4/9.
//

#ifndef VM700_DEBUG_H
#define VM700_DEBUG_H


#include <string>
#include "chunk.h"
using std::string;

class Debug
{
public:
	void disassembleChunk(Chunk* chunk, const char* name);
	size_t disassembleInstruction(Chunk* chunk, size_t offset);
	static size_t simpleInstruction(const char* name, size_t offset, string& opCodeName);
	size_t constantInstruction(const char* name, Chunk* chunk, size_t offset, string& opCodeName);
	size_t byteInstruction(const char* name, Chunk* chunk, size_t offset, string& opCodeName);
	string printValue(Value value);
};


#endif //VM700_DEBUG_H
