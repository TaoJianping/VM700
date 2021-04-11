//
// Created by tjp56 on 2021/4/9.
//

#include "Debug.h"
#include "glog/logging.h"
#include "absl/strings/str_format.h"

using absl::StrFormat;


void Debug::disassembleChunk(Chunk* chunk, const char* name)
{
	LOG(INFO) << absl::StrFormat("== %s ==\n", name);
	for (auto i = 0; i < chunk->size(); )
	{
		i = this->disassembleInstruction(chunk, i);
	}
	LOG(INFO) << "==============\n";
}

size_t Debug::disassembleInstruction(Chunk* chunk, size_t offset)
{
	string byteOffset = absl::StrFormat("%04d ", offset);
	string opCodeName {};
	string lineNumber {};

	if (offset > 0 && (chunk->lines[offset] == chunk->lines[offset - 1]))
	{
		lineNumber = "   | ";
	}
	else
	{
		lineNumber = StrFormat("%4d ", chunk->lines[offset]);
	}

	auto instruction = static_cast<OpCode>( chunk->at(offset));
	switch (instruction)
	{
	case OpCode::OP_ADD:
	{
		offset = this->simpleInstruction("OP_ADD", offset, opCodeName);
		break;
	}
	case OpCode::OP_SUBTRACT:
	{
		offset = this->simpleInstruction("OP_SUBTRACT", offset, opCodeName);
		break;
	}
	case OpCode::OP_MULTIPLY:
	{
		offset = this->simpleInstruction("OP_MULTIPLY", offset, opCodeName);
		break;
	}
	case OpCode::OP_DIVIDE:
	{
		offset = this->simpleInstruction("OP_DIVIDE", offset, opCodeName);
		break;
	}
	case OpCode::OP_NEGATE:
	{
		offset = this->simpleInstruction("OP_NEGATE", offset, opCodeName);
		break;
	}
	case OpCode::OP_RETURN:
	{
		offset = this->simpleInstruction("OP_RETURN", offset, opCodeName);
		break;
	}
	case OpCode::OP_CONSTANT:
	{
		offset = this->constantInstruction("OP_CONSTANT", chunk, offset, opCodeName);
		break;
	}
	default:
	{
		LOG(ERROR) << StrFormat("Unknown opcode %d\n", static_cast<uint8_t>(instruction));
		offset = offset + 1;
	}
	}

	LOG(INFO) << StrFormat("%s %s %s", byteOffset, lineNumber, opCodeName);
	return offset;
}

size_t Debug::simpleInstruction(const char* name, size_t offset, string& opCodeName)
{
	opCodeName = name;
	return offset + 1;
}

size_t Debug::constantInstruction(const char* name, Chunk* chunk, size_t offset, string& opCodeName)
{
	uint8_t constant = chunk->at(offset + 1);
	opCodeName = StrFormat("%-16s %4d '%s", name, constant, this->printValue(chunk->constants.at(constant)));
	return offset + 2;
}

string Debug::printValue(Value value)
{
	return StrFormat("%g", value);
}