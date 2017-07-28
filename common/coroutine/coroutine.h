/*
 * =====================================================================================
 *
 *       Filename:  coroutine.h
 *
 *    Description:  coroutine
 *
 *        Version:  1.0
 *        Created:  07/27/2017 17:22:39 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include <ucontext.h>

namespace goddard
{
	const int DEFAULT_COROUTINE_COUNT = 10;
	const int DEFAULT_STACK_SIZE = 128 * 1024;
	typedef void(*CoroutineFun)(void *);

	enum CoroutineStatus
	{
		CoroutineDead = 0,
		CoroutineReady,
		CoroutineRunning,
		CoroutineSuspend,
	};

	struct coroutine
	{
		ucontext c;
		CoroutineFun fun;
		void *args;
		CoroutineStatus status;
		char stack[DEFAULT_STACK_SIZE];
	};

	struct schedule
	{
		ucontext main;
		coroutine *cos;
		int running_id;
		int use_count;
		int max_size;
	};

	bool schedule_init(schedule *s);
	void schedule_destory(schedule *s);
	int coroutine_create(schedule *s, CoroutineFun fun, void *args);
	void coroutine_resume(schedule *s, int id);
	void coroutine_yield(schedule *s);
}

#endif
