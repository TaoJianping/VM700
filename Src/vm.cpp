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
		LOG(INFO) << StrFormat("          %s", this->_stack.toString());
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
			this->push()
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
		case OpCode::OP_RETURN:
		{
			auto ret = this->pop();
			LOG(INFO) << "RETURN -> " << ret.asNumber();
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

Value vm::readConstant()
{
	auto index = this->readByte();
	auto value = this->chunk->constants.at(static_cast<size_t>(index));
	return value;
}

void vm::push(Value value)
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
		Value b = this->pop();
		Value a = this->pop();
		if (!this->peek(0).isNumber() || !this->peek(1).isNumber())
		{
			this->runtimeError("Operands must be numbers.");
			return InterpretResult::INTERPRET_RUNTIME_ERROR;
		}

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
	} while (false);
}

void vm::execute(int argc, char** argv)
{
	if (argc == 1) {
		this->repl();
	} else if (argc == 2) {
		this->runFile(argv[1]);
	} else {
		fprintf(stderr, "Usage: clox [path]\n");
		exit(64);
	}
}

void vm::repl()
{
	char line[1024];
	for (;;) {
		printf("> ");

		if (!fgets(line, sizeof(line), stdin)) {
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

	if (!this->compile(source, c)) {
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
