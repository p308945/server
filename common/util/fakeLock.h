/*
 * =====================================================================================
 *
 *       Filename:  fakeLock.h
 *
 *    Description:  fakeLock
 *
 *        Version:  1.0
 *        Created:  03/11/2017 09:27:10 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

namespace goddard
{
		class FakeLock
		{
				public:
						FakeLock() {}
						virtual ~FakeLock() {}
						void lock() {}
						bool try_lock() { return true; }
						template <typename T>
								bool try_lock_for(T &t) { return true; }
						template <typename T>
								bool try_lock_util(T &t) { return true; }
						void unlock() {}
		}
}
