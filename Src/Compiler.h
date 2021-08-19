//
// Created by tjp56 on 2021/4/10.
//

#ifndef VM700_COMPILER_H
#define VM700_COMPILER_H

#include <string>
#include <map>
#include <functional>
#include <limits>

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
	std::function<void(bool canAssign)> prefix;
	std::function<void(bool canAssign)> infix;
	Precedence precedence;
};

struct Local
{
	Token name;
	int32_t depth;
};

class Compiler
{
private:
	Scanner* scanner = nullptr;
	Parser* parser = nullptr;
	Chunk* compilingChunk = nullptr;
	map<TokenType, ParseRule> rules{};
	Debug debugger {};

	Local locals[std::numeric_limits<uint8_t>::max()];
	int32_t localCount = 0;
	int32_t scopeDepth = 0;

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

	void namedVariable(Token* name, bool canAssign);

	void defineVariable(uint8_t global);

	uint8_t makeConstant(Value value);

	ParseRule* getRule(TokenType type);

	void parsePrecedence(Precedence precedence);

	void grouping(bool canAssign);

	void unary(bool canAssign);

	void binary(bool canAssign);

	void number(bool canAssign);

	void literal(bool canAssign);

	void readString(bool canAssign);

	void variable(bool canAssign);

	void declaration();

	void statement();

	void printStatement();

	void expressionStatement();

	void varDeclaration();

	void block();

	void beginScope();

	void endScope();

	void declareVariable();

	void addLocal(Token name);

	bool identifiersEqual(Token* a, Token* b);

	int resolveLocal(Token* name);

	void markInitialized();

public:
	Compiler();

	bool compile(const string& source, Chunk* chunk);

	void advance();

	void expression();
};


#endif //VM700_COMPILER_H
