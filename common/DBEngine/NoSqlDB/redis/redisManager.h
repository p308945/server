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
 *       Compiler:  g++
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

						/* list */
						bool lindexList(const char *key, const int64_t idx, std::string &value);
						bool llenList(const char *key, int64_t &len);
						bool lpopList(const char *key, std::string &value, bool saveValue = true);
						bool lpushList(const char *key, const std::string &value);
						bool lrangeList(const char *key, const int64_t start, const int64_t stop, stringVecType &valueVec); //[start, stop], [0, (len - 1)] or [-1, -len]
						bool lremList(const char *key, const int64_t count, const std::string &value, int64_t &rtnCount); //if count > 0 from head to tail remove count items, count < 0 from tail to head remove |count| items, count == 0 remove all equal to value
						bool lsetList(const char *key, const int64_t index, const std::string &value);
						bool ltrimList(const char *key, const int64_t start, const int64_t stop);	//[start, stop], [0, len - 1]. if start > stop all item will be removed
						bool rpopList(const char *key, std::string &value, bool saveValue = true);
						bool rpushList(const char *key, const std::string &value);
						/* end list */

						/* set */
						bool saddSet(const char *key, const std::string &value, int64_t &num);
						bool scardSet(const char *key, int64_t &count);
						bool sismemberSet(const char *key, const std::string &value, bool &ismember);
						bool smembersSet(const char *key, stringVecType &valueVec);
						bool spopSet(const char *key, std::string &value);
						bool srandmemberSet(const char *key, stringVecType &valueVec, const int64_t count = 1);	//if count > 0 count = count < set.totalCnt ? count : set.totalCnt, return count elements, all elements are different; if count < 0, return |count| elements, each element  may be repeated many times
						bool sremSet(const char *key, const std::string &value, int64_t &remCnt);
						/* end set */

						/* sortset */
						bool zaddSortset(const char *key, const std::string &value, const double score, int64_t &count);
						bool zcardSortset(const char *key, int64_t &count);
						bool zcountSortset(const char *key, const double min, const double max, int64_t &count); //return score in [min, max] count of sortset members, if min > max return 0
						bool zincrbySortset(const char *key, const double by, const std::string &value, double &newScore);
						bool zrangeSortset(const char *key, const int64_t start, const int64_t stop, vsPairVecType &vsVec /* member<->score */, bool withscore = false);	//asc rank : [0, len -1] or [-1, -len], if true == withscore return member<->score else return member and score will not be returned
						bool zrangeByScoreSortset(const char *key, const double min, const double max, vsPairVecType &vsVec /* member<->score */, bool withscore = false);	//asc rank, score: [min, max] or [-1, -len], if true == withscore return member<->score else return member and score will not be returned
						bool zrankSortset(const char *key, const std::string &member, int64_t &rank);
						bool zremSortset(const char *key, const std::string &member, int64_t &remCount);
						bool zremRangeByRankSortset(const char *key, const int64_t start, const int64_t stop, int64_t &remCount);	//[0, len - 1] or [ -1, - len]
						bool zremRangeByScoreSortset(const char *key, const double min, const double max, int64_t &remCount);
						bool zrevrangeSortset(const char *key, const int64_t start, const int64_t stop, vsPairVecType &vsVec /* member<->score */, bool withscore = false);	//desc rank : [0, len -1] or [-1, -len], if true == withscore return member<->score else return member and score will not be returned
						bool zrevrangeByScoreSortset(const char *key, const double min, const double max, vsPairVecType &vsVec /* member<->score */, bool withscore = false);	//desc rank : [min, max], if true == withscore return member<->score else return member and score will not be returned
						bool zrevrankSortset(const char *key, const std::string &member, int64_t &rank);
						bool zscoreSortset(const char *key, const std::string &member, double &score);
						/* end sortset */

						/* geo */
						bool geoaddGeo(const char *key, const ddPairType &degree /* <longtitude, latitude> */, const std::string &member, int64_t &num);
						bool geoposGeo(const char *key, const std::string &member, ddPairType &degree /* <longtitude, latitude> */);
						bool geodistGeo(const char *key, const std::string &memberA, const std::string &memberB, double &distM /* unit ism */);
						bool georadiusGeo(const char *key, const ddPairType &degree, const double radius /* unit is m */, const int64_t count, sdPairVecType &resVec, int sortRule = 0 /* 0 no sort, 1 asc, 2 desc */);
						bool georadiusByMemberGeo(const char *key, const std::string &member, const double radius /* unit is m */, const int64_t count, sdPairVecType &resVec, int sortRule = 0/* 0 no sort, 1 asc, 2 desc */);
						bool geodelGeo(const char *key, const std::string &member, int64_t &remCount);

						/* end geo */

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
