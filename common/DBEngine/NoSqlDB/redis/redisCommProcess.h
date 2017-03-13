/*
 * =====================================================================================
 *
 *       Filename:  redisCommProcess.h
 *
 *    Description:  common Process
 *
 *        Version:  1.0
 *        Created:  03/09/2017 10:18:28 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __REDISCOMMPROCESS_H__
#define __REDISCOMMPROCESS_H__

#include "hiredis.h"

namespace goddard
{
		class RedisCommon;
		class RedisCommonNode;

		struct cmd_st
		{
				RedisCommon *redis;
				RedisCommonNode *node;
				redisReply *ireply;
				redisReply *oreply;
				const char *format;
				va_list ap;
		};

		class RedisCommonProcess
		{
				public:
						enum REDIS_REPLY_ERR_TYPE
						{
								REPLY_ERR_OK = 0,
								REPLY_ERR_NULL_REPLY = 0,
								REPLY_CLUSTER_ERR_MOVED,
								REPLY_CLUSTER_ERR_ASK,
								REPLY_ERR_TRYAGAIN,
								REPLY_CLUSTER_ERR_CROSSSLOT,
								REPLY_CLUSTER_ERR_CLUSTERDOWN,
								REPLY_CLUSTER_ERR_SENTINEL,


								REPLY_ERR_UNKNOWN,
						};

						enum REDIS_ERR_PROCESS_RET_CODE
						{
								REDIS_ERR_PROCESS_CODE_OK = 0,
								REDIS_ERR_PROCESS_CODE_RETRY,
								REDIS_ERR_PROCESS_CODE_REPROCESS,
								REDIS_ERR_PROCESS_CODE_ERROR,
						};

				public:
						RedisCommonProcess()
						{
						}

						virtual ~RedisCommonProcess()
						{
						}

				public:
						redisReply *execCommand(RedisCommon *redis, const char *key, const char *format, va_list ap);
						virtual int errProcess(cmd_st *cmd);

				protected:
						virtual int getErrorType(redisReply *reply);

		};
		
}

#endif
