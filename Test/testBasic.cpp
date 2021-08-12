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