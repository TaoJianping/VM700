//
// Created by tjp56 on 2021/4/10.
//

#include "Compiler.h"

#include <utility>
#include "absl/strings/str_format.h"
#include "glog/logging.h"

bool Compiler::compile(const string& source, Chunk* chunk)
{
	this->scanner = new Scanner(source);
	this->parser = new Parser(this->scanner);
	this->compilingChunk = chunk;

	this->advance();
	this->expression();
	this->consume(TokenType::TOKEN_EOF, "Expect end of expression.");

	this->endCompiler();
	return !this->parser->hadError;
}

void Compiler::advance()
{
	this->parser->previous = this->parser->current;

	for (;;)
	{
		this->parser->current = this->scanner->scanToken();
		if (this->parser->current.type != TokenType::TOKEN_ERROR)
			break;

		this->errorAtCurrent(this->parser->current.lexeme);
	}
}

void Compiler::expression()
{
	this->parsePrecedence(Precedence::PREC_ASSIGNMENT);
}

void Compiler::errorAtCurrent(const string& message)
{
	this->errorAt(&this->parser->current, message);
}

void Compiler::error(const string& message)
{
	this->errorAt(&this->parser->previous, message);
}

void Compiler::errorAt(Token* token, const string& message)
{
	if (this->parser->panicMode)
		return;
	this->parser->panicMode = true;
	fprintf(stderr, "[line %d] Error", token->line);

	if (token->type == TokenType::TOKEN_EOF)
	{
		fprintf(stderr, " at end");
	}
	else if (token->type == TokenType::TOKEN_ERROR)
	{
		// Nothing.
	}
	else
	{
		fprintf(stderr, " at '%s'", token->lexeme.c_str());
	}

	fprintf(stderr, ": %s\n", message.c_str());
	parser->hadError = true;
}

void Compiler::consume(TokenType type, const string& message)
{
	if (this->parser->current.type == type)
	{
		this->advance();
		return;
	}

	this->errorAtCurrent(message);
}

void Compiler::emitByte(uint8_t byte)
{
	this->compilingChunk->write(byte, this->parser->previous.line);
}

void Compiler::emitByte(OpCode code)
{
	this->emitByte(static_cast<uint8_t>(code));
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2)
{
	this->emitByte(byte1);
	this->emitByte(byte2);
}

void Compiler::emitBytes(OpCode code, uint8_t byte2)
{
	this->emitByte(code);
	this->emitByte(byte2);
}

void Compiler::endCompiler()
{
	this->emitReturn();
#ifdef DEBUG_PRINT_CODE
	if (!this->parser->hadError) {
		this->debugger.disassembleChunk(this->compilingChunk, "code");
	}
#endif
}

void Compiler::emitReturn()
{
	this->emitByte(OpCode::OP_RETURN);
}

void Compiler::emitConstant(double value)
{
	this->emitBytes(OpCode::OP_CONSTANT, makeConstant(value));
}

uint8_t Compiler::makeConstant(Value value)
{
	std::size_t constant = this->compilingChunk->addConstant(value);
	if (constant > UINT8_MAX)
	{
		this->error("Too many constants in one chunk.");
		return 0;
	}

	return static_cast<uint8_t>(constant);
}

void Compiler::number()
{
	double value = std::stod(this->parser->previous.lexeme);
	this->emitConstant(value);
}

void Compiler::grouping()
{
	this->expression();
	this->consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary()
{
	TokenType operatorType = this->parser->previous.type;

	// Compile the operand.
	this->parsePrecedence(Precedence::PREC_UNARY);

	// Emit the operator instruction.
	switch (operatorType)
	{
	case TokenType::TOKEN_MINUS:
		this->emitByte(OpCode::OP_NEGATE);
		break;
	default:
		return; // Unreachable.
	}
}

void Compiler::binary()
{
	// Remember the operator.
	TokenType operatorType = this->parser->previous.type;

	// Compile the right operand.
	ParseRule* rule = getRule(operatorType);
	parsePrecedence((Precedence)(static_cast<int>(rule->precedence) + 1));

	// Emit the operator instruction.
	switch (operatorType)
	{
	case TokenType::TOKEN_PLUS:
		emitByte(OpCode::OP_ADD);
		break;
	case TokenType::TOKEN_MINUS:
		emitByte(OpCode::OP_SUBTRACT);
		break;
	case TokenType::TOKEN_STAR:
		emitByte(OpCode::OP_MULTIPLY);
		break;
	case TokenType::TOKEN_SLASH:
		emitByte(OpCode::OP_DIVIDE);
		break;
	default:
		return; // Unreachable.
	}
}

Compiler::Compiler()
{
	auto funcReadNumber = [this]() { this->number(); };
	auto funcGrouping = [this]() { this->grouping(); };
	auto funcUnary = [this]() { this->unary(); };
	auto funcBinary = [this]() { this->binary(); };

	this->rules = map<TokenType, ParseRule>{
			{ TokenType::TOKEN_LEFT_PAREN,    { funcGrouping,   nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_RIGHT_PAREN,   { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_LEFT_BRACE,    { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_RIGHT_BRACE,   { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_COMMA,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_DOT,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_MINUS,         { funcUnary,      funcBinary, Precedence::PREC_TERM }},
			{ TokenType::TOKEN_PLUS,          { nullptr,        funcBinary, Precedence::PREC_TERM }},
			{ TokenType::TOKEN_SEMICOLON,     { nullptr,        nullptr,    Precedence::PREC_TERM }},
			{ TokenType::TOKEN_SLASH,         { nullptr,        funcBinary, Precedence::PREC_FACTOR }},
			{ TokenType::TOKEN_STAR,          { nullptr,        funcBinary, Precedence::PREC_FACTOR }},
			{ TokenType::TOKEN_BANG,          { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_BANG_EQUAL,    { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_EQUAL,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_EQUAL_EQUAL,   { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_GREATER,       { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_GREATER_EQUAL, { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_LESS,          { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_LESS_EQUAL,    { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_IDENTIFIER,    { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_STRING,        { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_NUMBER,        { funcReadNumber, nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_AND,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_CLASS,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_ELSE,          { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_FALSE,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_FOR,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_FUN,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_IF,            { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_NIL,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_OR,            { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_PRINT,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_RETURN,        { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_SUPER,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_THIS,          { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_TRUE,          { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_VAR,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_WHILE,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_ERROR,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
			{ TokenType::TOKEN_EOF,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
	};
}

ParseRule* Compiler::getRule(TokenType type)
{
	return &this->rules.at(type);
}

void Compiler::parsePrecedence(Precedence precedence)
{
	this->advance();
	auto prefixRule = this->getRule(parser->previous.type)->prefix;
	if (prefixRule == nullptr)
	{
		this->error("Expect expression.");
		return;
	}

	prefixRule();

	while (precedence <= this->getRule(this->parser->current.type)->precedence) {
		this->advance();
		auto infixRule = getRule(parser->previous.type)->infix;
		infixRule();
	}
}




