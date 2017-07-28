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
	const int DEFAULT_COROUTINE_COUNT = 10;
	const int DEFAULT_STACK_SIZE = 128 * 1024;

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
		coroutine **cos;
		int running_id;
		int use_count;
		int max_size;
	};

	static bool schedule_init(schedule *s)
	{
		s->cos = NULL;
		s->running_id = -1;
		s->use_count = 0;
		s->max_size = DEFAULT_COROUTINE_COUNT;
		s->cos = (coroutine **)malloc(sizeof(coroutine *) * s->max_size);
		if (NULL == s->cos)
		{
			return false;
		}
		memset(s->cos, 0, sizeof(coroutine *) * s->max_size);
		return true;
	}

	schedule* schedule_new()
	{
		schedule *s = (schedule *)malloc(sizeof(schedule));
		if (schedule_init(s))
		{
			return s;
		}
		schedule_destory(s);
		return NULL;
	}


	void schedule_destory(schedule *s)
	{
		if (NULL == s)
		{
			return;
		}
		for (int i = 0; i < s->max_size; ++i)
		{
			coroutine *co = s->cos[i];
			if (NULL != co)
			{
				free(co);
			}
		}
		free(s->cos);
		free(s);
	}

	int coroutine_create(schedule *s, CoroutineFun fun, void *args)
	{
		if (s->use_count < s->max_size)
		{
			for (int i = 0; i < s->max_size; ++i)
			{
				int id = (i + s->use_count) % s->max_size;
				coroutine *co = s->cos[id];
				if (NULL == co)
				{
					co = (coroutine *)malloc(sizeof(coroutine));
					if (NULL == co)
					{
						return -1;
					}
					s->cos[id] = co;
					co->status = CoroutineDead;
				}
				if (CoroutineDead == co->status)
				{
					co->fun = fun;
					co->args = args;
					co->status = CoroutineReady;
//					memset(co->stack, 0, sizeof(co->stack));
					++s->use_count;
					return id;
				}

			}
		}
		else
		{
			coroutine **p = (coroutine **)realloc(s->cos, s->max_size * 2 * sizeof(struct coroutine *));
			if (NULL == p)
			{
				return -1;
			}
			int id = s->max_size;
			s->cos = p;
			memset(s->cos + s->max_size, 0, sizeof(coroutine *) * s->max_size);
			s->max_size *= 2;
			++s->use_count;
			return id;
		}
		return -1;
	}

	static void schedule_fun(schedule *s)
	{
		int id = s->running_id;
		coroutine *co = s->cos[id];
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
		coroutine *co = s->cos[id];
		if (NULL == co)
		{
			return;
		}
		switch(co->status)
		{
			case CoroutineReady:
				{
					getcontext(&co->c);
					co->c.uc_stack.ss_sp = co->stack;
					co->c.uc_stack.ss_size = sizeof(co->stack);
					co->c.uc_flags = 0;
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
		coroutine *co = s->cos[s->running_id];
		s->running_id = -1;
		co->status = CoroutineSuspend;
		swapcontext(&co->c, &s->main);
	}
}
