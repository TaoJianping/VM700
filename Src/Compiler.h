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
using std::map;

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

	void synchronize();

	void consume(TokenType type, const string& message);

	bool match(TokenType type);

	bool check(TokenType type);

	void emitByte(uint8_t byte);

	void emitByte(OpCode code);

	void emitBytes(uint8_t byte1, uint8_t byte2);

	void emitBytes(OpCode code, uint8_t byte2);

	void emitBytes(OpCode code, OpCode code2);

	void emitConstant(double value);

	void emitConstant(string value);

	void emitReturn();

	void endCompiler();

	uint8_t parseVariable(const string& errorMessage);

	uint8_t identifierConstant(Token* name);

	void namedVariable(Token* name);

	void defineVariable(uint8_t global);

	uint8_t makeConstant(Value value);

	ParseRule* getRule(TokenType type);

	void parsePrecedence(Precedence precedence);

	void grouping();

	void unary();

	void binary();

	void number();

	void literal();

	void readString();

	void variable();

	void declaration();

	void statement();

	void printStatement();

	void expressionStatement();

	void varDeclaration();

public:
	Compiler();

	bool compile(const string& source, Chunk* chunk);

	void advance();

	void expression();
};


#endif //VM700_COMPILER_H
