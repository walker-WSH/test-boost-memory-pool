#include "pch.h"
#include "TestBoostMemoryPoolDlg.h"
#include <Windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <shlobj.h>
#include <intrin.h>
#include <psapi.h>

bool os_get_proc_memory_usage_internal(PROCESS_MEMORY_COUNTERS *pmc)
{
	if (!GetProcessMemoryInfo(GetCurrentProcess(), pmc, sizeof(*pmc)))
		return false;
	return true;
}

uint64_t os_get_proc_resident_size()
{
	PROCESS_MEMORY_COUNTERS pmc = {sizeof(PROCESS_MEMORY_COUNTERS)};
	if (!os_get_proc_memory_usage_internal(&pmc))
		return 0;
	return pmc.WorkingSetSize;
}

std::string ConvertMemory(DWORDLONG bytes)
{
	auto const ONE_KB = 1024.0;
	auto const ONE_MB = (ONE_KB * 1024.0);
	auto const ONE_GB = (ONE_MB * 1024.0);

	char retStr[MAX_PATH] = {0};
	double value = 0;

	if (bytes >= ONE_GB) {
		value = (double)bytes / ONE_GB;
		snprintf(retStr, MAX_PATH, "%.2lfGB", value);

	} else if (bytes >= ONE_MB) {
		value = (double)bytes / ONE_MB;
		snprintf(retStr, MAX_PATH, "%.2lfMB", value);

	} else if (bytes >= ONE_KB) {
		value = (double)bytes / ONE_KB;
		snprintf(retStr, MAX_PATH, "%.2lfKB", value);

	} else {
		value = (double)bytes;
		snprintf(retStr, MAX_PATH, "%.2lfByte", value);
	}

	return retStr;
}

std::string CTestBoostMemoryPoolDlg::GetUsedMemory()
{
	uint64_t bytes = os_get_proc_resident_size();
	std::string str = ConvertMemory(bytes);
	return str;
}
