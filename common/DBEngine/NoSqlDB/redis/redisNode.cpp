/*
 * =====================================================================================
 *
 *       Filename:  redisNode.cpp
 *
 *    Description:  node
 *
 *        Version:  1.0
 *        Created:  03/09/2017 05:11:27 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "redisNode.h"
#include "redisCluster.h"

namespace goddard
{
		bool RedisCommonNode::addSlaveNode(const std::string &ip, const uint32_t port, const struct timeval timeout)
		{
				RedisCommonNode *node = newInstance();
				if (!node)
				{
						std::ostringstream oss;
						oss << "addSlaveNode failed newInstance return NULL";
						print_error(oss.str());
						return false;
				}
				node->reset(ip, port);
				bool b = node->online(timeout, REDIS_ROLE_SLAVE);
				if (!b)
				{
						std::ostringstream oss;
						oss << "addSlaveNode failed online failed";
						print_error(oss.str());
						delete node;
						return false;
				}
				_slaveList.push_back(node);
				return true;
		}

		RedisCommonNode * RedisCommonNode::getSlaveNode(const std::string &ip, const uint32_t &port)
		{
				slaveListIter iter = std::find_if(_slaveList.begin(), _slaveList.end(), [this, &ip, &port](RedisCommonNode *node)
								{
										return node && (node->_ip == ip && node->_port == port);
								});
				if (_slaveList.end() != iter)
				{
						return *iter;
				}
				return NULL;
		}

		bool RedisCommonNode::online(const struct timeval &timeout, const REDISNODEROLE role)
		{
				if (NULL != _ctx)
				{
						redisFree(_ctx);
				}

				_ctx = redisConnectWithTimeout(_ip.c_str(), _port, timeout);
				if (NULL == _ctx || _ctx->err)
				{
						std::ostringstream oss;
						oss << "redis node " << _ip << ", " << _port << ", online failed:";
						if (NULL != _ctx)
						{
								if (_ctx->err)
								{
										oss << _ctx->errstr;
								}
								redisFree(_ctx);
								_ctx = NULL;
						}
						else
						{
								oss << "_cxt is NULL";
						}
						print_error(oss.str());
						eState = REDIS_OFFLINE;
						return false;
				}
				if (!_passwd.empty())
				{
						std::string authCmd = "AUTH " + _passwd;
						redisReply *reply = (redisReply *)redisCommand(_ctx, authCmd.c_str());
						if (NULL == reply || reply->type == REDIS_REPLY_ERROR)
						{
								std::ostringstream oss;
								oss << "redis node : "<< _ip << ", " << _port << ", online failed passwd auth failed:" << _passwd;
								print_error(oss.str());
								if (reply)
								{
										freeReplyObject(reply);
										reply = NULL;
								}
						}
						freeReplyObject(reply);
						reply = NULL;
				}
				eState = REDIS_ONLINE;
				eRole = role;
				return true;
		}

		void RedisCommonNode::removeAllSlaveNode()
		{
				std::for_each(_slaveList.begin(), _slaveList.end(), [this](RedisCommonNode *node)
								{
										if (NULL != node)
										{
												node->reset();
												delete node;
										}
								});
				_slaveList.clear();
		}

		void RedisClusterNode::reset(const std::string &ip, const uint32_t port, const std::string &passwd)
		{
				RedisCommonNode::reset(ip, port, passwd);
				if (NULL != _owner)
				{
						RedisCluster *cRedis = dynamic_cast<RedisCluster *>(_owner);
						if (NULL != cRedis)
						{
								for (auto id : _slotSet)
								{
										cRedis->disconnNodeSlot(this, id);
								}
						}
				}
		}
}
