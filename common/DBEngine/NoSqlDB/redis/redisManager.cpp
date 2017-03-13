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
 *       Compiler:  g++
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
#include "redisCmdFormat.h"
#include "mathUtil.h"

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

#define MACRO_REPLY_GET_INT64(reply, res)\
		if (reply->type != REDIS_REPLY_ERROR) \
		{	\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:	\
													{	\
															res = reply->integer;	\
													}	\
						break;	\
						case REDIS_REPLY_STRING:\
												{\
														res = ::atoll(reply->str);	\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_INT64_RET(reply, res, ret)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				ret = true;\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 res = reply->integer;	\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
												{\
														res = ::atoll(reply->str);	\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_DOUBLE_RET(reply, res, ret)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				ret = true;\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 res = reply->integer;	\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
												{\
														res = ::atof(reply->str);	\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_INT64PTR_RET(reply, res, ret)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				ret = true;\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 if (res)\
														 *res = reply->integer;	\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
												{\
														if (res)\
														*res = ::atoll(reply->str);	\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_STRING_RET(reply, value, ret)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				ret = true;\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 char tmp[64] = { 0 };	\
														 sprintf(tmp, "%lld", reply->integer);	\
														 value.assign(tmp, strlen(tmp));\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
						case REDIS_REPLY_STATUS:\
												{\
														value.assign(reply->str, reply->len);\
												}\
						break;\
				}\
		}

#define MACRO_REPLY_GET_STRING(reply, value)\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{	\
				switch(reply->type)\
				{	\
						case REDIS_REPLY_INTEGER:\
												 {\
														 char tmp[64] = { 0 };	\
														 sprintf(tmp, "%lld", reply->integer);	\
														 value.assign(tmp, strlen(tmp));\
												 }\
						break;\
						case REDIS_REPLY_STRING:\
						case REDIS_REPLY_STATUS:\
												{\
														value.assign(reply->str, reply->len);\
												}\
						break;\
				}\
		}

		/* key */
		bool RedisManager::delKey(const char *key, int64_t &delCnt)
		{
				redisReply *reply = _component.fireCmd(key, DEL_KEY_FORMAT, key);
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, delCnt, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::existsKey(const char *key, int64_t &exists)	//0 exists, 1 not exists, 2 error
		{
				redisReply *reply = _component.fireCmd(key, EXISTS_KEY_FORMAT, key);
				if (!reply) return false;
				bool ret = false;
				exists = 2;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						int64_t num = -1;
						MACRO_REPLY_GET_INT64(reply, num);
						if (num > 0)
						{
								exists = 0;
						}
						else if (0 == num)
						{
								exists = 1;
						}
						else
						{
								exists = 2;
						}
				}
				if (reply)
				{
						freeReplyObject(reply);
				}
				return ret;
		}

		bool RedisManager::pexpireKey(const char *key, uint64_t ms, bool &setSucc)
		{
				uint64_t nowms = 0;
				if (!getNowMs(nowms))
				{
						return false;
				}
				ms += nowms;
				return pexpireAtKey(key, ms, setSucc);
		}

		bool RedisManager::pexpireAtKey(const char *key, const uint64_t ms, bool &setSucc)
		{
				redisReply *reply = _component.fireCmd(key, PEXPIRE_AT_KEY_FORMAT, key, ms);
				if (!reply) return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						int64_t res = 0;
						MACRO_REPLY_GET_INT64(reply, res);
						if (res > 0)
						{
								setSucc = true;
						}

				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::pttlKey(const char *key, int64_t &pttl)
		{
				redisReply *reply = _component.fireCmd(key, PTTL_KEY_FORMAT, key);
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, pttl, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::typeKey(const char *key, std::string &typeStr,enum KEYTYPE &type)
		{
				redisReply *reply = _component.fireCmd(key, TYPE_KEY_FORMAT, key);
				if (!reply)
						return false;

				bool ret = false;
				MACRO_REPLY_GET_STRING_RET(reply, typeStr, ret);
				if (ret)
				{
						if (!strcasecmp(KEY_TYPE_STR_NONE, typeStr.c_str()))
								type = eNone;
						else if (!strcasecmp(KEY_TYPE_STR_STRING, typeStr.c_str()))
								type = eString;
						else if (!strcasecmp(KEY_TYPE_STR_LIST, typeStr.c_str()))
								type = eList;
						else if (!strcasecmp(KEY_TYPE_STR_SET, typeStr.c_str()))
								type = eSet;
						else if (!strcasecmp(KEY_TYPE_STR_ZSET, typeStr.c_str()))
								type = eZSet;
						else if (!strcasecmp(KEY_TYPE_STR_HASH, typeStr.c_str()))
								type = eHash;
						else
						{
								std::ostringstream oss;
								oss << "typeStr:" << typeStr;
								print_error(typeStr.c_str());
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		/* end key */

		/* string */

		bool RedisManager::getString(const char *key, std::string &value)
		{
				redisReply *reply = _component.fireCmd(key, GET_STRING_FORMAT, key);
				if (!reply)
						return false;
				bool ret = false;
				MACRO_REPLY_GET_STRING_RET(reply, value, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::incrByString(const char *key, const int64_t by, int64_t *rtn)
		{
				redisReply *reply = _component.fireCmd(key, INCRBY_STRING_FORMAT, key, by);
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						MACRO_REPLY_GET_INT64PTR_RET(reply, rtn, ret);
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::setString(const kvPairType &kvPair)
		{
				const std::string &key = kvPair.first;
				const std::string &value = kvPair.second;
				redisReply *reply = _component.fireCmd(key.c_str(), SET_STRING_FORMAT, key.c_str(), value.c_str(), value.length());
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
						ret = true;
				freeReplyObject(reply);
				return ret;
		}

		/* end string */

		/* hash */
		bool RedisManager::hdelHash(const char *key, const char *field)
		{
				redisReply *reply = _component.fireCmd(key, HDEL_HASH_FORMAT, key, field);
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
						ret = true;
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::hexistsHash(const char *key, const char *field, bool &exists)
		{
				redisReply *reply = _component.fireCmd(key, HEXISTS_HASH_FORMAT, key, field);
				if (!reply)
						return false;
				bool ret = false;
				int64_t num = 0;
				MACRO_REPLY_GET_INT64_RET(reply, num, ret);
				if (num > 0)
						exists = true;
				else
						exists = false;
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::hgetHash(const char *key, const char *field, std::string &value)
		{
				redisReply *reply = _component.fireCmd(key, HGET_HASH_FORMAT, key, field);
				if (!reply)
						return false;
				bool ret = false;
				MACRO_REPLY_GET_STRING_RET(reply, value, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::hgetallHash(const char *key, kvPairVecType &fieldValueVec /* field value */)
		{
				redisReply *reply = _component.fireCmd(key, HGETALL_HASH_FORMAT, key);
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						if (reply->type == REDIS_REPLY_ARRAY)
						{
								if (reply->elements > 0 && iseven(reply->elements))
								{
										fieldValueVec.reserve(reply->elements / 2);
										for (uint32_t i = 0; i < reply->elements; i += 2)
										{
												std::string field;
												std::string value;
												MACRO_REPLY_GET_STRING(reply->element[i], field);
												MACRO_REPLY_GET_STRING(reply->element[i + 1], value);
												fieldValueVec.emplace_back(field, value);
										}

								}
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::hkeysHash(const char *key, stringVecType &fieldVec)
		{
				redisReply *reply = _component.fireCmd(key, HKEYS_HASH_FORMAT, key);
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						if (reply->type == REDIS_REPLY_ARRAY)
						{
								if (reply->elements > 0)
								{
										fieldVec.reserve(reply->elements);
										for (uint32_t i = 0; i < reply->elements; ++i)
										{
												std::string field;
												bool data = false;
												MACRO_REPLY_GET_STRING_RET(reply->element[i], field, data);
												if (data)
														fieldVec.push_back(field);
										}
								}
						}

				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::hlenHash(const char *key, int64_t &len)
		{
				redisReply *reply = _component.fireCmd(key, HLEN_HASH_FORMAT, key);
				if (!reply)
						return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, len, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::hsetHash(const char *key, const char *field, const std::string &value)
		{
				redisReply *reply = _component.fireCmd(key, HSET_HASH_FORMAT, key, field, value.c_str(), value.length());
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
						ret = true;
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::hvalsHash(const char *key, stringVecType &valueVec)
		{
				redisReply *reply = _component.fireCmd(key, HVALS_HASH_FORMAT, key);
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						if (reply->type == REDIS_REPLY_ARRAY)
						{
								if (reply->elements > 0)
								{
										valueVec.reserve(reply->elements);
										for (uint32_t i = 0; i < reply->elements; ++i)
										{
												std::string value;
												MACRO_REPLY_GET_STRING(reply->element[i], value);
												valueVec.push_back(value);
										}
								}
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::hstrlenHash(const char *key, const char *field, int64_t &len)
		{
				redisReply *reply = _component.fireCmd(key, HSTRLEN_HASH_FORMAT, key, field);
				if (!reply)
						return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, len, ret);
				freeReplyObject(reply);
				return ret;
		}

		/* end hash */

		/* list */
		bool RedisManager::lindexList(const char *key, const int64_t idx, std::string &value)
		{
				redisReply *reply = _component.fireCmd(key, LINDEX_LIST_FORMAT, key, idx);
				if (!reply)
						return false;
				bool ret = false;
				MACRO_REPLY_GET_STRING_RET(reply, value, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::llenList(const char *key, int64_t &len)
		{
				redisReply *reply = _component.fireCmd(key, LLEN_LIST_FORMAT, key);
				if (!reply)
						return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, len, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::lpopList(const char *key, std::string &value, bool saveValue)
		{
				redisReply *reply = _component.fireCmd(key, LPOP_LIST_FORMAT, key);
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						switch(reply->type)
						{
								case REDIS_REPLY_INTEGER:
										{
												if (saveValue)
												{
														char tmp[64] = { 0 };
														sprintf(tmp, "%lld", reply->integer);
														value.assign(tmp, strlen(tmp));
												}
										}
										break;
								case REDIS_REPLY_STRING:
										{
												if (saveValue)
														value.assign(reply->str, reply->len);
										}
										break;
								default:
										{
										}
										break;
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::lpushList(const char *key, const std::string &value)
		{
				redisReply *reply = _component.fireCmd(key, LPUSH_LIST_FORMAT, key, value.c_str(), value.length());
				if (!reply)
						return false;
				bool ret = false;
				int64_t num = 0;
				MACRO_REPLY_GET_INT64(reply, num);
				if (num > 0) ret = true;
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::lrangeList(const char *key, const int64_t start, const int64_t stop, stringVecType &valueVec)
		{
				redisReply *reply = _component.fireCmd(key, LRANGE_LIST_FORMAT, key, start, stop);
				if (!reply) return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						if (reply->type == REDIS_REPLY_ARRAY)
						{
								if (reply->elements > 0)
								{
										valueVec.reserve(reply->elements);
										for (uint32_t i = 0; i < reply->elements; ++i)
										{
												std::string value;
												MACRO_REPLY_GET_STRING(reply->element[i], value);
												valueVec.push_back(value);
										}
								}
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::lremList(const char *key, const int64_t count, const std::string &value, int64_t &rtnCount)
		{
				redisReply *reply = _component.fireCmd(key, LREM_LIST_FORMAT, key, count, value.c_str(), value.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, rtnCount, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::lsetList(const char *key, const int64_t index, const std::string &value)
		{
				redisReply *reply = _component.fireCmd(key, LSET_LIST_FORMAT, key, index, value.c_str(), value.length());
				if (!reply) return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						switch(reply->type)
						{
								case REDIS_REPLY_STRING:
										{
												if (!strcasecmp("ok", reply->str)) ret = true;
										}
										break;
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::ltrimList(const char *key, const int64_t start, const int64_t stop)
		{
				redisReply *reply = _component.fireCmd(key, LTRIM_LIST_FORMAT, key, start, stop);
				if (!reply) return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						switch(reply->type)
						{
								case REDIS_REPLY_STRING:
								case REDIS_REPLY_STATUS:
										{
												if (!strcasecmp("ok", reply->str)) ret = true;
										}
										break;
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::rpopList(const char *key, std::string &value, bool saveValue)
		{
				redisReply *reply = _component.fireCmd(key, RPOP_LIST_FORMAT, key);
				if (!reply)
						return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						switch(reply->type)
						{
								case REDIS_REPLY_INTEGER:
										{
												if (saveValue)
												{
														char tmp[64] = { 0 };
														sprintf(tmp, "%lld", reply->integer);
														value.assign(tmp, strlen(tmp));
												}
										}
										break;
								case REDIS_REPLY_STRING:
										{
												if (saveValue)
														value.assign(reply->str, reply->len);
										}
										break;
								default:
										{
										}
										break;
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::rpushList(const char *key, const std::string &value)
		{
				redisReply *reply = _component.fireCmd(key, RPUSH_LIST_FORMAT, key, value.c_str(), value.length());
				if (!reply)
						return false;
				bool ret = false;
				int64_t num = 0;
				MACRO_REPLY_GET_INT64(reply, num);
				if (num > 0) ret = true;
				freeReplyObject(reply);
				return ret;
		}
		/* end list */

		/* set */
		bool RedisManager::saddSet(const char *key, const std::string &value, int64_t &num)
		{
				redisReply *reply = _component.fireCmd(key, SADD_SET_FORMAT, key, value.c_str(), value.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, num, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::scardSet(const char *key, int64_t &count)
		{
				redisReply *reply = _component.fireCmd(key, SCARD_SET_FORMAT, key);
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, count, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::sismemberSet(const char *key, const std::string &value, bool &ismember)
		{
				redisReply *reply = _component.fireCmd(key, SISMEMBER_SET_FORMAT, key, value.c_str(), value.length());
				if (!reply) return false;
				bool ret = false;
				int64_t count = 0;
				MACRO_REPLY_GET_INT64_RET(reply, count, ret);
				if (count > 0) ismember = true;
				else ismember = false;
				freeReplyObject(reply);
				return ret;

		}
		bool RedisManager::smembersSet(const char *key, stringVecType &valueVec)
		{
				redisReply *reply = _component.fireCmd(key, SMEMBERS_SET_FORMAT, key);
				if (!reply) return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						if (reply->type == REDIS_REPLY_ARRAY)
						{
								if (reply->elements > 0)
								{
										valueVec.reserve(reply->elements);
										for (uint32_t i = 0; i < reply->elements; ++i)
										{
												std::string value;
												MACRO_REPLY_GET_STRING(reply->element[i], value);
												valueVec.push_back(value);
										}
								}
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::spopSet(const char *key, std::string &value)
		{
				redisReply *reply = _component.fireCmd(key, SPOP_SET_FORMAT, key);
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_STRING_RET(reply, value, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::srandmemberSet(const char *key, stringVecType &valueVec, const int64_t count)
		{
				redisReply *reply = _component.fireCmd(key, SRANDMEMBER_SET_FORMAT, key, count);
				if (!reply) return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						if (reply->type == REDIS_REPLY_ARRAY)
						{
								if (reply->elements > 0)
								{
										valueVec.reserve(reply->elements);
										for (uint32_t i = 0; i < reply->elements; ++i)
										{
												std::string value;
												MACRO_REPLY_GET_STRING(reply->element[i], value);
												valueVec.push_back(value);
										}
								}
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::sremSet(const char *key, const std::string &value, int64_t &remCnt)
		{
				redisReply *reply = _component.fireCmd(key, SREM_SET_FORMAT, key, value.c_str(), value.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, remCnt, ret);
				freeReplyObject(reply);
				return ret;

		}
		/* end set */

		/* sortset */
		bool RedisManager::zaddSortset(const char *key, const std::string &value, const double score, int64_t &count)
		{
				redisReply *reply = _component.fireCmd(key, ZADD_SORTSET_FORMAT, key, score, value.c_str(), value.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, count, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zcardSortset(const char *key, int64_t &count)
		{
				redisReply *reply = _component.fireCmd(key, ZCARD_SORTSET_FORMAT, key);
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, count, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zcountSortset(const char *key, const double min, const double max, int64_t &count)
		{
				redisReply *reply = _component.fireCmd(key, ZCOUNT_SORTSET_FORMAT, key, min, max);
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, count, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zincrbySortset(const char *key, const double by, const std::string &value, double &newScore)
		{
				redisReply *reply = _component.fireCmd(key, ZINCRBY_SORTSET_FORMAT, key, by, value.c_str(), value.length());
				if (!reply) return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						switch(reply->type)
						{
								case REDIS_REPLY_STRING:
										{
												newScore = ::atof(reply->str);
										}
										break;
								case REDIS_REPLY_INTEGER:
										{
												newScore = reply->integer;
										}
										break;
						}
				}
				freeReplyObject(reply);
				return ret;
		}

#define MACRO_SORT_SET_VS_RES(reply, vsVec)	\
		bool ret = false;\
		do\
		{	\
				if (reply->type != REDIS_REPLY_ERROR)			\
				{	\
						ret = true;\
						if (reply->type == REDIS_REPLY_ARRAY)\
						{	\
								if (reply->elements > 0)\
								{	\
										if (withscore)\
										{	\
												if (!iseven(reply->elements))\
												{	\
														goto end;	\
												}	\
												vsVec.reserve(reply->elements / 2);	\
										}	\
										else \
										vsVec.reserve(reply->elements);\
										for (uint32_t i = 0; i < reply->elements; )\
										{	\
												std::string value;\
												double score = 0.0;\
												switch(reply->element[i]->type)\
												{	\
														case REDIS_REPLY_INTEGER:\
																				 {\
																						 char tmp[64] = { 0 };\
																						 sprintf(tmp, "%lld", reply->element[i]->integer);\
																						 value.assign(tmp, strlen(tmp));\
																				 }\
														break;\
														case REDIS_REPLY_STRING:\
																				{\
																						value.assign(reply->element[i]->str, reply->element[i]->len);\
																				}\
														break;\
												}\
												if (withscore)\
												{\
														switch(reply->element[i + 1]->type)\
														{\
																case REDIS_REPLY_INTEGER:\
																						 {\
																							 	 score = reply->element[i + 1]->integer;\
																						 }\
																break;\
																case REDIS_REPLY_STRING:\
																						{\
																								score = ::atof(reply->element[i + 1]->str);\
																						}\
																break;\
														}\
												}\
												vsVec.emplace_back(std::make_pair(value, score));\
												if (withscore) i += 2;\
												else ++i;\
										}\
								}\
						}\
				}\
		}while(0)

		bool RedisManager::zrangeSortset(const char *key, const int64_t start, const int64_t stop, vsPairVecType &vsVec /* member<->score */, bool withscore)
		{
				const char *format = NULL;
				if (withscore) format = ZRANGE_WITHSCORES_SORTSET_FORMAT;
				else format = ZRANGE_SORTSET_FORMAT;
				redisReply *reply = _component.fireCmd(key, format, key, start, stop);
				if (!reply) return false;
				MACRO_SORT_SET_VS_RES(reply, vsVec);
end:
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zrangeByScoreSortset(const char *key, const double min, const double max, vsPairVecType &vsVec /* member<->score */, bool withscore)
		{
				const char *format = NULL;
				if (withscore) format = ZRANGEBYSCORE_WITHSCORES_SORTSET_FORMAT;
				else format = ZRANGEBYSCORE_SORTSET_FORMAT;
				redisReply *reply = _component.fireCmd(key, format, key, min, max);
				if (!reply) return false;
				MACRO_SORT_SET_VS_RES(reply, vsVec);
end:
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zrankSortset(const char *key, const std::string &member, int64_t &rank)
		{
				redisReply *reply = _component.fireCmd(key, ZRANK_SORTSET_FORMAT, key, member.c_str(), member.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, rank, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zremSortset(const char *key, const std::string &member, int64_t &remCount)
		{
				redisReply *reply = _component.fireCmd(key, ZREM_SORTSET_FORMAT, key, member.c_str(), member.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, remCount, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zremRangeByRankSortset(const char *key, const int64_t start, const int64_t stop, int64_t &remCount)
		{
				redisReply *reply = _component.fireCmd(key, ZREMRANGEBYRANK_SORTSET_FORMAT, key, start, stop);
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, remCount, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zremRangeByScoreSortset(const char *key, const double min, const double max, int64_t &remCount)
		{
				redisReply *reply = _component.fireCmd(key, ZREMRANGEBYSCORE_SORTSET_FORMAT, key, min, max);
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, remCount, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zrevrangeSortset(const char *key, const int64_t start, const int64_t stop, vsPairVecType &vsVec /* member<->score */, bool withscore)
		{
				const char *format = NULL;
				if (withscore) format = ZREVRANGE_WITHSCORES_SORTSET_FORMAT;
				else format = ZREVRANGE_SORTSET_FORMAT;
				redisReply *reply = _component.fireCmd(key, format, key, start, stop);
				if (!reply) return false;
				MACRO_SORT_SET_VS_RES(reply, vsVec);
end:
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zrevrangeByScoreSortset(const char *key, const double min, const double max, vsPairVecType &vsVec /* member<->score */, bool withscore)
		{
				const char *format = NULL;
				if (withscore) format = ZREVRANGEBYSCORE_WITHSCORES_SORTSET_FORMAT;
				else format = ZREVRANGEBYSCORE_SORTSET_FORMAT;
				redisReply *reply = _component.fireCmd(key, format, key, max, min);
				if (!reply) return false;
				MACRO_SORT_SET_VS_RES(reply, vsVec);
end:
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zrevrankSortset(const char *key, const std::string &member, int64_t &rank)
		{
				redisReply *reply = _component.fireCmd(key, ZREVRANK_SORTSET_FORMAT, key, member.c_str(), member.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, rank, ret);
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::zscoreSortset(const char *key, const std::string &member, double &score)
		{
				redisReply *reply = _component.fireCmd(key, ZSCORE_SORTSET_FORMAT, key, member.c_str(), member.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_DOUBLE_RET(reply, score, ret);
				freeReplyObject(reply);
				return ret;
		}
		/* end sortset */

		/* geo */
		bool RedisManager::geoaddGeo(const char *key, const ddPairType &degree /* <longtitude, latitude> */, const std::string &member, int64_t &num)
		{
				redisReply *reply = _component.fireCmd(key, GEOADD_GEO_FORMAT, key, degree.first, degree.second, member.c_str(), member.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_INT64_RET(reply, num, ret);
				(void)num;
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::geoposGeo(const char *key, const std::string &member, ddPairType &degree /* <longtitude, latitude> */)
		{
				redisReply *reply = _component.fireCmd(key, GEOPOS_GEO_FORMAT, key, member.c_str(), member.length());
				if (!reply) return false;
				bool ret = false;
				if (reply->type != REDIS_REPLY_ERROR)
				{
						ret = true;
						if (reply->type == REDIS_REPLY_ARRAY)
						{
								if (reply->elements > 0)
								{
										if (reply->element[0]->type == REDIS_REPLY_ARRAY && 2 == reply->element[0]->elements)
										{
												MACRO_REPLY_GET_DOUBLE_RET(reply->element[0]->element[0], degree.first, ret);
												MACRO_REPLY_GET_DOUBLE_RET(reply->element[0]->element[1], degree.second, ret);
										}
								}
						}
				}
				freeReplyObject(reply);
				return ret;
		}

		bool RedisManager::geodistGeo(const char *key, const std::string &memberA, const std::string &memberB, double &distM /* unit ism */)
		{
				redisReply *reply = _component.fireCmd(key, GEODIST_GEO_FORMAT, key, memberA.c_str(), memberA.length(), memberB.c_str(), memberB.length());
				if (!reply) return false;
				bool ret = false;
				MACRO_REPLY_GET_DOUBLE_RET(reply, distM, ret);
				freeReplyObject(reply);
				return ret;
		}

#define MACRO_GEO_RADIUS_GET(prefix, resVec, ismember)	\
		char cmd[256] = { 0 };	\
		strcpy(cmd, prefix);	\
		strcat(cmd, GEORADIUS_GEO_FORMAT_PART_WITHDIST);	\
		if (1 == sortRule)\
				strcat(cmd, GEORADIUS_GEO_FORMAT_PART_ASC);	\
		else if (2 == sortRule)\
				strcat(cmd, GEORADIUS_GEO_FORMAT_PART_DESC);	\
		strcat(cmd, GEORADIUS_GEO_FORMAT_PART_COUNT);	\
		redisReply *reply = NULL;\
		if (ismember) 	\
				reply = _component.fireCmd(key, cmd, key, member.c_str(), member.length(), radius, count);	\
		else	\
				reply = _component.fireCmd(key, cmd, key, myDegree.first, myDegree.second, radius, count);	\
		if (!reply) return false;	\
		bool ret = false;\
		if (reply->type != REDIS_REPLY_ERROR)	\
		{\
				ret = true;\
				if (reply->type == REDIS_REPLY_ARRAY)\
				{	\
						if (reply->elements > 0)\
						{\
								resVec.reserve(reply->elements);\
								for (uint32_t i = 0; i < reply->elements; ++i)\
								{\
										redisReply *element = reply->element[i]; \
										if (2 == element->elements)	\
										{	\
												std::string value;\
												double distM = 0.0;\
												bool data1 = false;\
												bool data2 = false;\
												MACRO_REPLY_GET_STRING_RET(element->element[0], value, data1);	\
												MACRO_REPLY_GET_DOUBLE_RET(element->element[1], distM, data2);	\
												if (data1 && data2)\
														resVec.emplace_back(value, distM);\
										}	\
								}\
						}\
				}	\
		}\
		freeReplyObject(reply);\
		return ret;

		bool RedisManager::georadiusGeo(const char *key, const ddPairType &myDegree, const double radius /* unit is m */, const int64_t count, sdPairVecType &resVec, int sortRule /* 0 no sort, 1 asc, 2 desc */)
		{
				std::string member;		//just for compile
				MACRO_GEO_RADIUS_GET(GEORADIUS_GEO_FORMAT, resVec, false);
		}

		bool RedisManager::georadiusByMemberGeo(const char *key, const std::string &member, const double radius /* unit is m */, const int64_t count, sdPairVecType &resVec, int sortRule /* 0 no sort, 1 asc, 2 desc */)
		{
				ddPairType myDegree;		//just for compile
				MACRO_GEO_RADIUS_GET(GEORADIUSBYMEMBER_GEO_FORMAT, resVec, true);
		}

		bool RedisManager::geodelGeo(const char *key, const std::string &member, int64_t &remCount)
		{
				return zremSortset(key, member, remCount);
		}

		/* end geo */


}
