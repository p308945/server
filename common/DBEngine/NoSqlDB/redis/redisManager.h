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
						bool existsKey(const char *key, int64_t &exists);	//0 exists, 1 not exists, 2 error
						bool expireKey(const char *key, const uint64_t sec, bool &setSucc)
						{
								return pexpireKey(key, sec * 1000, setSucc);
						}

						bool expireAtKey(const char *key, const uint64_t sec, bool &setSucc)
						{
								return pexpireAtKey(key, sec * 1000, setSucc);
						}
						bool pexpireKey(const char *key, uint64_t ms, bool &setSucc);
						bool pexpireAtKey(const char *key, const uint64_t ms, bool &setSucc);
						bool ttlKey(const char *key, int64_t &ttl)	//ttl -1 no ttl, permanent key; -2 key does not exists, positive ttl time
						{
								if (!pttlKey(key, ttl))
										return false;
								if (ttl > 0)
								{
										if (ttl < 1000)
												ttl = 1;
										else
												ttl /= 1000;

								}
								return true;
						}

						bool pttlKey(const char *key, int64_t &pttl);	//ttl -1 no ttl, permanent key; -2 key does not exists, positive ttl time
						bool typeKey(const char *key, std::string &typeStr, enum KEYTYPE &type);
						/* end key */

						/* string */
						bool decrString(const char *key, int64_t *rtn = NULL)
						{
								return incrByString(key, -1, rtn);
						}
						bool decrByString(const char *key, const int64_t by, int64_t *rtn = NULL)
						{
								return incrByString(key, -by, rtn);
						}
						bool getString(const char *key, std::string &value);
						bool incrString(const char *key, int64_t *rtn = NULL)
						{
								return incrByString(key, 1, rtn);
						}
						bool incrByString(const char *key, const int64_t by, int64_t *rtn = NULL);
						bool setString(const kvPairType &kvPair);
						/* end string */

						/* hash */
						bool hdelHash(const char *key, const char *field);
						bool hexistsHash(const char *key, const char *field, bool &exists);
						bool hgetHash(const char *key, const char *field, std::string &value);
						bool hgetallHash(const char *key, kvPairVecType &fieldValueVec /* field value */);
						bool hkeysHash(const char *key, stringVecType &fieldVec);
						bool hlenHash(const char *key, int64_t &len);
						bool hsetHash(const char *key, const char *field, const std::string &value);
						bool hvalsHash(const char *key, stringVecType &valueVec);
						bool hstrlenHash(const char *key, const char *field, int64_t &len);
						/* end hash */

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
