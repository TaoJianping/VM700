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
	case OpCode::OP_NOT:
	{
		offset = this->simpleInstruction("OP_NOT", offset, opCodeName);
		break;
	}
	case OpCode::OP_NEGATE:
	{
		offset = this->simpleInstruction("OP_NEGATE", offset, opCodeName);
		break;
	}
	case OpCode::OP_PRINT:
	{
		offset = this->simpleInstruction("OP_PRINT", offset, opCodeName);
		break;
	}
    case OpCode::OP_JUMP:
    {
        offset = jumpInstruction("OP_JUMP", chunk,offset, 1, opCodeName);
        break;
    }
    case OpCode::OP_JUMP_IF_FALSE:
    {
        offset = jumpInstruction("OP_JUMP_IF_FALSE", chunk, offset, 1, opCodeName);
        break;
    }
    case OpCode::OP_LOOP:
    {
        offset = jumpInstruction("OP_LOOP", chunk, offset, -1, opCodeName);
        break;
    }
	case OpCode::OP_RETURN:
	{
		offset = this->simpleInstruction("OP_RETURN", offset, opCodeName);
		break;
	}
	case OpCode::OP_NIL:
	{
		offset = this->simpleInstruction("OP_NIL", offset, opCodeName);
		break;
	}
	case OpCode::OP_TRUE:
	{
		offset = this->simpleInstruction("OP_TRUE", offset, opCodeName);
		break;
	}
	case OpCode::OP_FALSE:
	{
		offset = this->simpleInstruction("OP_FALSE", offset, opCodeName);
		break;
	}
	case OpCode::OP_POP:
	{
		offset = this->simpleInstruction("OP_POP", offset, opCodeName);
		break;
	}
	case OpCode::OP_GET_LOCAL:
	{
		offset = this->byteInstruction("OP_GET_LOCAL", chunk, offset, opCodeName);
		break;
	}
	case OpCode::OP_SET_LOCAL:
	{
		offset = this->byteInstruction("OP_SET_LOCAL", chunk, offset, opCodeName);
		break;
	}
	case OpCode::OP_GET_GLOBAL:
	{
		offset = this->constantInstruction("OP_GET_GLOBAL", chunk, offset, opCodeName);
		break;
	}
	case OpCode::OP_DEFINE_GLOBAL:
	{
		offset = this->constantInstruction("OP_DEFINE_GLOBAL", chunk, offset, opCodeName);
		break;
	}
	case OpCode::OP_SET_GLOBAL:
	{
		offset = this->constantInstruction("OP_SET_GLOBAL", chunk, offset, opCodeName);
		break;
	}
	case OpCode::OP_CONSTANT:
	{
		offset = this->constantInstruction("OP_CONSTANT", chunk, offset, opCodeName);
		break;
	}
	case OpCode::OP_EQUAL:
	{
		offset = this->simpleInstruction("OP_EQUAL", offset, opCodeName);
		break;
	}
	case OpCode::OP_GREATER:
	{
		offset = this->simpleInstruction("OP_GREATER", offset, opCodeName);
		break;
	}
	case OpCode::OP_LESS:
	{
		offset = this->simpleInstruction("OP_LESS", offset, opCodeName);
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
	return value.toString();
}

size_t Debug::byteInstruction(const char* name, Chunk* chunk, size_t offset, string& opCodeName)
{
	uint8_t slot = chunk->at(offset + 1);
	opCodeName = StrFormat("%-16s %4d\n", name, slot);
	return offset + 2;
}

size_t Debug::jumpInstruction(const char *name, Chunk *chunk, size_t offset, int sign, string &opCodeName) {
    auto jump = (uint16_t)(chunk->at(offset + 1) << 8);
    jump |= chunk->at(offset + 2);
    opCodeName =  StrFormat("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}
