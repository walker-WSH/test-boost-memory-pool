#include "pch.h"
#include "TestBoostMemoryPoolDlg.h"
#include <iostream>
#include <Windows.h>
#include <assert.h>
#include <memory>
#include <process.h>

#include <boost/pool/pool.hpp>
using namespace boost;

const auto ONE_MB = (1024 * 1024);
extern HWND hMainWnd;

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
		ATLTRACE("[%d/%d] pointer: %p \n", i + 1, 1024, ret);
		Sleep(10);
	}

	OutputDebugStringA("func to end, press any key to free all memory \n");
	MessageBoxA(hMainWnd, "Click OK to free all memory", "Note", 0);
	// 函数结束时 pool对象会析构 其申请的所有堆内存 都会被释放
}

//--------------------------------------------------------------------------------
enum class TestType {
	testSimple = 0,
};

static unsigned __stdcall ThreadFunc(void *pParam)
{
	switch (static_cast<enum TestType>((int)pParam)) {
	case TestType::testSimple:
		TestSimplePool();
		break;

	default:
		break;
	}

	return 0;
}

void CTestBoostMemoryPoolDlg::TestSimplePool()
{
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testSimple, 0, 0);
}
