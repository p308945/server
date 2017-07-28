/*
 * =====================================================================================
 *
 *       Filename:  coroutine.cpp
 *
 *    Description:  coroutine
 *
 *        Version:  1.0
 *        Created:  07/27/2017 17:21:29 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <cstdlib>
#include <cstring>
#include "coroutine.h"

namespace goddard
{

	bool schedule_init(schedule *s)
	{
		s->cos = NULL;
		s->running_id = -1;
		s->use_count = 0;
		s->max_size = DEFAULT_COROUTINE_COUNT;
		s->cos = (coroutine *)malloc(sizeof(coroutine) * s->max_size);
		if (NULL == s->cos)
		{
			return false;
		}
		memset(s->cos, 0, sizeof(coroutine) * s->max_size);
		return true;
	}

	void schedule_destory(schedule *s)
	{
		if (NULL != s->cos)
		{
			free(s->cos);
			s->cos = NULL;
		}
		s->running_id = -1;
		s->use_count = 0;
		s->max_size = DEFAULT_COROUTINE_COUNT;
	}

	int coroutine_create(schedule *s, CoroutineFun fun, void *args)
	{
		if (s->use_count < s->max_size)
		{
			for (int i = 0; i < s->max_size; ++i)
			{
				int id = (i + s->use_count) % s->max_size;
				if (CoroutineDead == s->cos[id].status)
				{
					coroutine *co = &s->cos[id];
					co->fun = fun;
					co->args = args;
					co->status = CoroutineReady;
					memset(co->stack, 0, sizeof(co->stack));
					++s->use_count;
					return id;
				}

			}
		}
		else
		{
			coroutine *p = (coroutine *)realloc(s->cos, s->max_size * 2 * sizeof(struct coroutine));
			if (NULL == p)
			{
				return -1;
			}
			int id = s->max_size;
			s->cos = p;
			memset(s->cos + s->max_size, 0, sizeof(coroutine) * s->max_size);
			s->max_size *= 2;
			++s->use_count;
			return id;
		}
		return -1;
	}

	static void schedule_fun(schedule *s)
	{
		int id = s->running_id;
		coroutine *co = &s->cos[id];
		co->fun(co->args);
		co->status = CoroutineDead;
		s->running_id = -1;
		--s->use_count;
	}

	void coroutine_resume(schedule *s, int id)
	{
		if (id < 0 || id >= s->max_size)
		{
			return;
		}
		coroutine *co = &s->cos[id];
		switch(co->status)
		{
			case CoroutineReady:
				{
					getcontext(&co->c);
					co->c.uc_stack.ss_sp = co->stack;
					co->c.uc_stack.ss_size = sizeof(co->stack);
					co->c.uc_link = &s->main;
					s->running_id = id;
					co->status = CoroutineRunning;
					makecontext(&co->c, (void (*)())schedule_fun, 1, s);
					swapcontext(&s->main, &co->c);
				}
				break;
			case CoroutineSuspend:
				{
					s->running_id = id;
					co->status = CoroutineRunning;
					swapcontext(&s->main, &co->c);
				}
				break;
			default:
				{
				}
				break;
		}
	}

	void coroutine_yield(schedule *s)
	{
		if (-1 == s->running_id)
		{
			return;
		}
		coroutine *co = &s->cos[s->running_id];
		s->running_id = -1;
		co->status = CoroutineSuspend;
		swapcontext(&co->c, &s->main);
	}
}
