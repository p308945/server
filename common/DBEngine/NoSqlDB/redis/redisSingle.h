/*
 * =====================================================================================
 *
 *       Filename:  redisSingle.h
 *
 *    Description:  redis single
 *
 *        Version:  1.0
 *        Created:  03/09/2017 07:36:09 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "redisCommon.h"

#ifndef __REDISSINGLE_H__
#define __REDISSINGLE_H__

namespace goddard
{
		class RedisSingle : public RedisCommon
		{
				public:
						RedisSingle(const RedisConfig *config) : RedisCommon(config)
						{
						}

						virtual bool start()
						{
								mNode.reset(getIP(), getPort(), getPasswd());
								bool b = mNode.online(getTimeout(), REDIS_ROLE_MASTER);
								if (!b)
								{
										std::ostringstream oss;
										oss << "start failed: online failed ip:" << getIP() << ", port:" << getPort();
										print_error(oss.str());
										goto err;
								}
								std::cout << "RedisSingle start success" << std::endl;
								return true;
err:
								reset();
								return false;
						}

						virtual RedisCommonNode* getRedisNode(const char *key)
						{
								(void)key;
								return &mNode;
						}
				protected:
						void reset()
						{
								mNode.reset();
						}
				private:
						RedisCommonNode mNode;
		};
}

#endif
