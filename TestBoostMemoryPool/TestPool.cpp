#include "pch.h"
#include "TestBoostMemoryPoolDlg.h"
#include <iostream>
#include <Windows.h>
#include <assert.h>
#include <memory>

#include <boost/pool/pool.hpp>
using namespace boost;

const auto ONE_MB = (1024 * 1024);

void TestSimplePool()
{
	// 使用默认分配器，其内部使用new[]、delete[]分配内存
	// 注意：这个pool对象释放的时候 会释放所有通过其申请的堆内存
	std::shared_ptr<pool<>> testMemPool = std::make_shared<pool<>>(ONE_MB);

	char *p = static_cast<char *>(testMemPool->malloc());
	assert(testMemPool->is_from(p));
	testMemPool->free(p);

	int *p2 = new int;
	assert(testMemPool->is_from(p2) == false);
	delete p2;

	for (int i = 0; i < 1024; ++i) {
		void *ret = testMemPool->malloc();
		printf("ret: %p \n", ret);
		Sleep(50);
	}

	OutputDebugStringA("func to end, press any key to free all memory \n");
	Sleep(5000);
	// 函数结束时 pool对象会析构 其申请的所有堆内存 都会被释放
}

void LoopTestSimplePool()
{
	for (int i = 0; i < 100; ++i) {
		TestSimplePool();
	}
}

void CTestBoostMemoryPoolDlg::TestSimplePool()
{
	// TODO
}
