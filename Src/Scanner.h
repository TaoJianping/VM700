//
// Created by tjp56 on 2021/4/10.
//

#ifndef VM700_SCANNER_H
#define VM700_SCANNER_H


#include <cstddef>
#include <string>

using std::string;

enum class TokenType {
	// Single-character tokens.
	TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
	TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
	TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

	// One or two character tokens.
	TOKEN_BANG, TOKEN_BANG_EQUAL,
	TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
	TOKEN_GREATER, TOKEN_GREATER_EQUAL,
	TOKEN_LESS, TOKEN_LESS_EQUAL,

	// Literals.
	TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

	// Keywords.
	TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
	TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
	TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
	TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

	TOKEN_ERROR,
	TOKEN_EOF
};


struct Token {
	TokenType type;
	std::string lexeme;
	int length;
	int line;
};


class Scanner
{
private:
	int start = 0;
	int current = 0;
	int line = 1;
	const string source;

	bool isAtEnd();
	char advance();
	bool match(char expected);
	void skipWhitespace();
	bool isDigit(char c);
	bool isAlpha(char c);
	char peek();
	char peekNext();
	Token readString();
	Token readNumber();
	Token identifier();
	TokenType identifierType();
	TokenType checkKeyword(int startIndex, int length, string rest, TokenType type);
	Token makeToken(TokenType type);
	[[nodiscard]] Token errorToken(const string& message) const;
public:
	Token scanToken();
	explicit Scanner(string  source);
};


#endif //VM700_SCANNER_H
