//
// Created by tjp56 on 2021/4/11.
//

#ifndef VM700_PARSER_H
#define VM700_PARSER_H

#include "Scanner.h"

class Parser
{
public:
	explicit Parser(Scanner* s);
	Scanner* scanner;

	bool hadError = false;
	bool panicMode = false;
	Token current;
	Token previous;
};


#endif //VM700_PARSER_H
