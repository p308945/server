/*
 * =====================================================================================
 *
 *       Filename:  redisManager.cpp
 *
 *    Description:  redisManager
 *
 *        Version:  1.0
 *        Created:  03/10/2017 01:04:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "redisManager.h"
#include "redisClusterProcess.h"
#include "redisCluster.h"
#include "redisSingle.h"
#include "redisCmdFormat.h"

namespace goddard
{
		bool RedisManager::init(const std::string &fileName)
		{
				if (!_config.init(fileName))
				{
						return false;
				}

				RedisCommon *instance = NULL;
				RedisCommonProcess *processor = NULL;
				bool b = false;
				switch(_config.getMode())
				{
						case eRedisCluster:
								{
										instance = new RedisCluster(&_config);
										processor = new RedisClusterProcess();
										if (!instance || !processor)
										{
												std::ostringstream oss;
												oss << "redis instance or processor new return NULL";
												print_error(oss.str());
												goto err;
										}
								}
								break;
						case eRedisSingle:
								{
										instance = new RedisSingle(&_config);
										processor = new RedisCommonProcess();
										if (!instance || !processor)
										{
												std::ostringstream oss;
												oss << "redis instance or processor new return NULL";
												print_error(oss.str());
												goto err;
										}
								}
								break;
						default:
								{
										std::ostringstream oss;
										oss << "mode error: " << _config.getMode();
										print_error(oss.str());
										goto err;
								}
								break;
				}
				b = instance->start();
				if (!b)
				{
						std::ostringstream oss;
						oss << "redis instance start failed" << _config.getMode();
						print_error(oss.str());
						goto err;
				}

				_component.uninstall();
				_component.install(instance, processor);
				return true;
err:
				if (instance)
				{
						delete instance;
				}
				if (processor)
				{
						delete processor;
				}
				return false;
		}

#define MACRO_REPLY_GET_INT64(reply, res)\
		if (reply->type != REDIS_REPLY_ERROR) \
		{	\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:	\
													{	\
															res = reply->integer;	\
													}	\
						break;	\
						case REDIS_REPLY_STRING:\
												{\
														res = ::atoll(reply->str);	\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_INT64_RET(reply, res, ret)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				ret = true;\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 res = reply->integer;	\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
												{\
														res = ::atoll(reply->str);	\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_INT64PTR_RET(reply, res, ret)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				ret = true;\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 if (res)\
														 *res = reply->integer;	\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
												{\
														if (res)\
														*res = ::atoll(reply->str);	\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_STRING_RET(reply, value, ret)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				ret = true;\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 char tmp[64] = { 0 };	\
														 sprintf(tmp, "%lld", reply->integer);	\
														 value.assign(tmp, strlen(tmp));\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
						case REDIS_REPLY_STATUS:\
												{\
														value.assign(reply->str, reply->len);\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_STRING(reply, value)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 char tmp[64] = { 0 };	\
														 sprintf(tmp, "%lld", reply->integer);	\
														 value.assign(tmp, strlen(tmp));\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
						case REDIS_REPLY_STATUS:\
												{\
														value.assign(reply->str, reply->len);\
												}\
						break;\
				}\
		}
		bool RedisManager::delKey(const char *key, int64_t &delCnt)
		{
				redisReply *reply = _component.fireCmd(key, DEL_KEY_FORMAT, key);
				bool ret = false;
				if (!ret)
				{
						return ret;
				}
				if (REDIS_REPLY_ERROR != reply->type)
				{
						MACRO_REPLY_GET_INT64_RET(reply, delCnt, ret);
				}
				freeReplyObject(reply);
				return ret;
		}

}
