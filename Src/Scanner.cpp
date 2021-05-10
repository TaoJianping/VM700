//
// Created by tjp56 on 2021/4/10.
//

#include "Scanner.h"

#include <utility>

Scanner::Scanner(string source) : source(std::move(source))
{

}

Token Scanner::scanToken()
{
	this->skipWhitespace();
	auto len = this->source.length();
	this->start = this->current;
	if (this->isAtEnd())
		return this->makeToken(TokenType::TOKEN_EOF);

	char c = this->advance();

	if (this->isAlpha(c)) return this->identifier();
	if (this->isDigit(c)) return this->readNumber();

	switch (c)
	{
	case '(':
		return makeToken(TokenType::TOKEN_LEFT_PAREN);
	case ')':
		return makeToken(TokenType::TOKEN_RIGHT_PAREN);
	case '{':
		return makeToken(TokenType::TOKEN_LEFT_BRACE);
	case '}':
		return makeToken(TokenType::TOKEN_RIGHT_BRACE);
	case ';':
		return makeToken(TokenType::TOKEN_SEMICOLON);
	case ',':
		return makeToken(TokenType::TOKEN_COMMA);
	case '.':
		return makeToken(TokenType::TOKEN_DOT);
	case '-':
		return makeToken(TokenType::TOKEN_MINUS);
	case '+':
		return makeToken(TokenType::TOKEN_PLUS);
	case '/':
		return makeToken(TokenType::TOKEN_SLASH);
	case '*':
		return makeToken(TokenType::TOKEN_STAR);
	case '!':
		return makeToken(match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_BANG);
	case '=':
		return makeToken(match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL);
	case '<':
		return makeToken(match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
	case '>':
		return makeToken(match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);
	case '"':
		return this->readString();
	}

	return this->errorToken("Unexpected character.");
}

bool Scanner::isAtEnd()
{
	return this->current >= this->source.length();
}

Token Scanner::makeToken(TokenType type)
{
	auto t = Token();
	t.type = type;
	t.line = this->line;
	t.lexeme = this->source.substr(this->start, (this->current - this->start));
	t.length = this->current - this->start;
	return t;
}

Token Scanner::errorToken(const string& message) const
{
	auto t = Token();
	t.type = TokenType::TOKEN_ERROR;
	t.line = this->line;
	t.lexeme = message;
	t.length = message.length();
	return t;
}

char Scanner::advance()
{
	this->current++;
	return this->source.at(this->current - 1);
}

bool Scanner::match(char expected)
{
	if (this->isAtEnd())
		return false;
	if (this->source.at(this->current) != expected)
		return false;

	this->current++;
	return true;
}

void Scanner::skipWhitespace()
{
	for (;;)
	{
		char c = this->peek();
		switch (c)
		{
		case ' ':
		case '\r':
		case '\t':
			this->advance();
			break;
		case '\n':
			this->line++;
			this->advance();
			break;
		case '/':
			if (peekNext() == '/')
			{
				// A comment goes until the end of the line.
				while (peek() != '\n' && !isAtEnd()) advance();
			}
			else
			{
				return;
			}
			break;
		default:
			return;
		}
	}
}

char Scanner::peek()
{
	if (isAtEnd()) return '\0';
	return this->source.at(this->current);
}

char Scanner::peekNext()
{
	if (isAtEnd()) return '\0';
	return this->source.at(this->current + 1);
}

Token Scanner::readString()
{
	while (this->peek() != '"' && !this->isAtEnd())
	{
		if (peek() == '\n')
			this->line++;
		this->advance();
	}

	if (this->isAtEnd())
		return this->errorToken("Unterminated string.");

	// The closing quote.
	this->advance();
	return makeToken(TokenType::TOKEN_STRING);
}

bool Scanner::isDigit(char c)
{
	return c >= '0' && c <= '9';
}

Token Scanner::readNumber()
{
	while (this->isDigit(this->peek()))
		this->advance();

	// Look for a fractional part.
	if (this->peek() == '.' && this->isDigit(this->peekNext()))
	{
		// Consume the ".".
		this->advance();

		while (this->isDigit(this->peek()))
			this->advance();
	}

	return this->makeToken(TokenType::TOKEN_NUMBER);
}

bool Scanner::isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') ||
		   (c >= 'A' && c <= 'Z') ||
		   c == '_';
}

Token Scanner::identifier()
{
	while (this->isAlpha(peek()) || this->isDigit(peek())) this->advance();

	return this->makeToken(this->identifierType());
}

TokenType Scanner::identifierType()
{
	switch (this->source.at(this->start))
	{
	case 'a':
		return this->checkKeyword(1, 2, "nd", TokenType::TOKEN_AND);
	case 'c':
		return this->checkKeyword(1, 4, "lass", TokenType::TOKEN_CLASS);
	case 'e':
		return this->checkKeyword(1, 3, "lse", TokenType::TOKEN_ELSE);
	case 'f':
		if (this->current - this->start > 1)
		{
			switch (this->source.at(this->start + 1))
			{
			case 'a':
				return this->checkKeyword(2, 3, "lse", TokenType::TOKEN_FALSE);
			case 'o':
				return this->checkKeyword(2, 1, "r", TokenType::TOKEN_FOR);
			case 'u':
				return this->checkKeyword(2, 1, "n", TokenType::TOKEN_FUN);
			}
		}
		break;
	case 'i':
		return this->checkKeyword(1, 1, "f", TokenType::TOKEN_IF);
	case 'n':
		return this->checkKeyword(1, 2, "il", TokenType::TOKEN_NIL);
	case 'o':
		return this->checkKeyword(1, 1, "r", TokenType::TOKEN_OR);
	case 'p':
		return this->checkKeyword(1, 4, "rint", TokenType::TOKEN_PRINT);
	case 'r':
		return this->checkKeyword(1, 5, "eturn", TokenType::TOKEN_RETURN);
	case 's':
		return this->checkKeyword(1, 4, "uper", TokenType::TOKEN_SUPER);
	case 't':
		if (this->current - this->start > 1)
		{
			switch (this->source.at(this->start + 1))
			{
			case 'h':
				return this->checkKeyword(2, 2, "is", TokenType::TOKEN_THIS);
			case 'r':
				return this->checkKeyword(2, 2, "ue", TokenType::TOKEN_TRUE);
			}
		}
		break;
	case 'v':
		return this->checkKeyword(1, 2, "ar", TokenType::TOKEN_VAR);
	case 'w':
		return this->checkKeyword(1, 4, "hile", TokenType::TOKEN_WHILE);
	}
	return TokenType::TOKEN_IDENTIFIER;
}

TokenType Scanner::checkKeyword(int startIndex, int length, string rest, TokenType type)
{
	if (this->current - this->start == (startIndex + length) &&
		this->source.substr(this->start + startIndex, length) == rest)
	{
		return type;
	}

	return TokenType::TOKEN_IDENTIFIER;
}