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
}
