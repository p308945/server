/*
 * =====================================================================================
 *
 *       Filename:  TSingleton.h
 *
 *    Description:  TSingleton
 *
 *        Version:  1.0
 *        Created:  03/10/2017 01:13:22 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __TSINGLETON_H__
#define __TSINGLETON_H__

#include <pthread.h>

namespace goddard
{

		template <typename T>
				class TSingleton
				{
						public:
								static T& getInstance()
								{
										pthread_once(&_ponce, &TSingleton::init);
										return *instance;
								}
						protected:
								TSingleton() {}
								TSingleton(const TSingleton &) {}
						private:
								static void init()
								{
										instance = new T();
								}

						private:
								static pthread_once_t _ponce;
								static T *instance;
				};

		template <typename T>
				pthread_once_t TSingleton<T>::_ponce = PTHREAD_ONCE_INIT;

		template <typename T>
				T* TSingleton<T>::instance = NULL;
}

#endif
