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
#include "ObjFunction.h"

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

struct UpValue {
    uint8_t index;
    bool isLocal;
} ;

struct Local
{
	Token name;
	int32_t depth;
    bool isCaptured = false;
};

class Compiler
{
private:
	Parser* parser = nullptr;
    Compiler* enclosing;

	map<TokenType, ParseRule> rules{};
	Debug debugger {};

    ObjFunction* function = nullptr;
    FunctionType type = FunctionType::TYPE_SCRIPT;

	Local locals[std::numeric_limits<uint8_t>::max()];
    UpValue upValues[std::numeric_limits<uint8_t>::max()];
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

    ObjFunction* endCompiler();

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

    int32_t resolveLocal(Compiler* enclosing, Token* name);

	void markInitialized();

    void ifStatement();

    int32_t emitJump(OpCode instruction);

    Chunk* currentChunk();

    void patchJump(int32_t offset);

    void and_(bool canAssign);

    void or_(bool canAssign);

    void whileStatement();

    void emitLoop(int32_t loopStart);

    void forStatement();

    void funDeclaration();

    void functionBody(FunctionType type);

    void initRules();

    void call(bool canAssign);

    uint8_t argumentList();

    void returnStatement();

    int32_t resolveUpvalue(Compiler* compiler, Token* name);

    int32_t addUpvalue(Compiler* compiler, uint8_t index, bool isLocal);

public:
	Compiler();

    Compiler(Compiler* compiler, FunctionType type);

	ObjFunction* compile(const string& source);

    ObjFunction* compileFunc(Parser* p);

	void advance();

	void expression();

    Compiler* getEnclosing();
};


#endif //VM700_COMPILER_H
