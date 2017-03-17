/*
 * =====================================================================================
 *
 *       Filename:  lockedQueue.h
 *
 *    Description:  lockedQueue
 *
 *        Version:  1.0
 *        Created:  03/11/2017 09:16:15 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __LOCKEDQUEUE_H__
#define __LOCKEDQUEUE_H__

#include <deque>
#include <mutex>
#include <cstdint>

namespace goddard
{
		template <typename T, typename LockType = std::timed_mutex, typename StorgeType = std::deque<T>>
				class LockedQueue
				{
						public:
								virtual ~LockedQueue() {}
								void add(const T &item)
								{
										std::lock_guard<LockType> guard(_lock);
										_queue.push_back(item);
								}

								bool add(const T &item, uint64_t timeout)
								{
										if (!_lock.try_lock_for(timeout))
												return false;
										_queue.push_back(item);
										_lock.unlock();
										return true;
								}

								bool next(T &item)
								{
										std::lock_guard<LockType> guard(_lock);
										if (_queue.empty()) return false;
										item = _queue.front();
										_queue.pop_front();
										return true;
								}

								bool next(T &item, uint64_t timeout)
								{
										if (!_lock.try_lock_for(timeout))
												return false;
										if (_queue.empty()) return false;
										item = _queue.front();
										_queue.pop_front();
										_lock.unlock();
										return true;
								}

								bool front(T &item)
								{
										std::lock_guard<LockType> guard(_lock);
										if (_queue.empty())	return false;
										item = _queue.front();
										return true;
								}

								bool size(T &item)
								{
										std::lock_guard<LockType> guard(_lock);
										return _queue.size();
								}

								bool size(T &item, uint64_t timeout)
								{
										if (!_lock.try_lock_for(timeout))
												return false;
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
								LockType _lock;
								StorgeType _queue;
				};
}

#endif
