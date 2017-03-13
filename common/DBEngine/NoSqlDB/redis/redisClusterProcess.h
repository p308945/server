/*
 * =====================================================================================
 *
 *       Filename:  redisClusterProcess.h
 *
 *    Description:  redisClusterProcess
 *
 *        Version:  1.0
 *        Created:  03/10/2017 12:02:32 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __REDISCLUSTERPROCESS_H__
#define __REDISCLUSTERPROCESS_H__

#include "redisCommProcess.h"

namespace goddard
{
		class RedisClusterProcess : public RedisCommonProcess
		{
				public:
						virtual int errProcess(cmd_st *cmd);

		};
}


#endif
