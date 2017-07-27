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

using namespace goddard;

void fun1(void *s)
{
	puts("fun1 111");
	coroutine_yield((schedule *)s);
	puts("fun1 222");
	coroutine_yield((schedule *)s);
	puts("fun1 333");
	coroutine_yield((schedule *)s);
	puts("fun1 444");
	coroutine_yield((schedule *)s);
	puts("fun1 555");
}

void fun2(void *s)
{
	puts("fun2 111");
	coroutine_yield((schedule *)s);
	puts("fun2 222");
	coroutine_yield((schedule *)s);
	puts("fun2 333");
	coroutine_yield((schedule *)s);
	puts("fun2 444");
	coroutine_yield((schedule *)s);
	puts("fun2 555");
}


int main()
{
	schedule s;
	schedule_init(&s);
	int id1 = coroutine_create(&s, fun1, (void *)&s);
	int id2 = coroutine_create(&s, fun2, (void *)&s);

	coroutine_resume(&s, id2);
	coroutine_resume(&s, id1);
	while (1)
	{
		coroutine_resume(&s, id2);
		coroutine_resume(&s, id1);
	}
}
