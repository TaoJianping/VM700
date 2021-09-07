//
// Created by tjp56 on 2021/4/10.
//

#include <fstream>
#include "vm.h"
#include "glog/logging.h"
#include "absl/strings/str_format.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "chunk.h"
#include "Compiler.h"

using absl::StrFormat;


InterpretResult vm::interpret(Chunk* c)
{
	this->chunk = c;
	this->ip = 0;
	return this->run();
}

InterpretResult vm::run()
{
	for (;;)
	{
#ifdef DEBUG_TRACE_EXECUTION
		LOG(INFO) << StrFormat("              %s", this->_stack.toString());
		this->debugger.disassembleInstruction(this->chunk, this->ip);
#endif
		uint8_t instruction = this->readByte();
		switch (static_cast<OpCode>(instruction))
		{
		case OpCode::OP_ADD:
		{
			this->binaryOp("+");
			break;
		}
		case OpCode::OP_SUBTRACT:
		{
			this->binaryOp("-");
			break;
		}
		case OpCode::OP_MULTIPLY:
		{
			this->binaryOp("*");
			break;
		}
		case OpCode::OP_DIVIDE:
		{
			this->binaryOp("/");
			break;
		}
		case OpCode::OP_NOT:
		{
			this->push(this->isFalsey(this->pop()));
			break;
		}
		case OpCode::OP_NEGATE:
		{
			if (!this->peek(0).isNumber())
			{
				this->runtimeError("Operand must be a number.");
				return InterpretResult::INTERPRET_RUNTIME_ERROR;
			}
			// Todo
			this->push(this->pop());
			break;
		}
		case OpCode::OP_PRINT: {
			auto ret = this->pop();
			LOG(INFO) << ret.toString() << "\n";
			break;
		}
        case OpCode::OP_JUMP:
        {
            uint16_t offset = this->readShort();
            this->ip += offset;
            break;
        }
        case OpCode::OP_JUMP_IF_FALSE:
        {
            uint16_t offset = this->readShort();
            if (this->isFalsey(this->peek(0)))
            {
                this->ip += offset;
            }
            break;
        }
        case OpCode::OP_LOOP:
        {
            uint16_t offset = this->readShort();
            this->ip -= offset;
            break;
        }
		case OpCode::OP_RETURN:
		{
//			auto ret = this->pop();
//			LOG(INFO) << "RETURN -> " << ret.toString();
			return InterpretResult::INTERPRET_OK;
			break;
		}
		case OpCode::OP_CONSTANT:
		{
			Value constant = this->readConstant();
			this->push(constant);
//			LOG(INFO) << this->debugger.printValue(constant);
			break;
		}
		case OpCode::OP_NIL:
		{
			this->push(std::monostate());
			break;
		}
		case OpCode::OP_TRUE:
		{
			this->push(true);
			break;
		}
		case OpCode::OP_FALSE:
		{
			this->push(false);
			break;
		}
		case OpCode::OP_POP:
		{
			this->pop();
			break;
		}
		case OpCode::OP_GET_LOCAL:
		{
			uint8_t slot = this->readByte();
			this->push(this->peek(slot));
			break;
		}
		case OpCode::OP_SET_LOCAL: {
			uint8_t slot = this->readByte();
			this->_stack[slot] = this->peek(0);
			break;
		}
		case OpCode::OP_GREATER:
		{
			this->binaryOp(">");
			break;
		}
		case OpCode::OP_LESS:
		{
			this->binaryOp("<");
			break;
		}
		case OpCode::OP_GET_GLOBAL:
		{
			auto name = this->chunk->constants.at(this->readByte());
			auto val = name.asObject();
			auto str = dynamic_cast<ObjString*>(val);

			if (this->globals.count(*str) == 0)
			{
				runtimeError("Undefined variable '%s'.", str);
				return InterpretResult::INTERPRET_COMPILE_ERROR;
			}

			this->push(this->globals.at(*str));
			break;
		}
		case OpCode::OP_DEFINE_GLOBAL:
		{
			auto name = this->chunk->constants.at(this->readByte());
			auto value = name.asObject();
			if (value->type != ObjType::OBJ_STRING)
			{
				LOG(ERROR) << "TYPE IS WRONG!";
			}
			auto s = dynamic_cast<ObjString*>(value);
//			this->globals.insert({
//				s, this->peek(0)
//			});
			auto v = this->peek(0);
			this->globals[*s] = v;
			this->pop();
			break;
		}
		case OpCode::OP_SET_GLOBAL:
		{
			auto name = this->chunk->constants.at(this->readByte());
			auto value = name.asObject();
			if (value->type != ObjType::OBJ_STRING)
			{
				LOG(ERROR) << "TYPE IS WRONG!";
			}
			auto s = dynamic_cast<ObjString*>(value);
			if (!this->globals.count(*s))
			{
				runtimeError("Undefined variable '%s'.", s);
				return InterpretResult::INTERPRET_COMPILE_ERROR;
			}
			auto top = this->peek(0);
			this->globals[*s] = top;
			break;
		}
		case OpCode::OP_EQUAL:
		{
			Value b = this->pop();
			Value a = this->pop();
			this->push(valuesEqual(a, b));
			break;
		}
		default:
			LOG(ERROR) << "NOT SUPPORT INSTRUCTION";
		}
	}
	return InterpretResult::INTERPRET_RUNTIME_ERROR;
}

uint8_t vm::readByte()
{
	auto ret = this->chunk->at(this->ip);
	this->ip++;
	return ret;
}

uint16_t vm::readShort()
{
    uint8_t a = this->chunk->at(this->ip);
    uint8_t b = this->chunk->at(this->ip + 1);

    this->ip += 2;

    return (uint16_t)((a << 8) | b);
}

Value vm::readConstant()
{
	auto index = this->readByte();
	auto value = this->chunk->constants.at(static_cast<size_t>(index));
	return value;
}

void vm::push(const Value& value)
{
	this->_stack.push(value);
}

Value vm::pop()
{
	return this->_stack.pop();
}

Value vm::top()
{
	return this->_stack.top();
}

InterpretResult vm::binaryOp(const string& op)
{
	do
	{

//		if (!this->peek(0).isNumber() || !this->peek(1).isNumber())
//		{
//			this->runtimeError("Operands must be numbers.");
//			return InterpretResult::INTERPRET_RUNTIME_ERROR;
//		}
		if (this->peek(0).isNumber() && this->peek(1).isNumber())
		{
			Value b = this->pop();
			Value a = this->pop();
			auto vb = b.asNumber();
			auto va = a.asNumber();

			if (op == "*")
			{
				this->push(va * vb);
			}
			else if (op == "/")
			{
				this->push(va / vb);
			}
			else if (op == "+")
			{
				this->push(va + vb);
			}
			else if (op == "-")
			{
				this->push(va - vb);
			}
			else if (op == ">")
			{
				this->push(va > vb);
			}
			else if (op == "<")
			{
				this->push(va < vb);
			}
		}
		else if (this->peek(0).isObject() && this->peek(1).isObject())
		{
			Value b = this->pop();
			Value a = this->pop();
			auto vb = b.asObject();
			auto va = a.asObject();

			if (va->type == ObjType::OBJ_STRING && vb->type == ObjType::OBJ_STRING)
			{
				auto strA = dynamic_cast<ObjString*>(va);
				auto strB = dynamic_cast<ObjString*>(vb);

				auto newStr = new ObjString(*strA + *strB);
				this->addObject(newStr);
				this->push(newStr);
			}
			else
			{
				runtimeError("Operands must be two numbers or two strings.");
				return InterpretResult::INTERPRET_RUNTIME_ERROR;
			}
		}
		else
		{
			runtimeError("Operands must be two numbers or two strings.");
			return InterpretResult::INTERPRET_RUNTIME_ERROR;
		}

	} while (false);

	return InterpretResult::INTERPRET_OK;
}

void vm::execute(int argc, char** argv)
{
	if (argc == 1)
	{
		this->repl();
	}
	else if (argc == 2)
	{
		this->runFile(argv[1]);
	}
	else
	{
		fprintf(stderr, "Usage: clox [path]\n");
		exit(64);
	}
}

void vm::repl()
{
	char line[1024];
	for (;;)
	{
		printf("> ");

		if (!fgets(line, sizeof(line), stdin))
		{
			printf("\n");
			break;
		}

//		interpret(line);
	}
}

string vm::readFile(const string& path)
{
	std::ifstream file;
	file.open(path);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string fileContent(buffer.str());
		return fileContent;
	}
	LOG(ERROR) << "OPEN FILE FAILED !!!";
	return string{};
}

void vm::runFile(const string& path)
{
	string source = this->readFile(path);
	if (source.empty())
		return;
	InterpretResult result = this->interpret(source);

	if (result == InterpretResult::INTERPRET_COMPILE_ERROR) exit(65);
	if (result == InterpretResult::INTERPRET_RUNTIME_ERROR) exit(70);
}

InterpretResult vm::interpret(const string& source)
{
	auto c = new Chunk();

	if (!this->compile(source, c))
	{
		delete c;
		return InterpretResult::INTERPRET_COMPILE_ERROR;
	}

	this->chunk = c;
	this->ip = 0;

	InterpretResult result = this->run();
	delete c;

	return result;
}

bool vm::compile(const string& source, Chunk* c)
{
	auto compiler = Compiler();
	return compiler.compile(source, c);
}

Value vm::peek(int distance)
{
	auto topStack = this->_stack.size();
	return this->_stack[topStack - 1 - distance];
}

void vm::runtimeError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

//	size_t instruction = this->ip - this->chunk->code - 1;
	int line = this->chunk->lines[this->ip - 1];
	fprintf(stderr, "[line %d] in script\n", line);
//	resetStack();
}

void vm::addObject(Object* object)
{
	this->chunk->addObjects(object);
}

bool vm::isFalsey(Value value)
{
	return value.isNil() || (value.isBool() && !value.asBool());
}

bool vm::valuesEqual(Value a, Value b)
{
	if (a.index() != b.index()) return false;
	switch (a.type())
	{
	case ValueType::BOOL:
	{
		return a.asBool() == b.asBool();
	}
	case ValueType::NIL:
	{
		return true;
	}
	case ValueType::NUMBER:
	{
		return a.asNumber() == b.asNumber();
	}
	default:
		return false; // Unreachable.
	}
}

vm::~vm()
{
	this->freeObjects();
}

void vm::freeObjects()
{
	if (this->chunk)
	{
		auto obj = this->chunk->objects;
		while (obj != nullptr)
		{
			auto next = obj->next;
			freeObject(obj);
			obj = next;
		}
	}
}

void vm::freeObject(Object* obj)
{
	switch (obj->type)
	{
	case ObjType::OBJ_STRING:
	{
		auto str = dynamic_cast<ObjString*>(obj);
		delete str;
	}
	}
}
