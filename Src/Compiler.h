//
// Created by tjp56 on 2021/4/10.
//

#ifndef VM700_COMPILER_H
#define VM700_COMPILER_H

#include <string>

#include "Scanner.h"
#include "Parser.h"

using std::string;

class Compiler
{
private:
	Scanner* scanner = nullptr;
	Parser* parser = nullptr;

	void errorAtCurrent(const string& message);
	void error(string message);
	void errorAt(Token* token, string message);

	void consume(TokenType type, const string& message);
public:
	bool compile(const string& source);
	void advance();
	void expression();
};


#endif //VM700_COMPILER_H
