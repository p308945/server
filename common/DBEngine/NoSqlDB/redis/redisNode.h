/*
 * =====================================================================================
 *
 *       Filename:  redisNode.h
 *
 *    Description:  node
 *
 *        Version:  1.0
 *        Created:  03/09/2017 04:42:39 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __REDISNODE_H__
#define __REDISNODE_H__

#include <cstdint>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <functional>
#include "hiredis.h"
#include "util.h"
#include "redisConfig.h"

namespace goddard
{
		enum REDISNODEROLE
		{
				REDIS_ROLE_INVALID,
				REDIS_ROLE_MASTER,
				REDIS_ROLE_SLAVE,
		};

		enum REDISNODESTATE
		{
				REDIS_OFFLINE,
				REDIS_ONLINE,
		};

		class RedisCommon;

		class RedisCommonNode
		{
				public:
						RedisCommonNode()
						{
								_owner = NULL;
								_ctx = NULL;
								_port = 0;
								_id = (uint32_t)-1;
								eRole = REDIS_ROLE_INVALID;
								eState = REDIS_OFFLINE;
						}

						virtual ~RedisCommonNode()
						{
						}

						virtual void reset(const std::string &ip = "", const uint32_t port = 0, const std::string &passwd = "")
						{
								if (_ctx)
								{
										redisFree(_ctx);
								}
								_ctx = NULL;
								_ip = ip;
								_port = port;
								_passwd = passwd;
								_id = (uint32_t)-1;
								eRole = REDIS_ROLE_INVALID;
								eState = REDIS_OFFLINE;
								removeAllSlaveNode();
						}

						void setOwner(RedisCommon *owner)
						{
								_owner = owner;
						}

						virtual bool online(const struct timeval &timeout, const REDISNODEROLE role);

						bool isOnline()
						{
								return eState == REDIS_ONLINE;
						}

						const std::string& getIP()
						{
								return _ip;
						}

						uint32_t getPort()
						{
								return _port;
						}

						redisContext *getCtx()
						{
								return _ctx;
						}
						RedisCommonNode* getSlaveNode(const std::string &ip, const uint32_t &port);
						virtual bool addSlaveNode(const std::string &ip, const uint32_t port, const struct timeval timeout);

				private:
						void removeAllSlaveNode();
						virtual RedisCommonNode* newInstance()
						{
								RedisCommonNode *node = new RedisCommonNode();
								if (NULL == node)
								{
										std::ostringstream oss;
										oss << "newInstance failed new return NULL";
										print_error(oss.str());
								}
								return node;
						}

				protected:
						RedisCommon *_owner;
				private:
						redisContext *_ctx;
						std::string _ip;
						uint32_t _port;
						std::string _passwd;
						uint32_t _id;
						enum REDISNODEROLE eRole;
						enum REDISNODESTATE eState;
						using slaveList = std::list<RedisCommonNode *>;
						using slaveListIter = slaveList::iterator;
						slaveList _slaveList;

		};

		const uint32_t CLUSTER_NODE_SIZE = 256;
		const uint32_t SLOT_MAX_SIZE = SLOT_MAX_MASK + 1;

		enum REDISCLUSTERSTATE
		{
				REDIS_CLUSTER_ON,
				REDIS_CLUSTER_FAIL,
				REDIS_CLUSTER_DOWN,
		};

		class RedisClusterNode : public RedisCommonNode
		{
				public:
						RedisClusterNode()
						{
						}

						~RedisClusterNode()
						{
								reset();
						}

						virtual RedisCommonNode* newInstance()
						{
								RedisCommonNode* node = new RedisClusterNode();
								if (NULL == node)
								{
										std::ostringstream oss;
										oss << "newInstance failed failed new return NULL";
										print_error(oss.str());
										return NULL;
								}
								return node;
						}

						void clearSlot(const uint32_t slot)
						{
								_slotSet.erase(slot);
						}

						virtual void reset(const std::string &ip = "", const uint32_t port = 0, const std::string &passwd = "");

				private:
						using SlotSet = std::set<uint32_t>;
						using SlotSetIter = SlotSet::iterator;
						SlotSet _slotSet;

		};

		class RedisSlotNode
		{
				public:
						RedisSlotNode()
						{
								_node = NULL;
								_slot = (uint32_t)-1;
						}

						void reset(RedisClusterNode *node = NULL, uint32_t slot = (uint32_t)-1)
						{
								_node = node;
								_slot = slot;
						}

						void disconnNodeSlot(RedisClusterNode *node, uint32_t slot)
						{
								if (_node == node && _slot == slot)
								{
										reset();
								}
						}

						void connNodeSlot(RedisClusterNode *node, uint32_t slot)
						{
								if (_node != NULL)
								{
										_node->clearSlot(slot);
								}
								reset(node, slot);
						}

						RedisClusterNode *getNode()
						{
								return _node;
						}

						uint32_t getSlot()
						{
								return _slot;
						}

				public:
						RedisClusterNode *_node;
						uint32_t _slot;

		};

}

#endif
