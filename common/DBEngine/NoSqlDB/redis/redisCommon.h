/*
 * =====================================================================================
 *
 *       Filename:  redisCommon.h
 *
 *    Description:  redisCommon
 *
 *        Version:  1.0
 *        Created:  03/09/2017 07:15:34 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __REDISCOMMON_H__
#define __REDISCOMMON_H__

#include <cstdint>
#include "redisNode.h"
#include "redisConfig.h"

namespace goddard
{
		enum KEYTYPE
		{
				eNone,
				eString,
				eList,
				eSet,
				eZSet,
				eHash,
		};

#define KEY_TYPE_STR_NONE "none"
#define KEY_TYPE_STR_STRING "string"
#define KEY_TYPE_STR_LIST "list"
#define KEY_TYPE_STR_SET "set"
#define KEY_TYPE_STR_ZSET "zset"
#define KEY_TYPE_STR_HASH "hash"

		class RedisCommon
		{
				public:
						RedisCommon(const RedisConfig *config) : _config(config)
				{
						_master_count = 0;
						_slave_count = 0;
				}

						virtual ~RedisCommon()
						{
						}

						void setConfig(const RedisConfig *config)
						{
								_config = config;
						}

				public:
						virtual bool start() = 0;
						virtual RedisCommonNode* getRedisNode(const char *key) = 0;

						uint32_t getMasterCount()
						{
								return _master_count;
						}

						uint32_t getSlaveCount()
						{
								return _slave_count;
						}

						void incrMasterCount()
						{
								++_master_count;
						}

						void incrSlaveCount()
						{
								++_slave_count;
						}

						void decrMasterCount()
						{
								--_master_count;
						}

						void decrSlaveCount()
						{
								--_slave_count;
						}

				protected:
						const char *getIP() const
						{
								return _config->getIP();
						}

						uint32_t getPort() const
						{
								return _config->getPort();
						}

						struct timeval getTimeout() const
						{
								return _config->getTimeout();
						}

						const std::string& getPasswd() const
						{
								return _config->getPasswd();
						}

						virtual void reset() = 0;

				protected:
						const RedisConfig *_config;
				private:
						uint32_t _master_count;
						uint32_t _slave_count;
		};
}

#endif
