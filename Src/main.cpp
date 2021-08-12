#include <iostream>
#include "common.h"
#include "glog/logging.h"   // glog 头文件
#include "chunk.h"
#include "Debug.h"
#include "vm.h"

int main(int argc, char** argv)
{
	FLAGS_logtostderr = true;  //输出到控制台
	google::InitGoogleLogging(argv[0]);    // 初始化

	auto v = vm();
//	v.execute(argc, argv);
	v.runFile("../Resource/test/testPrint.lox");

	return 0;
}
