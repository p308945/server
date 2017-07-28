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
	struct schedule;
	typedef void(*CoroutineFun)(void *);

	struct arg_coroutine
	{
		schedule *s;
		int id;
	};

	schedule* schedule_new();
	void schedule_destory(schedule *s);
	int schedule_usecount(schedule *s);
	void schedule_check(schedule *s, int num);
	int coroutine_create(schedule *s, CoroutineFun fun, void *args);
	void coroutine_resume(schedule *s, int id);
	void coroutine_yield(schedule *s);
	void coroutine_del(schedule *s, int id);
}

#endif
