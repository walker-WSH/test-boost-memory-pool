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

// 注意：
// 这个pool对象析构的时候 会释放所有通过其申请的堆内存
// 但是pool对象析构之前 里面申请的内存并不会自动被释放

static __declspec(thread) std::shared_ptr<pool<>> testMemPool = std::make_shared<pool<>>(ONE_MB);

void CallSimplePool()
{
	char *p = static_cast<char *>(testMemPool->malloc());
	assert(testMemPool->is_from(p));
	testMemPool->free(p);

	int *p2 = new int;
	assert(testMemPool->is_from(p2) == false);
	delete p2;

	for (int i = 0; i < 1024; ++i) {
		void *ret = testMemPool->malloc();
		ATLTRACE("[%d/%d] pointer: %p  tid:%u \n", i + 1, 1024, ret, GetCurrentThreadId());
		testMemPool->free(ret);
		Sleep(GetTickCount() * GetCurrentThreadId() % 200);
	}

	MessageBoxA(hMainWnd, "Test end", "Note", 0);
}

void CallCppPool()
{
	// 注意：
	// object_pool 不是线程安全的 当声明为全局变量时 多个线程同时访问就会偶现crash
	// https://stackoverflow.com/questions/19347890/how-to-get-boostobject-pool-thread-safe
	object_pool<CTest> pl;

	for (size_t i = 0; i < 2048; i++) {
		CTest *obj1 = pl.construct();
		CTest *obj2 = pl.construct(i);

		ATLTRACE("[%d/%d] pointer: %p  %p \n", i + 1, 2048, obj1, obj2);
		Sleep(GetTickCount() * GetCurrentThreadId() % 400);

		//注意：错误！这样调用 并不会调用析构函数
		//pl.free(obj1);
		//pl.free(obj2);

		pl.destroy(obj1);
		pl.destroy(obj2);
	}

	MessageBoxA(hMainWnd, "Test end", "Note", 0);
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
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testSimple, 0, 0);
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testSimple, 0, 0);
	_beginthreadex(0, 0, ThreadFunc, (void *)TestType::testSimple, 0, 0);
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
