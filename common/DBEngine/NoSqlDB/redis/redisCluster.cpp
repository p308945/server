/*
 * =====================================================================================
 *
 *       Filename:  redisCluster.cpp
 *
 *    Description:  cluster
 *
 *        Version:  1.0
 *        Created:  03/09/2017 07:14:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "redisCluster.h"

namespace goddard
{
		static const char *REDIS_COMMAND_CLUSTER_SLOTS = "CLUSTER SLOTS";
		static const char *REDIS_COMMAND_ASKING = "ASKING";
		void RedisClusterItem::setOwner(RedisCluster *owner)
		{
				std::for_each(_clusterNodeArray, _clusterNodeArray + CLUSTER_NODE_SIZE,[this, owner](RedisClusterNode &node)
								{
								node.setOwner(owner);
								});
				_owner = owner;
		}

		RedisClusterNode* RedisClusterItem::getClusterNodeBySlotIdx(uint32_t slot)
		{
				if (!IsValidSlotIdx(slot))
				{
						return NULL;
				}
				RedisSlotNode *slotNode = &(_slotArray[slot]);
				RedisClusterNode *node = slotNode->getNode();
				if (NULL == node)
				{
						return NULL;
				}
				if (!node->isOnline())
				{
						return NULL;
				}
				return node;
		}

		RedisClusterNode* RedisClusterItem::getMasterNode(const std::string &ip, const uint32_t &port)
		{
				auto iter = std::find_if(_clusterNodeArray, _clusterNodeArray + CLUSTER_NODE_SIZE, [this, &ip, &port](RedisClusterNode &node)
								{
								return ip == node.getIP() && port == node.getPort();
								});
				if (iter == _clusterNodeArray + CLUSTER_NODE_SIZE)
				{
						return NULL;
				}
				return iter;
		}

		RedisClusterNode *RedisClusterItem::getFreeNode()
		{
				auto iter =  std::find_if(_clusterNodeArray, _clusterNodeArray + CLUSTER_NODE_SIZE, [this](RedisClusterNode &node)
								{
								return !node.isOnline();
								});
				if (iter == _clusterNodeArray + CLUSTER_NODE_SIZE)
				{
						return NULL;
				}
				return iter;
		}

		bool RedisCluster::start()
		{
				if (!clusterRefresh())
				{
						return false;
				}
				std::cout << "RedisCluster::start success" << std::endl;
				return true;
		}

		bool RedisCluster::clusterRefresh()
		{
				RedisClusterNode node;
				node.reset(getIP(), getPort(), getPasswd());
				bool res = node.online(getTimeout(), REDIS_ROLE_MASTER);
				if (!res)
				{
						std::ostringstream oss;
						oss << "clusterRefresh node online failed";
						print_error(oss.str());
						return false;
				}

				redisReply *reply = commandClusterSlots(node.getCtx());
				if (NULL == reply)
				{
						std::ostringstream oss;
						oss << "clusterRefresh reply is NULL";
						print_error(oss.str());
						return false;
				}
				if (!clusterRefreshFromReply(reply))
				{
						std::ostringstream oss;
						oss << "clusterRefreshFromReply return false";
						print_error(oss.str());
						return false;
				}
				freeReplyObject(reply);
				return true;
		}

		bool RedisCluster::clusterRefreshFromReply(const redisReply *reply)
		{
				if (NULL == reply)
				{
						return false;
				}
				mCluster.reset();
				std::string ip;
				uint32_t port;
				uint32_t i, j, k;
				if (!(reply->type == REDIS_REPLY_ARRAY && reply->elements > 0))
				{
						std::ostringstream oss;
						oss << "Command(cluster slots) reply type:" << reply->type << ". elements: " << reply->elements;
						print_error(oss.str());
						goto err;
				}
				for (i = 0; i < reply->elements; ++i)
				{
						if (!(reply->element[i]->elements >= 3 &&
												reply->element[i]->element[0]->type == REDIS_REPLY_INTEGER &&
												reply->element[i]->element[1]->type == REDIS_REPLY_INTEGER &&
												reply->element[i]->element[2]->type == REDIS_REPLY_ARRAY &&
												reply->element[i]->element[2]->elements >= 2 &&
												reply->element[i]->element[2]->element[0]->type == REDIS_REPLY_STRING &&
												reply->element[i]->element[2]->element[1]->type == REDIS_REPLY_INTEGER
							 ))
						{
								print_error("Invalid Type");
								goto err;
						}

						uint32_t start = (uint32_t)-1;
						uint32_t stop = (uint32_t)-1;
						RedisClusterNode *masterNode = NULL;
						for (j = 0; j < reply->element[i]->elements; ++j)
						{
								if (0 == j)
								{
										start = reply->element[i]->element[j]->integer;
								}
								else if (1 == j)
								{
										stop = reply->element[i]->element[j]->integer;
								}
								else
								{
										ip.assign(reply->element[i]->element[j]->element[0]->str);
										port = reply->element[i]->element[j]->element[1]->integer;

										//MASTER NODE
										if (2 == j)
										{
												masterNode = mCluster.getMasterNode(ip, port);
												if (NULL == masterNode)
												{
														mCluster.nodeReset(i, ip, port);
														bool b = mCluster.nodeOnline(i, getTimeout(), REDIS_ROLE_MASTER);
														if (!b)
														{
																std::ostringstream oss;
																oss << "cluster node online failed ip: " << ip << ", port: " << port;
																print_error(oss.str());
																goto err;
														}
														masterNode = mCluster.getNodeByIdx(i);
														incrMasterCount();
														std::cout << "start: " << start << ", stop: " << stop << std::endl;
														for (k = start; k < stop; ++k)
														{
																setSlotNode(masterNode, k);
														}
														std::cout << "connect master server ok ip: " << ip << ", port: " << port << ", master count: " << getMasterCount() << std::endl;
												}
												else
												{
														std::cout << "master server already connected ip: " << ip << ", port: " << port << ", master count: " << getMasterCount() << std::endl;
												}

										}
										else	//SLAVE NODE
										{
												if (NULL != masterNode)
												{
														RedisClusterNode *node = NULL;
														node = dynamic_cast<RedisClusterNode *>(masterNode->getSlaveNode(ip, port));
														if (NULL == node)
														{
																masterNode->addSlaveNode(ip, port, getTimeout());
																incrSlaveCount();
																std::cout << "connect slave server ok ip: " << ip << ", port: " << port << ", slave count: " << getSlaveCount() << std::endl;
														}
														else
														{
																std::cout << "slave already connected ip: " << ip << ", port: " << port << ", slave count: " << getSlaveCount() << std::endl;
														}

												}
												else
												{
														std::ostringstream oss;
														oss << "master node is NULL slave ip: " << ip << ", port: " << port;
														print_error(oss.str());
														goto err;
												}
										}
								}
						}

				}
				mCluster.online();
				return true;
err:
				mCluster.reset();
				return false;
		}

		redisReply * RedisCluster::commandClusterSlots(redisContext *ctx)
		{
				if (NULL == ctx)
				{
						std::ostringstream oss;
						oss << "commandClusterSlots ctx is NULL";
						print_error(oss.str());
						return NULL;
				}
				redisReply *reply = (redisReply *)redisCommand(ctx, REDIS_COMMAND_CLUSTER_SLOTS);
				if (NULL == reply)
				{
						std::ostringstream oss;
						oss << "commandClusterSlots reply is NULL";
						print_error(oss.str());
						return NULL;
				}
				return reply;
		}

		RedisCommonNode *RedisCluster::getRedisNode(const char *key)
		{
				uint32_t slotIdx = keyHashSlot(key, strlen(key));
				RedisClusterNode *node = mCluster.getClusterNodeBySlotIdx(slotIdx);
				if (NULL == node)
				{
						std::ostringstream oss;
						oss << "getClusterNodeBySlotIdx return NULL, key:" << key << ", slotIdx:" << slotIdx;
						print_error(oss.str());
						return NULL;
				}
				return node;
		}

		void RedisCluster::disconnNodeSlot(RedisClusterNode *node, uint32_t slot)
		{
				mCluster.disconnNodeSlot(node, slot);
		}

		bool RedisCluster::processMovedAsk(cmd_st *cmd, bool isAsk)
		{
				redisReply *reply = cmd->ireply;
				//format:
				//MOVED 16383 127.0.0.1:6379
				//of
				//ASK 16383 127.0.0.1:6379
				sds *part = NULL;
				int count = 0;
				bool ret = false;
				part = sdssplitlen(reply->str, strlen(reply->str), " ", 1, &count);
				if (3 == count)
				{
						char *p = strchr(part[2], ':');
						if (NULL != p)
						{
								char ip[32] = { 0 };
								strncpy(ip, part[2], p - part[2]);
								uint32_t port = ::atoi(p + 1);
								RedisClusterNode *node = mCluster.getMasterNode(ip, port);
								if (NULL == node)
								{
										node = mCluster.getFreeNode();
										if (NULL == node)
										{
												std::ostringstream oss;
												oss << "processMovedAsk getFreeNode failed:" << ", str:" << reply->str;
												print_error(oss.str());
												ret = false;
												goto end;
										}
								}
								if (!node->isOnline())
								{
										node->reset(ip, port, getPasswd());
										if (!node->online(getTimeout(), REDIS_ROLE_MASTER))
										{
												std::ostringstream oss;
												oss << "processMovedAsk online failed:" << ", str:" << reply->str;
												print_error(oss.str());
												ret = false;
												goto end;
										}
								}
								if (isAsk)
								{
										redisReply *askReply = (redisReply *)redisCommand(node->getCtx(), REDIS_COMMAND_ASKING);
										if (NULL == askReply || REDIS_REPLY_ERROR == askReply->type)
										{
												std::ostringstream oss;
												oss << "processMovedAsk ask failed:" << ", str:" << reply->str;
												if (NULL != askReply)
												{
														oss << "askReply errstr:" << askReply->str;
														freeReplyObject(askReply);
														askReply = NULL;
												}
												print_error(oss.str());
												ret = false;
												goto end;
										}
										else
										{
												freeReplyObject(askReply);
												askReply = NULL;
										}
								}
								cmd->oreply = (redisReply *)redisvCommand(cmd->node->getCtx(), cmd->format, cmd->ap);
								ret = true;
								goto end;
						}
						else
						{
								std::ostringstream oss;
								oss << "processMovedAsk reply str part 2 is not splt by[:]" << part[2] << ", str:" << reply->str;
								print_error(oss.str());
								ret = false;
								goto end;
						}
				}
				else
				{
						std::ostringstream oss;
						oss << "processMovedAsk reply str part is not 3:" << count << ":" << reply->str;
						print_error(oss.str());
						ret = false;
						goto end;
				}
				return true;
end:
				sdsfreesplitres(part, count);
				return ret;
		}
}

