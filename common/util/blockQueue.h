/*
 * =====================================================================================
 *
 *       Filename:  blockQueue.h
 *
 *    Description:  blockQueue
 *
 *        Version:  1.0
 *        Created:  03/11/2017 09:51:15 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BLOCKQUEUE_H__
#define __BLOCKQUEUE_H__

#include <mutex>
#include <condition_variable>
#include <deque>

namespace goddard
{
		template <typename T, typename LockType = std::timed_mutex, typename StrogeType = std::deque<T>>
		class BlockQueue
		{
				public:
						BlockQueue() {}

						virtual ~BlockQueue() {}

						void add(T &item)
						{
								std::lock_guard<LockType> guard(_lock);
								_queue.push_back(item);
								_condition.notify_one();
						}

						void add(T &item, uint64_t timeout)
						{
								if (!_lock.try_lock_for(timeout)) return false;
								_queue.push_back(item);
								_condition.notify_one();
								_lock.unlock();
								return true;
						}

						bool next(T &item)
						{
								std::lock_guard<LockType> guard(_lock);
								while (_queue.empty())
										_condition.wait(_lock);
								if (_queue.empty())
										return false;
								item = _queue.front();
								_queue.pop_front();
								return true;
						}

						bool next(T &item, uint64_t timeout)
						{
								if (!_lock.try_lock_for(timeout)) return false;
								while (_queue.empty())
										if (_condition.wait_for(_lock, timeout) == std::cv_status::timeout)
										{
												_lock.unlock();
												return false;
										}
								if (_queue.empty())
								{
										_lock.unlock();
										return false;
								}
								item = _queue.front();
								_queue.pop_front();
								_lock.unlock();
								return true;
						}

						size_t size()
						{
								std::lock_guard<LockType> guard(_lock);
								return _queue.size();
						}

						size_t size(uint64_t timeout)
						{
								if (!_lock.try_lock_for(timeout)) return false;
								size_t s = _queue.size();
								_lock.unlock();
								return s;
						}

						bool empty()
						{
								std::lock_guard<LockType> guard(_lock);
								return _queue.empty();
						}

				private:
						std::condition_variable _condition;
						LockType _lock;
						StrogeType _queue;
		};
}

#endif
