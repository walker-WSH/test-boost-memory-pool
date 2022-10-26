#include "pch.h"
#include "TestBoostMemoryPoolDlg.h"
#include <iostream>
#include <Windows.h>
#include <assert.h>
#include <memory>
#include <process.h>

#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

using namespace boost;

const auto ONE_MB = (1024 * 1024);
extern HWND hMainWnd;

struct CTest {
public:
	CTest() { ATLTRACE("%p created \n", this); }
	CTest(int a) : m_buf(new char[ONE_MB])
	{
		ATLTRACE("%p created with value : %d \n", this, a);
	}
	virtual ~CTest() { ATLTRACE("%p deleted \n", this); }

private:
	std::shared_ptr<char> m_buf;
};

void CallSimplePool()
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

	MessageBoxA(hMainWnd, "Click OK to free all memory", "Note", 0);
	// 函数结束时 pool对象会析构 其申请的所有堆内存 都会被释放
}

void CallCppPool()
{
	object_pool<CTest> pl;

	for (size_t i = 0; i < 2048; i++) {
		CTest *obj1 = pl.construct();
		CTest *obj2 = pl.construct(i);

		ATLTRACE("[%d/%d] pointer: %p  %p \n", i + 1, 2048, obj1, obj2);
		Sleep(GetTickCount() * GetCurrentThreadId() % 400);

		//错误！这样调用 并不会调用析构函数
		//pl.free(obj1);
		//pl.free(obj2);

		pl.destroy(obj1);
		pl.destroy(obj2);
	}

	MessageBoxA(hMainWnd, "Test end.\n Click OK to free all memory", "Note", 0);
	// 函数结束时 pool对象会析构 其申请的所有堆内存 都会被释放
}

//--------------------------------------------------------------------------------
enum class TestType {
	testSimple = 0,
	testCppPool,
};

static unsigned __stdcall ThreadFunc(void *pParam)
{
	switch (static_cast<enum TestType>((int)pParam)) {
	case TestType::testSimple:
		CallSimplePool();
		break;

	case TestType::testCppPool:
		CallCppPool();
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

void CTestBoostMemoryPoolDlg::TestCppPool()
{
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testCppPool, 0, 0);
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testCppPool, 0, 0);
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testCppPool, 0, 0);
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testCppPool, 0, 0);
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testCppPool, 0, 0);
}
