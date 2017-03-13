/*
 * =====================================================================================
 *
 *       Filename:  redisComponent.h
 *
 *    Description:  redisComponent
 *
 *        Version:  1.0
 *        Created:  03/10/2017 12:49:41 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __REDISCOMPONENT_H__
#define __REDISCOMPONENT_H__

#include "redisCommon.h"
#include "redisCommProcess.h"

namespace goddard
{
		class RedisComponent
		{
				public:
						RedisComponent() : _redisInstance(NULL), _redisProcessor(NULL)
				{

				}

						~RedisComponent()
						{
								uninstall();
						}

						void uninstall()
						{
								if (_redisInstance)
								{
										delete _redisInstance;
										_redisInstance = NULL;
								}
								if (_redisProcessor)
								{
										delete _redisProcessor;
										_redisProcessor = NULL;
								}
						}

						void install(RedisCommon *instance, RedisCommonProcess *processor)
						{
								_redisInstance = instance;
								_redisProcessor = processor;
						}

						redisReply *fireCmd(const char *key, const char *format, ...)
						{
								if (NULL == _redisProcessor)
								{
										return NULL;
								}
								va_list ap;
								va_start(ap, format);
								return _redisProcessor->execCommand(_redisInstance, key, format, ap);
						}

				private:
						RedisCommon *_redisInstance;
						RedisCommonProcess *_redisProcessor;

		};
}

#endif
