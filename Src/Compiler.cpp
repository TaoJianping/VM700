//
// Created by tjp56 on 2021/4/10.
//

#include "Compiler.h"

#include <utility>

ObjFunction* Compiler::compile(const std::string& source)
{
	auto s = new Scanner(source);
	this->parser = new Parser(s);

    this->function = new ObjFunction();

    Local* local = &this->locals[this->localCount++];
    local->depth = 0;
    local->name.lexeme = "";
    local->name.length = 0;

	this->advance();

	while (!this->match(TokenType::TOKEN_EOF))
	{
		this->declaration();
	}

	auto func = this->endCompiler();

	return this->parser->hadError ? nullptr : func;
}

void Compiler::advance()
{
	this->parser->previous = this->parser->current;

	for (;;)
	{
		this->parser->current = this->parser->scanner->scanToken();
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

void Compiler::synchronize()
{
	this->parser->panicMode = false;

	while (this->parser->current.type != TokenType::TOKEN_EOF) {
		if (this->parser->previous.type == TokenType::TOKEN_SEMICOLON) return;
		switch (this->parser->current.type) {
		case TokenType::TOKEN_CLASS:
		case TokenType::TOKEN_FUN:
		case TokenType::TOKEN_VAR:
		case TokenType::TOKEN_FOR:
		case TokenType::TOKEN_IF:
		case TokenType::TOKEN_WHILE:
		case TokenType::TOKEN_PRINT:
		case TokenType::TOKEN_RETURN:
			return;

		default:
			; // Do nothing.
		}

		this->advance();
	}
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

bool Compiler::match(TokenType type)
{
	if (!this->check(type)) return false;
	this->advance();
	return true;
}

bool Compiler::check(TokenType type)
{
	return this->parser->current.type == type;
}

void Compiler::emitByte(uint8_t byte)
{
	this->function->getChunk()->write(byte, this->parser->previous.line);
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

void Compiler::emitBytes(OpCode code, OpCode code2)
{
	this->emitByte(static_cast<uint8_t>(code));
	this->emitByte(static_cast<uint8_t>(code2));
}

void Compiler::emitBytes(OpCode code, uint8_t byte2)
{
	this->emitByte(code);
	this->emitByte(byte2);
}

ObjFunction* Compiler::endCompiler()
{
	this->emitReturn();

#ifdef DEBUG_PRINT_CODE
	if (!this->parser->hadError)
	{
		this->debugger.disassembleChunk(this->currentChunk(), !function->name.empty()
                                                              ? function->_name_() : "<script>");
	}
#endif

    return this->function;
}

void Compiler::emitReturn()
{
    this->emitByte(OpCode::OP_NIL);
    this->emitByte(OpCode::OP_RETURN);
}

void Compiler::emitConstant(double value)
{
	this->emitBytes(OpCode::OP_CONSTANT, makeConstant(value));
}

void Compiler::emitConstant(string value)
{
	auto v = new ObjString(std::move(value));
	this->emitBytes(OpCode::OP_CONSTANT, makeConstant(v));
}

uint8_t Compiler::parseVariable(const string& errorMessage)
{
	this->consume(TokenType::TOKEN_IDENTIFIER, errorMessage);
	this->declareVariable();
	if (this->scopeDepth > 0) return 0;

	return identifierConstant(&parser->previous);
}

uint8_t Compiler::identifierConstant(Token* name)
{
	auto str = new ObjString(name->lexeme);
	return makeConstant(str);
}

void Compiler::defineVariable(uint8_t global)
{
	if (this->scopeDepth > 0) {
		this->markInitialized();
		return;
	}
	this->emitBytes(OpCode::OP_DEFINE_GLOBAL, global);
}

uint8_t Compiler::makeConstant(Value value)
{
	std::size_t constant = this->function->getChunk()->addConstant(value);
	if (constant > UINT8_MAX)
	{
		this->error("Too many constants in one chunk.");
		return 0;
	}

	return static_cast<uint8_t>(constant);
}

void Compiler::number(bool canAssign)
{
	double value = std::stod(this->parser->previous.lexeme);
	this->emitConstant(value);
}

void Compiler::grouping(bool canAssign)
{
	this->expression();
	this->consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary(bool canAssign)
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
	case TokenType::TOKEN_BANG:
		this->emitByte(OpCode::OP_NOT);
		break;
	default:
		this->error("UNSUPPORT UNARY OPERATOR TYPE");
		return; // Unreachable.
	}
}

void Compiler::binary(bool canAssign)
{
	// Remember the operator.
	TokenType operatorType = this->parser->previous.type;

	// Compile the right operand.
	ParseRule* rule = this->getRule(operatorType);
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
	case TokenType::TOKEN_BANG_EQUAL:
		emitBytes(OpCode::OP_EQUAL, OpCode::OP_NOT);
		break;
	case TokenType::TOKEN_EQUAL_EQUAL:
		emitByte(OpCode::OP_EQUAL);
		break;
	case TokenType::TOKEN_GREATER:
		emitByte(OpCode::OP_GREATER);
		break;
	case TokenType::TOKEN_GREATER_EQUAL:
		emitBytes(OpCode::OP_LESS, OpCode::OP_NOT);
		break;
	case TokenType::TOKEN_LESS:
		emitByte(OpCode::OP_LESS);
		break;
	case TokenType::TOKEN_LESS_EQUAL:
		emitBytes(OpCode::OP_GREATER, OpCode::OP_NOT);
		break;
	default:
		return; // Unreachable.
	}
}

void Compiler::literal(bool canAssign)
{
	switch (this->parser->previous.type)
	{
	case TokenType::TOKEN_FALSE:
		this->emitByte(OpCode::OP_FALSE);
		break;
	case TokenType::TOKEN_NIL:
		this->emitByte(OpCode::OP_NIL);
		break;
	case TokenType::TOKEN_TRUE:
		this->emitByte(OpCode::OP_TRUE);
		break;
	default:
		return; // Unreachable.
	}
}

void Compiler::readString(bool canAssign)
{
	auto raw = this->parser->previous.lexeme;
	auto str = raw.substr(1, raw.length() - 2);
	this->emitConstant(str);
}

void Compiler::variable(bool canAssign)
{
	this->namedVariable(&this->parser->previous, canAssign);
}

void Compiler::declaration()
{
	if (this->match(TokenType::TOKEN_VAR))
    {
		this->varDeclaration();
	}
    else if (match(TokenType::TOKEN_FUN))
    {
        funDeclaration();
    }
	else
	{
		this->statement();
	}
	if (this->parser->panicMode) this->synchronize();
}

void Compiler::statement()
{
	if (this->match(TokenType::TOKEN_PRINT))
	{
		this->printStatement();
	}
    else if (this->match(TokenType::TOKEN_FOR))
    {
        this->forStatement();
    }
    else if (this->match(TokenType::TOKEN_IF))
    {
        this->ifStatement();
    }
    else if (this->match(TokenType::TOKEN_RETURN))
    {
        returnStatement();
    }
    else if (this->match(TokenType::TOKEN_WHILE))
    {
        this->whileStatement();
    }
	else if (this->match(TokenType::TOKEN_LEFT_BRACE))
	{
		this->beginScope();
		this->block();
		this->endScope();
	}
	else
    {
		this->expressionStatement();
	}
}

void Compiler::printStatement()
{
	this->expression();
	this->consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after value.");
	this->emitByte(OpCode::OP_PRINT);
}

void Compiler::expressionStatement()
{
	this->expression();
	this->consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after expression.");
	this->emitByte(OpCode::OP_POP);
}

void Compiler::varDeclaration()
{
	// read variable name
	uint8_t global = this->parseVariable("Expect variable name.");
	if (this->match(TokenType::TOKEN_EQUAL))
	{
		this->expression();
	}
	else
	{
		this->emitByte(OpCode::OP_NIL);
	}
	this->consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

	this->defineVariable(global);
}

Compiler::Compiler()
{
    this->initRules();
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

	bool canAssign = precedence <= Precedence::PREC_ASSIGNMENT;
	prefixRule(canAssign);

	while (precedence <= this->getRule(this->parser->current.type)->precedence)
	{
		this->advance();
		auto infixRule = this->getRule(parser->previous.type)->infix;
		infixRule(canAssign);
	}

	if (canAssign && this->match(TokenType::TOKEN_EQUAL))
	{
		this->error("Invalid assignment target.");
	}
}

void Compiler::namedVariable(Token* name, bool canAssign)
{
	OpCode getOp, setOp;
	int arg = this->resolveLocal(name);
	if (arg != -1)
	{
		getOp = OpCode::OP_GET_LOCAL;
		setOp = OpCode::OP_SET_LOCAL;
	}
    else if ((arg = resolveUpvalue(current, &name)) != -1)
    {
        getOp = OpCode::OP_GET_UPVALUE;
        setOp = OpCode::OP_SET_UPVALUE;
    }
	else
	{
		arg = identifierConstant(name);
		getOp = OpCode::OP_GET_GLOBAL;
		setOp = OpCode::OP_SET_GLOBAL;
	}
	if (canAssign && this->match(TokenType::TOKEN_EQUAL)) {
		this->expression();
		emitBytes(setOp, arg);
	} else {
		emitBytes(getOp, arg);
	}
}

void Compiler::block()
{
	while (!this->check(TokenType::TOKEN_RIGHT_BRACE) && !this->check(TokenType::TOKEN_EOF))
	{
		this->declaration();
	}

	this->consume(TokenType::TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

void Compiler::beginScope()
{
	this->scopeDepth++;
}

void Compiler::endScope()
{
	this->scopeDepth--;

	while (this->localCount > 0 && this->locals[this->localCount - 1].depth > this->scopeDepth)
	{
		emitByte(OpCode::OP_POP);
		this->localCount--;
	}
}

void Compiler::declareVariable()
{
	if (this->scopeDepth == 0) return;
	Token* name = &parser->previous;

	for (int32_t i = this->localCount - 1; i >= 0; i--)
	{
		Local* local = &this->locals[i];
		if (local->depth != -1 && local->depth < this->scopeDepth)
		{
			break;
		}

		if (identifiersEqual(name, &local->name))
		{
			this->error("Already a variable with this name in this scope.");
		}
	}

	this->addLocal(*name);
}

void Compiler::addLocal(Token name)
{
	if (this->localCount == std::numeric_limits<uint8_t>::max())
	{
		this->error("Too many local variables in function.");
		return;
	}

	Local* local = &this->locals[this->localCount++];
	local->name = std::move(name);
	local->depth = -1;
}

bool Compiler::identifiersEqual(Token* a, Token* b)
{
	return a->lexeme == b->lexeme;
}

int Compiler::resolveLocal(Token* name)
{
	for (int i = this->localCount - 1; i >= 0; i--)
	{
		Local* local = &this->locals[i];
		if (identifiersEqual(name, &local->name))
		{
			if (local->depth == -1) {
				this->error("Can't read local variable in its own initializer.");
			}
			return i;
		}
	}

	return -1;
}

void Compiler::markInitialized()
{
    if (this->scopeDepth == 0) return;
	this->locals[this->localCount - 1].depth = this->scopeDepth;
}

void Compiler::ifStatement() {
    this->consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    this->expression();
    this->consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int32_t thenJump = this->emitJump(OpCode::OP_JUMP_IF_FALSE);
    this->emitByte(OpCode::OP_POP);
    this->statement();
    int32_t elseJump = this->emitJump(OpCode::OP_JUMP);
    this->patchJump(thenJump);

    this->emitByte(OpCode::OP_POP);
    if (this->match(TokenType::TOKEN_ELSE))
    {
        this->statement();
    }
    this->patchJump(elseJump);
}

int32_t Compiler::emitJump(OpCode instruction) {
    this->emitByte(instruction);
    this->emitByte(0xff);
    this->emitByte(0xff);
    return this->currentChunk()->size() - 2;
}

Chunk *Compiler::currentChunk() {
    return this->function->getChunk();
}

void Compiler::patchJump(int32_t offset) {
    // -2 to adjust for the bytecode for the jump offset itself.
    int32_t jump = this->currentChunk()->size() - offset - 2;

    if (jump > UINT16_MAX) {
        this->error("Too much code to jump over.");
    }


    this->currentChunk()->at(offset) = (jump >> 8) & 0xff;
    this->currentChunk()->at(offset + 1) = jump & 0xff;
}

void Compiler::and_(bool canAssign)
{
    int32_t endJump = this->emitJump(OpCode::OP_JUMP_IF_FALSE);

    this->emitByte(OpCode::OP_POP);
    this->parsePrecedence(Precedence::PREC_AND);

    this->patchJump(endJump);
}

void Compiler::or_(bool canAssign)
{
    int32_t elseJump = emitJump(OpCode::OP_JUMP_IF_FALSE);
    int32_t endJump = emitJump(OpCode::OP_JUMP);

    this->patchJump(elseJump);
    this->emitByte(OpCode::OP_POP);

    this->parsePrecedence(Precedence::PREC_OR);
    this->patchJump(endJump);
}

void Compiler::whileStatement()
{
    int32_t loopStart = this->currentChunk()->size();
    this->consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    this->expression();
    this->consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int32_t exitJump = emitJump(OpCode::OP_JUMP_IF_FALSE);
    this->emitByte(OpCode::OP_POP);
    this->statement();

    this->emitLoop(loopStart);

    this->patchJump(exitJump);
    this->emitByte(OpCode::OP_POP);
}

void Compiler::emitLoop(int32_t loopStart)
{
    this->emitByte(OpCode::OP_LOOP);

    int32_t offset = this->currentChunk()->size() - loopStart + 2;
    if (offset > UINT16_MAX) error("Loop body too large.");

    this->emitByte((offset >> 8) & 0xff);
    this->emitByte(offset & 0xff);
}

void Compiler::forStatement()
{
    this->beginScope();
    this->consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
    if (this->match(TokenType::TOKEN_SEMICOLON)) {
        // No initializer.
    } else if (match(TokenType::TOKEN_VAR)) {
        this->varDeclaration();
    } else {
        this->expressionStatement();
    }

    int32_t loopStart = this->currentChunk()->size();
    int32_t exitJump = -1;
    if (!this->match(TokenType::TOKEN_SEMICOLON)) {
        this->expression();
        this->consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after loop condition.");

        // Jump out of the loop if the condition is false.
        exitJump = this->emitJump(OpCode::OP_JUMP_IF_FALSE);
        this->emitByte(OpCode::OP_POP); // Condition.
    }

    if (!match(TokenType::TOKEN_RIGHT_PAREN)) {
        int32_t bodyJump = emitJump(OpCode::OP_JUMP);
        int32_t incrementStart = this->currentChunk()->size();
        expression();
        emitByte(OpCode::OP_POP);
        consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

        emitLoop(loopStart);
        loopStart = incrementStart;
        patchJump(bodyJump);
    }

    this->statement();
    this->emitLoop(loopStart);

    if (exitJump != -1) {
        this->patchJump(exitJump);
        this->emitByte(OpCode::OP_POP); // Condition.
    }

    this->endScope();
}

void Compiler::funDeclaration()
{
    uint8_t global = this->parseVariable("Expect function name.");
    markInitialized();
    this->functionBody(FunctionType::TYPE_FUNCTION);
    defineVariable(global);
}

void Compiler::functionBody(FunctionType type)
{
    Compiler compiler(this, type);
    auto func = compiler.compileFunc(this->parser);
    emitBytes(OpCode::OP_CLOSURE, makeConstant(func));
}

Compiler::Compiler(Compiler *enclosing, FunctionType type)
{
    this->initRules();

    this->enclosing = enclosing;
    this->function = new ObjFunction();
    this->type = type;

    Local* local = &this->locals[this->localCount++];
    local->depth = 0;
    local->name.lexeme = "";
    local->name.length = 0;
}

ObjFunction *Compiler::compileFunc(Parser *p) {
    this->parser = p;

    this->beginScope();

    if (this->type != FunctionType::TYPE_SCRIPT) {
        this->function->name = this->parser->previous.lexeme;
    }

    this->consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after function name.");
    if (!this->check(TokenType::TOKEN_RIGHT_PAREN)) {
        do {
            this->function->arity++;
            if (this->function->arity > 255) {
                this->errorAtCurrent("Can't have more than 255 parameters.");
            }
            uint8_t constant = this->parseVariable("Expect parameter name.");
            this->defineVariable(constant);
        } while (match(TokenType::TOKEN_COMMA));
    }
    this->consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
    this->consume(TokenType::TOKEN_LEFT_BRACE, "Expect '{' before function body.");
    this->block();

    ObjFunction* func = endCompiler();

    return func;
}

void Compiler::initRules() {
    auto funcReadNumber = [this](bool canAssign)
    {
        this->number(canAssign);
    };

    auto funcGrouping = [this](bool canAssign)
    {
        this->grouping(canAssign);
    };

    auto funcUnary = [this](bool canAssign)
    {
        this->unary(canAssign);
    };

    auto funcBinary = [this](bool canAssign)
    {
        this->binary(canAssign);
    };

    auto funcLiteral = [this](bool canAssign)
    {
        this->literal(canAssign);
    };

    auto funcString = [this](bool canAssign)
    {
        this->readString(canAssign);
    };

    auto funcVariable = [this](bool canAssign)
    {
        this->variable(canAssign);
    };

    auto funcAnd = [this](bool canAssign)
    {
        this->and_(canAssign);
    };

    auto funcOr = [this](bool canAssign)
    {
        this->or_(canAssign);
    };

    auto funcCall = [this](bool canAssign)
    {
        this->call(canAssign);
    };

    this->rules = map<TokenType, ParseRule>{
            { TokenType::TOKEN_LEFT_PAREN,    { funcGrouping,   funcCall,    Precedence::PREC_CALL }},
            { TokenType::TOKEN_RIGHT_PAREN,   { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_LEFT_BRACE,    { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_RIGHT_BRACE,   { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_COMMA,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_DOT,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_MINUS,         { funcUnary,      funcBinary, Precedence::PREC_TERM }},
            { TokenType::TOKEN_PLUS,          { nullptr,        funcBinary, Precedence::PREC_TERM }},
            { TokenType::TOKEN_SEMICOLON,     { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_SLASH,         { nullptr,        funcBinary, Precedence::PREC_FACTOR }},
            { TokenType::TOKEN_STAR,          { nullptr,        funcBinary, Precedence::PREC_FACTOR }},
            { TokenType::TOKEN_BANG,          { funcUnary,      nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_BANG_EQUAL,    { nullptr,        funcBinary, Precedence::PREC_EQUALITY }},
            { TokenType::TOKEN_EQUAL,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_EQUAL_EQUAL,   { nullptr,        funcBinary, Precedence::PREC_EQUALITY }},
            { TokenType::TOKEN_GREATER,       { nullptr,        funcBinary, Precedence::PREC_COMPARISON }},
            { TokenType::TOKEN_GREATER_EQUAL, { nullptr,        funcBinary, Precedence::PREC_COMPARISON }},
            { TokenType::TOKEN_LESS,          { nullptr,        funcBinary, Precedence::PREC_COMPARISON }},
            { TokenType::TOKEN_LESS_EQUAL,    { nullptr,        funcBinary, Precedence::PREC_COMPARISON }},
            { TokenType::TOKEN_IDENTIFIER,    { funcVariable,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_STRING,        { funcString,     nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_NUMBER,        { funcReadNumber, nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_AND,           { nullptr,        funcAnd,    Precedence::PREC_AND }},
            { TokenType::TOKEN_CLASS,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_ELSE,          { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_FALSE,         { funcLiteral,    nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_FOR,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_FUN,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_IF,            { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_NIL,           { funcLiteral,    nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_OR,            { nullptr,        funcOr,    Precedence::PREC_OR }},
            { TokenType::TOKEN_PRINT,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_RETURN,        { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_SUPER,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_THIS,          { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_TRUE,          { funcLiteral,    nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_VAR,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_WHILE,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_ERROR,         { nullptr,        nullptr,    Precedence::PREC_NONE }},
            { TokenType::TOKEN_EOF,           { nullptr,        nullptr,    Precedence::PREC_NONE }},
    };
}

void Compiler::call(bool canAssign)
{
    uint8_t argCount = this->argumentList();
    this->emitBytes(OpCode::OP_CALL, argCount);
}

uint8_t Compiler::argumentList()
{
    uint8_t argCount = 0;
    if (!check(TokenType::TOKEN_RIGHT_PAREN)) {
        do {
            this->expression();
            if (argCount == 255) {
                error("Can't have more than 255 arguments.");
            }
            argCount++;
        } while (this->match(TokenType::TOKEN_COMMA));
    }
    this->consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
    return argCount;
}

void Compiler::returnStatement()
{
    if (this->type == FunctionType::TYPE_SCRIPT)
    {
        error("Can't return from top-level code.");
    }

    if (match(TokenType::TOKEN_SEMICOLON)) {
        emitReturn();
    } else {
        expression();
        consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after return value.");
        emitByte(OpCode::OP_RETURN);
    }
}
