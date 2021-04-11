//
// Created by tjp56 on 2021/4/11.
//

#include "gtest/gtest.h"
#include "glog/logging.h"
#include "Scanner.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "absl/strings/str_format.h"

string readFile(const string& path)
{
	std::ifstream file;
	file.open(path);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string fileContent(buffer.str());
		return fileContent;
	}
	LOG(ERROR) << "OPEN FILE FAILED !!!";
	return string{};
}


TEST(testScanner, test1) {
	auto path = "../../Resource/test/testScanner.lox";
	auto source = readFile(path);
	using absl::StrFormat;

	auto scanner = Scanner(source);
	int line = -1;
	for (;;) {
		Token token = scanner.scanToken();
		string lineNumber = token.line != line ? StrFormat("%4d ", token.line) : "   | ";
		LOG(INFO) << StrFormat("%2d '%s'\n", token.type, token.lexeme);

		if (token.type == TokenType::TOKEN_EOF)
			break;
	}
}
