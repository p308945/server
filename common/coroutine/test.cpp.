/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  test
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

#include "coroutine.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <signal.h>

using namespace goddard;

void fun1(void *args)
{
	arg_coroutine *arg = (arg_coroutine *)args;
	puts("fun1 111");
	coroutine_yield((schedule *)arg->s);
	puts("fun1 222");
	coroutine_yield((schedule *)arg->s);
	puts("fun1 333");
	coroutine_yield((schedule *)arg->s);
	puts("fun1 444");
	coroutine_yield((schedule *)arg->s);
	puts("fun1 555");
	printf("finised %d\n", arg->id);
	free(arg);
	//coroutine_del(arg->s, arg->id);
}

void fun2(void *args)
{
	puts("fun2 111");
	arg_coroutine *arg = (arg_coroutine *)args;
	coroutine_yield((schedule *)arg->s);
	puts("fun2 222");
	coroutine_yield((schedule *)arg->s);
	puts("fun2 333");
	coroutine_yield((schedule *)arg->s);
	puts("fun2 444");
	coroutine_yield((schedule *)arg->s);
	puts("fun2 555");

	printf("finised %d\n", arg->id);
	free(arg);
}

#define MAX_COROUTINE_SIZE 100

schedule *s = NULL;

void sigHandle(int num)
{
	printf("xxxxxxxxxxxxxxxxxx");
	schedule_check(s, MAX_COROUTINE_SIZE);
	return;
}

int main()
{
	s = schedule_new();
	if (NULL == s)
	{
		return -1;
	}

	int count = 0;
	srandom(time(NULL));
	signal(SIGINT, sigHandle);

	while (1)
	{
		if (count < MAX_COROUTINE_SIZE)
		{
			++count;
			arg_coroutine *arg = (arg_coroutine*)malloc(sizeof(arg_coroutine));
			arg->s = s;
			arg->id = count;
			printf("create %d, count: %d\n", coroutine_create(s, fun1, (void *)arg), count);
		}
		else
		{
			if (0 == schedule_usecount(s))
			{
				schedule_destory(s);
				break;
			}
		}
		int r = 0;
		r = random() % count;
//		printf("coroutine_resume %d, use_count: %d\n", r, schedule_usecount(s));
		coroutine_resume(s, r);
		usleep(10);
	}
	puts("finished\n");
	getchar();
}
