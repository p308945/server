#ifndef _PROCESS_INFO_H_
#define _PROCESS_INFO_H_

#include <cstdint>
#include <cstdio>
#include <string>
#include "util/Log.h"

struct stMemInfo
{
	stMemInfo() : totalSize(0), vsz(0), rss(0) {}
	unsigned long totalSize;
	unsigned long vsz;
	unsigned long rss;
	void LogDebug()
	{
		LOG_DEBUG("stMemInfo info totalSize: %lu, rss: %lu, vsz: %lu", totalSize, vsz, rss);
	}

	void LogError()
	{
		LOG_ERROR("stMemInfo info totalSize: %lu, rss: %lu, vsz: %lu", totalSize, vsz, rss);
	}
};

bool GetMemInfo(stMemInfo &info);

bool GetCpuUseInfo(int &cpuPercent);

#endif
