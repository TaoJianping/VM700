//
// Created by tjp56 on 2021/4/10.
//

#include "Compiler.h"
#include "absl/strings/str_format.h"
#include "glog/logging.h"

bool Compiler::compile(const string& source)
{
	this->scanner = new Scanner(source);
	this->parser = new Parser(this->scanner);
	this->advance();
	this->expression();
	this->consume(TokenType::TOKEN_EOF, "Expect end of expression.");
	return !this->parser->hadError;
}

void Compiler::advance()
{
	this->parser->previous = this->parser->current;

	for (;;) {
		this->parser->current = this->scanner->scanToken();
		if (this->parser->current.type != TokenType::TOKEN_ERROR)
			break;

//		errorAtCurrent(parser.current.start);
	}
}

void Compiler::expression()
{

}

void Compiler::errorAtCurrent(const string& message)
{
	this->errorAt(&this->parser->current, message);
}

void Compiler::error(string message)
{
	this->errorAt(&this->parser->previous, message);
}

void Compiler::errorAt(Token* token, string message)
{
	if (this->parser->panicMode)
		return;
	this->parser->panicMode = true;
	fprintf(stderr, "[line %d] Error", token->line);

	if (token->type == TokenType::TOKEN_EOF) {
		fprintf(stderr, " at end");
	} else if (token->type == TokenType::TOKEN_ERROR) {
		// Nothing.
	} else {
		fprintf(stderr, " at '%s'", token->lexeme.c_str());
	}

	fprintf(stderr, ": %s\n", message.c_str());
	parser->hadError = true;
}

void Compiler::consume(TokenType type, const string& message)
{
	if (this->parser->current.type == type) {
		this->advance();
		return;
	}

	this->errorAtCurrent(message);
}
