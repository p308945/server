/*
 * =====================================================================================
 *
 *       Filename:  redisCluster.h
 *
 *    Description:  cluster
 *
 *        Version:  1.0
 *        Created:  03/09/2017 07:13:29 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __REDISCLUSTER_H__
#define __REDISCLUSTER_H__

#include "redisCommon.h"
#include "redisCommProcess.h"

namespace goddard
{
		class RedisCluster;

		class RedisClusterItem
		{
				public:
						RedisClusterItem()
						{
								eState = REDIS_CLUSTER_DOWN;
						}

						void setOwner(RedisCluster *owner);

						~RedisClusterItem()
						{
								reset();
						}

						void disconnNodeSlot(RedisClusterNode *node, uint32_t slot)
						{
								if (!IsValidSlotIdx(slot))
								{
										return;
								}
								_slotArray[slot].disconnNodeSlot(node, slot);
						}

						void connNodeSlot(RedisClusterNode *node, uint32_t slot)
						{
								if (!IsValidSlotIdx(slot))
								{
										return;
								}
								_slotArray[slot].connNodeSlot(node, slot);
						}

						bool IsValidSlotIdx(uint32_t slot)
						{
								return slot >= 0 && slot < SLOT_MAX_SIZE;
						}

						bool IsValidNodeIdx(uint32_t idx)
						{
								return idx >= 0 && idx < CLUSTER_NODE_SIZE;
						}

						bool nodeReset(uint32_t idx, const std::string &ip, const uint32_t port)
						{
								if (!IsValidNodeIdx(idx))
								{
										return false;
								}
								_clusterNodeArray[idx].reset(ip, port);
								return true;
						}

						bool nodeOnline(uint32_t idx, const timeval &timeout, REDISNODEROLE role)
						{
								if (!IsValidNodeIdx(idx))
								{
										return false;
								}
								return _clusterNodeArray[idx].online(timeout, role);
						}

						RedisClusterNode* getNodeByIdx(uint32_t idx)
						{
								if (!IsValidNodeIdx(idx))
								{
										return NULL;
								}
								return &_clusterNodeArray[idx];
						}

						RedisClusterNode* getClusterNodeBySlotIdx(uint32_t slot);

						void reset()
						{
								eState = REDIS_CLUSTER_DOWN;
								std::for_each(_clusterNodeArray, _clusterNodeArray + CLUSTER_NODE_SIZE, [this](RedisClusterNode &node)
												{
												node.reset();
												});

								std::for_each(_slotArray, _slotArray + SLOT_MAX_SIZE, [this](RedisSlotNode &node)
												{
												node.reset();
												});
						}

						void online()
						{
								eState = REDIS_CLUSTER_ON;
						}

						RedisClusterNode* getMasterNode(const std::string &ip, const uint32_t &port);
						RedisClusterNode* getFreeNode();

				private:

						RedisClusterNode _clusterNodeArray[CLUSTER_NODE_SIZE];
						RedisSlotNode _slotArray[SLOT_MAX_SIZE];
						enum REDISCLUSTERSTATE eState;
						RedisCluster *_owner;
		};

		class RedisCluster : public RedisCommon
		{
				public:
						RedisCluster(const RedisConfig *config) : RedisCommon(config)
				{
						mCluster.setOwner(this);
						_redirectCount = 0;
				}
						virtual ~RedisCluster()
						{
						}

						virtual bool start();
						virtual RedisCommonNode *getRedisNode(const char *key);
						void setSlotNode(RedisClusterNode *node, uint32_t slotIdx)
						{
								mCluster.connNodeSlot(node, slotIdx);
						}

						void disconnNodeSlot(RedisClusterNode *node, uint32_t slot);
						bool processMovedAsk(cmd_st *cmd, bool isAsk);
				protected:
						void reset()
						{
								mCluster.reset();
						}

				private:
						bool clusterRefresh();
						bool clusterRefreshFromReply(const redisReply* reply);
						redisReply *commandClusterSlots(redisContext *ctx);
			
				private:
						RedisClusterItem mCluster;
						uint32_t _redirectCount;
		};
}

#endif
