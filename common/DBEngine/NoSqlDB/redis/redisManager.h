/*
 * =====================================================================================
 *
 *       Filename:  redisManager.h
 *
 *    Description:  redisManager
 *
 *        Version:  1.0
 *        Created:  03/10/2017 01:02:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef __REDISMANAGER_H__
#define __REDISMANAGER_H__

#include "redisConfig.h"
#include "redisCommon.h"
#include "redisComponent.h"
#include "TSingleton.h"

namespace goddard
{
		using kvPairType = std::pair<std::string, std::string>;
		using ckvPairType = std::pair<const std::string, std::string>;
		using kcvPairType = std::pair<std::string, const std::string>;
		using stringVecType = std::vector<std::string>;
		using kvPairVecType = std::vector<kvPairType>;
		using vsPairType = std::pair<std::string, double>;
		using vsPairVecType = std::vector<vsPairType>;
		using ddPairType = std::pair<double, double>;
		using sdPairType = std::pair<std::string, double>;
		using sdPairVecType = std::vector<sdPairType>;

		class RedisManager : public TSingleton<RedisManager>
		{
				public:
						friend class TSingleton<RedisManager>;
						~RedisManager() {}

						bool init(const std::string &fileName = "redis_config.conf");
						bool seterrcallback(void *(const std::string &));
						bool update();
						bool fini();

						/* key */
						bool delKey(const char *key, int64_t &delCnt);
						/* end key */

				private:
						RedisManager() {}
						RedisManager(const RedisManager &) = delete;
						RedisManager& operator = (const RedisManager &) = delete;

				private:
						RedisConfig _config;
						RedisComponent _component;
		};

}

#endif
