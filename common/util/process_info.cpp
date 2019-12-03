#include "process_info.h"

class FileOperatorGuard
{
public:
	FileOperatorGuard(const char *p)
	{
		fp = fopen(p, "r");
	}

	~FileOperatorGuard()
	{
		if (NULL != fp)
		{
			fclose(fp);
		}
	}

	operator FILE *()
	{
		return fp;
	}

	void reset(const char *p)
	{
		if (NULL != fp)
		{
			fclose(fp);
		}
		fp = fopen(p, "r");
	}

private:
	FILE* fp;
};

bool GetMemInfo(stMemInfo &info)
{
	FileOperatorGuard fOperator("/proc/meminfo");
	if (fOperator)
	{
		char name[32] = { 0 };
		fscanf(fOperator, "%s\t%lu", name, &info.totalSize);
	}
	else
	{
		LOG_ERROR("GetMemInfo open /proc/meminfo error");
		return false;
	}
	fOperator.reset("/proc/self/statm");
	if (fOperator)
	{
		fscanf(fOperator, "%lu\t%lu",&info.vsz, &info.rss);
	}
	else
	{
		LOG_ERROR("GetMemInfo open /proc/self/statm error");
		return false;
	}
	LOG_DEBUG("GetMemInfo succ info: ");
	info.LogDebug();
	return true;
}

struct stTotalCpuInfo
{
	stTotalCpuInfo() : user(0), nice(0), system(0), idle(0) {}
	unsigned long user;
	unsigned long nice;
	unsigned long system;
	unsigned long idle;

	operator unsigned long ()
	{
		return user + nice + system + idle;
	}

	void LogDebug(const char *prefix)
	{
		LOG_DEBUG("%s stTotalCpuInfo info utime: %lu, stime: %lu, cutime: %lu, cstime: %lu, total %lu \n",
			prefix, user, nice, system, idle, this->operator unsigned long ());
	}

	void LogError(const char *prefix)
	{
		LOG_ERROR("%s stTotalCpuInfo info utime: %lu, stime: %lu, cutime: %lu, cstime: %lu, total %lu \n",
			prefix, user, nice, system, idle, this->operator unsigned long ());
	}
};

struct stProcCpuInfo
{
	stProcCpuInfo() : utime(0), stime(0), cutime(0), cstime(0) {}
	unsigned long utime;  //user time
	unsigned long stime;  //kernel time
	unsigned long cutime; //all user time
	unsigned long cstime; //all dead time

	operator unsigned long ()
	{
		return utime + stime + cutime + cstime;
	}

	void LogDebug(const char *prefix)
	{
		LOG_DEBUG("%s stProcCpuInfo info utime: %lu, stime: %lu, cutime: %lu, cstime: %lu, total %lu \n",
			prefix, utime, stime, cutime, cstime, this->operator unsigned long ());
	}

	void LogError(const char *prefix)
	{
		LOG_ERROR("%s stProcCpuInfo info utime: %lu, stime: %lu, cutime: %lu, cstime: %lu, total %lu \n",
			prefix, utime, stime, cutime, cstime, this->operator unsigned long ());
	}
};

bool GetCpuUseInfo(int &cpuPercent)
{
	static stTotalCpuInfo lastTotalCpuInfo;
	static stProcCpuInfo lastProcCpuInfo;
	FileOperatorGuard fOperator("/proc/stat");
	stTotalCpuInfo totalCpuInfo;
	if (fOperator)
	{
		char name[32] = { 0 };
		fscanf(fOperator, "%s\t%lu\t%lu\t%lu\t%lu", name, &totalCpuInfo.user, &totalCpuInfo.nice, &totalCpuInfo.system, &totalCpuInfo.idle);
	}
	else
	{
		return false;
	}
	fOperator.reset("/proc/self/stat");
	stProcCpuInfo procCpuInfo;
	if (fOperator)
	{
		int pid;
		char comm[32] = { 0 };
		char state;
		int ppid;           
		int pgrp;
		int session;
		int tty_nr;
		int tpgid;
		unsigned int flags;
		unsigned long minflt;
		unsigned long cminflt;
		unsigned long majflt;
		unsigned long cmajflt;
		fscanf(fOperator, "%d\t%s\t%c\t%d\t%d\t%d\t%d\t%d\t%u\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",
			&pid, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags, &minflt, &cminflt, &majflt,
			&cmajflt, &procCpuInfo.utime, &procCpuInfo.stime, &procCpuInfo.cutime, &procCpuInfo.cstime);
	}
	else
	{
		return false;
	}
	int deltProc = procCpuInfo - lastProcCpuInfo;
	int deltTotal = totalCpuInfo - lastTotalCpuInfo;
	if (0 == deltTotal)
	{
		cpuPercent = 0;
		LOG_ERROR("GetCpuUseInfo fail deltTotal == 0 %d info :", deltProc);
		procCpuInfo.LogError("procCpuInfo");
		lastProcCpuInfo.LogError("lastProcCpuInfo");
		totalCpuInfo.LogError("totalCpuInfo");
		lastTotalCpuInfo.LogError("lastTotalCpuInfo");
		lastProcCpuInfo = procCpuInfo;
		lastTotalCpuInfo = totalCpuInfo;
	}
	else
	{
		cpuPercent = 100 * deltProc / deltTotal;
		LOG_DEBUG("GetCpuUseInfo succ %d %d %d info:", cpuPercent, deltProc, deltTotal);
		procCpuInfo.LogDebug("procCpuInfo");
		lastProcCpuInfo.LogDebug("lastProcCpuInfo");
		totalCpuInfo.LogDebug("totalCpuInfo");
		lastTotalCpuInfo.LogDebug("lastTotalCpuInfo");
		lastProcCpuInfo = procCpuInfo;
		lastTotalCpuInfo = totalCpuInfo;
	}
	return true;
}
