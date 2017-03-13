/*
 * =====================================================================================
 *
 *       Filename:  redisCommProcess.cpp
 *
 *    Description:  redisCommProcess
 *
 *        Version:  1.0
 *        Created:  03/09/2017 11:07:10 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <cassert>
#include <cstdarg>
#include "redisCommProcess.h"
#include "redisSingle.h"
#include "redisCluster.h"

namespace goddard
{
		static const char *REDIS_ERR_TRYAGAIN_STR = "TRYAGAIN";
		static const char *REDIS_ERR_CROSSSLOT_STR = "CROSSSLOT";
		static const char *REDIS_ERR_CLUSTERDOWN_STR = "CLUSTERDOWN";
		static const char *REDIS_ERR_MOVED_STR = "MOVED";
		static const char *REDIS_ERR_ASK_STR = "ASK";

		int RedisCommonProcess::getErrorType(redisReply *reply)
		{
				if (NULL == reply)
				{
						return REPLY_ERR_NULL_REPLY;
				}
				if (REDIS_REPLY_ERROR == reply->type)
				{
						if (strlen(REDIS_ERR_MOVED_STR) <= reply->len &&
										0 == strncasecmp(reply->str, REDIS_ERR_MOVED_STR, strlen(REDIS_ERR_MOVED_STR)))
						{
								return REPLY_CLUSTER_ERR_MOVED;
						}
						else if (strlen(REDIS_ERR_ASK_STR) <= reply->len &&
										0 == strncasecmp(reply->str, REDIS_ERR_ASK_STR, strlen(REDIS_ERR_ASK_STR)))
						{
								return REPLY_CLUSTER_ERR_ASK;
						}
						else if (strlen(REDIS_ERR_TRYAGAIN_STR) <= reply->len &&
										0 == strncasecmp(reply->str, REDIS_ERR_TRYAGAIN_STR, strlen(REDIS_ERR_TRYAGAIN_STR)))
						{
								return REPLY_ERR_TRYAGAIN;
						}
						else if (strlen(REDIS_ERR_CROSSSLOT_STR) <= reply->len &&
										0 == strncasecmp(reply->str, REDIS_ERR_CROSSSLOT_STR, strlen(REDIS_ERR_CROSSSLOT_STR)))
						{
								return REPLY_CLUSTER_ERR_CROSSSLOT;
						}
						else if (strlen(REDIS_ERR_CLUSTERDOWN_STR) <= reply->len &&
										0 == strncasecmp(reply->str, REDIS_ERR_CLUSTERDOWN_STR, strlen(REDIS_ERR_CLUSTERDOWN_STR)))
						{
								return REPLY_CLUSTER_ERR_CLUSTERDOWN;
						}
						else
						{
								return REPLY_ERR_UNKNOWN;
						}
				}
				return REPLY_ERR_OK;
		}

		int RedisCommonProcess::errProcess(cmd_st *cmd)
		{
				return REDIS_ERR_PROCESS_CODE_OK;
		}

		redisReply *RedisCommonProcess::execCommand(RedisCommon *redis, const char *key, const char *format, va_list ap)
		{
retry:
				va_list ap1;
				va_copy(ap1, ap);
				RedisCommonNode *node = redis->getRedisNode(key);
				if (NULL == node)
				{
						std::ostringstream oss;
						oss << "getRedisNode return NULL, key: " << key;
						print_error(oss.str());
						va_end(ap1);
						return NULL;
				}

				redisContext *ctx = node->getCtx();
				if (NULL == ctx)
				{
						std::ostringstream oss;
						oss << "getCtx return NULL, key: " << key;
						print_error(oss.str());
						va_end(ap1);
						return NULL;
				}

				redisReply *reply = NULL;

				cmd_st cmd;
				reply = (redisReply *)redisvCommand(ctx, format, ap1);
				va_end(ap1);
				if (reply && REDIS_REPLY_ERROR == reply->type)
				{
						cmd.redis = redis;
						cmd.node = node;
						cmd.ireply = reply;
						cmd.oreply = NULL;
						cmd.format = format;
						va_copy(cmd.ap, ap);
						int err = errProcess(&cmd);
						va_end(cmd.ap);
						switch(err)
						{
								case REDIS_ERR_PROCESS_CODE_OK:
										{

										}
										break;
								case REDIS_ERR_PROCESS_CODE_RETRY:
										{
												if (NULL != reply)
												{
														freeReplyObject(reply);
												}
												goto retry;
										}
										break;
								case REDIS_ERR_PROCESS_CODE_REPROCESS:
										{
												if (NULL != reply)
												{
														freeReplyObject(reply);
														cmd.ireply = NULL;
												}
												reply = cmd.oreply;
										}
										break;
								default:
										{
										}
										break;
						}
				}
				return reply;
		}

}
