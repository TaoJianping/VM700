//
// Created by tao on 2021/8/12.
//

#include "gtest/gtest.h"
#include "glog/logging.h"
#include "../Src/vm.h"

TEST(testProgram, test1) {
	FLAGS_logtostderr = true;  //输出到控制台
//	google::InitGoogleLogging(argv[0]);    // 初始化
	const std::string path = "../../Resource/test/testAssignment.lox";
	auto v = vm();
	//	v.execute(argc, argv);
	v.runFile(path);
}

TEST(testProgram, test2) {
	FLAGS_logtostderr = true;  //输出到控制台
	//	google::InitGoogleLogging(argv[0]);    // 初始化
	const std::string path = "../../Resource/test/testLocal.lox";
	auto v = vm();
	//	v.execute(argc, argv);
	v.runFile(path);
}

TEST(testProgram, testIf) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testIf.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testAnd) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testAnd.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testOr) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testOr.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testWhile) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testWhile.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testFor1) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testFor.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testFor2) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testFor2.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testFunc1) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testFunc.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testFunc2) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testFun2.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testFunc3) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testFun3.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testFunc4) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testFun4.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testFun5) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testFun5.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testClosure) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testClosure.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testClosure2) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testClosure2.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testClosure3) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testClosure3.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}

TEST(testProgram, testClosure4) {
    FLAGS_logtostderr = true;  //输出到控制台
    //	google::InitGoogleLogging(argv[0]);    // 初始化
    const std::string path = "../../Resource/test/testClosure4.lox";
    auto v = vm();
    //	v.execute(argc, argv);
    v.runFile(path);
}