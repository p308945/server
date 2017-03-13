/*
 * =====================================================================================
 *
 *       Filename:  redisClusterProcess.cpp
 *
 *    Description:  redisClusterProcess
 *
 *        Version:  1.0
 *        Created:  03/10/2017 12:05:42 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "redisClusterProcess.h"
#include "redisCluster.h"

namespace goddard
{
		int RedisClusterProcess::errProcess(cmd_st *cmd)
		{
				RedisCluster *cRedis = dynamic_cast<RedisCluster *>(cmd->redis);
				if (NULL == cRedis)
				{
						std::ostringstream oss;
						oss << "dynamic_cast<RedisCluster *> return NULL";
						print_error(oss.str());
						return REDIS_ERR_PROCESS_CODE_ERROR;
				}
				int err = getErrorType(cmd->ireply);
				int ret = REDIS_ERR_PROCESS_CODE_ERROR;
				if (REDIS_ERR_PROCESS_CODE_OK != err)
				{
						switch(err)
						{
								case REPLY_CLUSTER_ERR_MOVED:
								case REPLY_CLUSTER_ERR_ASK:
										{
												if (cRedis->processMovedAsk(cmd, REPLY_CLUSTER_ERR_ASK == err))
												{
														ret = REDIS_ERR_PROCESS_CODE_REPROCESS;
												}
												else
												{
														ret = REDIS_ERR_PROCESS_CODE_ERROR;
												}

										}
										break;
								case REPLY_ERR_TRYAGAIN:
								case REPLY_CLUSTER_ERR_CROSSSLOT:
										{
												ret = REDIS_ERR_PROCESS_CODE_RETRY;
										}
										break;
								case REPLY_CLUSTER_ERR_CLUSTERDOWN:
								case REPLY_CLUSTER_ERR_SENTINEL:
								case REPLY_ERR_UNKNOWN:
										{
												ret = REDIS_ERR_PROCESS_CODE_ERROR;
										}
										break;
								default:
										{
												ret = REDIS_ERR_PROCESS_CODE_ERROR;
										}
										break;

						}

				}
				else
				{
						ret = REDIS_ERR_PROCESS_CODE_OK;
				}
				return ret;
		}
}
