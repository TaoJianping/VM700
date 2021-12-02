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
#include <utility>
#include "chunk.h"
#include "Compiler.h"
#include "ObjFunction.h"
#include "ObjClosure.h"
#include "ObjUpvalue.h"

using absl::StrFormat;

uint8_t readByte(CallFrame* frame)
{
    return *frame->ip++;
}

uint16_t readShort(CallFrame* frame)
{
    frame->ip += 2;
    return static_cast<uint16_t>((frame->ip[-2] << 8) | frame->ip[-1]);
}

Value readConstant(CallFrame* frame)
{
    auto i = readByte(frame);
    return frame->closure->function->getChunk()->constants.at(i);
}

InterpretResult vm::run()
{
    auto frame = this->frames.top();

	for (;;)
	{
#ifdef DEBUG_TRACE_EXECUTION
		LOG(INFO) << StrFormat("              %s", this->vmStack.toString());
		this->debugger.disassembleInstruction(frame->closure->function->getChunk(),
                                              (int)(frame->ip - frame->closure->function->getChunk()->data()));
#endif
		uint8_t instruction = readByte(frame);
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
            uint16_t offset = readShort(frame);
            frame->ip += offset;
            break;
        }
        case OpCode::OP_JUMP_IF_FALSE:
        {
            uint16_t offset = readShort(frame);
            if (this->isFalsey(this->peek(0)))
            {
                frame->ip += offset;
            }
            break;
        }
        case OpCode::OP_LOOP:
        {
            uint16_t offset = readShort(frame);
            frame->ip -= offset;
            break;
        }
        case OpCode::OP_CALL:
        {
            int32_t argCount = readByte(frame);
            if (!this->callValue(peek(argCount), argCount)) {
                return InterpretResult::INTERPRET_RUNTIME_ERROR;
            }
            frame = this->frames.top();
            break;
        }
        case OpCode::OP_CLOSURE:
        {
            auto value = readConstant(frame);
            auto obj = value.asObject();
            auto* function = dynamic_cast<ObjFunction*>(obj);
            auto* closure = new ObjClosure(function);
            this->push(closure);
			for (int32_t i = 0; i < closure->upvalues.size(); ++i) {
				uint8_t isLocal = readByte(frame);
				uint8_t index = readByte(frame);
				if (isLocal) {
					closure->upvalues[i] = captureUpvalue(frame->slots + index);
				} else {
					closure->upvalues[i] = frame->closure->upvalues[index];
				}
			}
            break;
        }
		case OpCode::OP_RETURN:
		{
            Value result = this->pop();
            this->frames.pop_back();
            if (this->frames.size() == 0) {
                this->pop();
                return InterpretResult::INTERPRET_OK;
            }
            vmStack.stackTop = frame->slots;
            this->push(result);
            frame = this->frames.top();
            break;
		}
		case OpCode::OP_CONSTANT:
		{
			Value constant = readConstant(frame);
			this->push(constant);
			break;
		}
		case OpCode::OP_NIL:
		{
			this->push(Value(Nil()));
			break;
		}
		case OpCode::OP_TRUE:
		{
			this->push(Value(true));
			break;
		}
		case OpCode::OP_FALSE:
		{
			this->push(Value(false));
			break;
		}
		case OpCode::OP_POP:
		{
			this->pop();
			break;
		}
		case OpCode::OP_GET_LOCAL:
		{
			uint8_t slot = readByte(frame);
			this->push(frame->slots[slot]);
			break;
		}
		case OpCode::OP_SET_LOCAL: {
			uint8_t slot = readByte(frame);
            frame->slots[slot] = this->peek(0);
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
            auto name = readConstant(frame);
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
            auto name = readConstant(frame);
			auto value = name.asObject();
			if (value->type != ObjType::OBJ_STRING)
			{
				LOG(ERROR) << "TYPE IS WRONG!";
			}
			auto s = dynamic_cast<ObjString*>(value);
			auto v = this->peek(0);
			this->globals[*s] = v;
			this->pop();
			break;
		}
		case OpCode::OP_SET_GLOBAL:
		{
            auto name = readConstant(frame);
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
		case OpCode::OP_GET_UPVALUE:
		{
			uint8_t slot = readByte(frame);
			push(*frame->closure->upvalues[slot]->location);
			break;
		}
		case OpCode::OP_SET_UPVALUE:
		{
			uint8_t slot = readByte(frame);
			*frame->closure->upvalues[slot]->location = peek(0);
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

void vm::push(const Value& value)
{
	this->vmStack.push(value);
}

Value vm::pop()
{
	return this->vmStack.pop();
}

Value vm::top()
{
	return this->vmStack.top();
}

InterpretResult vm::binaryOp(const string& op)
{
	do
	{
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
            LOG(INFO) << "operator -> " << op;
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
    auto function = this->compile(source);
	if (function == nullptr)
		return InterpretResult::INTERPRET_COMPILE_ERROR;

    this->push(function);
    auto* closure = new ObjClosure(function);
    pop();
    push(closure);
    call(closure, 0);

    defineNative("clock", clockNative);

    InterpretResult result = this->run();
	return result;
}

ObjFunction* vm::compile(const string& source)
{
    return Compiler().compile(source);
}

Value vm::peek(int distance)
{
    return vmStack.stackTop[-1 - distance];
}

void vm::runtimeError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

    for (int32_t i = this->frames.size() - 1; i >= 0; i--) {
        CallFrame* frame = &this->frames[i];
        ObjFunction* function = frame->closure->function;
        size_t instruction = frame->ip - function->chunk.data() - 1;
        fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);
        if (function->name.empty()) {
            fprintf(stderr, "script\n");
        } else {
            fprintf(stderr, "%s()\n", function->name.c_str());
        }
    }
    // resetStack();
}

void vm::addObject(Object* object)
{
	this->addObjects(object);
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
	if (this->objects)
	{
		auto obj = this->objects;
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
            break;
        }
        case ObjType::OBJ_FUNCTION:
        {
            auto func = dynamic_cast<ObjFunction*>(obj);
            delete func;
            break;
        }
        case ObjType::OBJ_NATIVE:
        {
            auto n = dynamic_cast<ObjNative*>(obj);
            delete n;
            break;
        }
        case ObjType::OBJ_CLOSURE:
        {
            auto c = dynamic_cast<ObjClosure*>(obj);
            delete c;
            break;
        }
		case ObjType::OBJ_UPVALUE:
		{
			auto u = dynamic_cast<ObjUpvalue*>(obj);
			delete u;
			break;
		}
	}
}

void vm::addObjects(Object *obj)
{
    obj->next = this->objects;
    this->objects = obj;
}

bool vm::callValue(Value callee, int32_t argCount)
{
    if (callee.isObject()) {
        switch (callee.type()) {
            case ValueType::Object:
            {
                auto obj = std::get<Object*>(callee);
                if (obj->type == ObjType::OBJ_NATIVE) {
                    auto native = dynamic_cast<ObjNative*>(obj);
                    Value result = native->function(argCount, vmStack.stackTop - argCount);
                    vmStack.stackTop -= argCount + 1;
                    this->push(result);
                    return true;
                } else if (obj->type == ObjType::OBJ_CLOSURE) {
                    return call(dynamic_cast<ObjClosure*>(obj), argCount);
                }
                else {
                    runtimeError("NOT SUPPORT OBJ TYPE!");
                }

            }
            default:
                break; // Non-callable object type.
        }
    }
    runtimeError("Can only call functions and classes.");
    return false;
}

void vm::defineNative(const char *name, NativeFn func) {
    auto nativeName = new ObjString(name);
    auto nativeFunc = new ObjNative(std::move(func));
    push(nativeName);
    push(nativeFunc);
    this->globals[*nativeName] = nativeFunc;
    pop();
    pop();
}

bool vm::call(ObjClosure *closure, int32_t argCount) {
    auto function = closure->function;
    if (argCount != function->arity)
    {
        runtimeError("Expected %d arguments but got %d.", function->arity, argCount);
        return false;
    }
    if (this->frames.size() == FRAMES_MAX)
    {
        runtimeError("Stack overflow.");
        return false;
    }

    CallFrame* frame = this->frames.newFrame();
    frame->closure = closure;
    frame->ip = function->chunk.data();
    frame->slots = vmStack.stackTop - argCount - 1 ;
    return true;
}

ObjUpvalue* vm::captureUpvalue(Value *local) {
	ObjUpvalue* upvalue = new ObjUpvalue();
	upvalue->location = local;
	return upvalue;
}
