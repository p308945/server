/*
 * =====================================================================================
 *
 *       Filename:  redisConfig.h
 *
 *    Description:  config
 *
 *        Version:  1.0
 *        Created:  03/09/2017 03:43:52 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __REDISCONFIG_H__
#define __REDISCONFIG_H__

#include <vector>
#include <string>
#include <cstring>
#include "sds.h"
#include "util.h"

namespace goddard
{
		enum REDISMODE
		{
				eRedisInvalid,
				eRedisCluster,
				eRedisSingle,
		};

		struct stRedisConfigItem
		{
				stRedisConfigItem()
				{
						reset();
				}

				void reset()
				{
						_ip.clear();
						_port = 0;
						_mode = eRedisInvalid;
						memset(&_timeout, 0x0, sizeof(_timeout));
						_passwd.clear();
				}

				std::string _ip;
				uint16_t _port;
				REDISMODE _mode;
				struct timeval _timeout;
				std::string _passwd;

		};

		class RedisConfig
		{
				public:
						bool init(const std::string &fileName);
						const char *getIP() const
						{
								return _config._ip.c_str();
						}

						uint16_t getPort() const
						{
								return _config._port;
						}

						struct timeval getTimeout() const
						{
								return _config._timeout;
						}

						REDISMODE getMode() const
						{
								return _config._mode;
						}

						const std::string& getPasswd() const
						{
								return _config._passwd;
						}

				private:
						bool loadConfig(const std::string &fileName);
						bool loadConfigFromString(const char *config);
						sds *parseConfigLine(const char *line, uint32_t *argc);
						bool fetchOneConfigArgv(uint32_t argc, sds *argv);
				private:
						stRedisConfigItem _config;

		};
		
}

#endif
