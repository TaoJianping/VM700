//
// Created by tjp56 on 2021/4/10.
//

#ifndef VM700_COMPILER_H
#define VM700_COMPILER_H

#include <string>
#include <map>
#include <functional>

#include "Scanner.h"
#include "Parser.h"
#include "chunk.h"
#include "Debug.h"

using std::string;
using std::map;
using std::function;

enum class Precedence : int
{
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
	PREC_AND,         // and
	PREC_EQUALITY,    // == !=
	PREC_COMPARISON,  // < > <= >=
	PREC_TERM,        // + -
	PREC_FACTOR,      // * /
	PREC_UNARY,       // ! -
	PREC_CALL,        // . ()
	PREC_PRIMARY
};

typedef void (* ParseFn)();

struct ParseRule
{
	std::function<void()> prefix;
	std::function<void()> infix;
	Precedence precedence;
};


class Compiler
{
private:
	Scanner* scanner = nullptr;
	Parser* parser = nullptr;
	Chunk* compilingChunk = nullptr;
	map<TokenType, ParseRule> rules{};
	Debug debugger {};

	void errorAtCurrent(const string& message);

	void error(const string& message);

	void errorAt(Token* token, const string& message);

	void consume(TokenType type, const string& message);

	void emitByte(uint8_t byte);

	void emitByte(OpCode code);

	void emitBytes(uint8_t byte1, uint8_t byte2);

	void emitBytes(OpCode code, uint8_t byte2);

	void emitConstant(double value);

	void emitReturn();

	void endCompiler();

	uint8_t makeConstant(Value value);

	ParseRule* getRule(TokenType type);

	void parsePrecedence(Precedence precedence);

	void grouping();

	void unary();

	void binary();

	void number();

public:
	Compiler();

	bool compile(const string& source, Chunk* chunk);

	void advance();

	void expression();
};


#endif //VM700_COMPILER_H
