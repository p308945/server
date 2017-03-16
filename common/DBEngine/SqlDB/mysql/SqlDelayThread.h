/*
 * =====================================================================================
 *
 *       Filename:  sqlDelayThread.h
 *
 *    Description:  sqlDelayThread
 *
 *        Version:  1.0
 *        Created:  03/11/2017 10:24:30 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __SQLDELAYTHREAD_H__
#define __SQLDELAYTHREAD_H__

#include <queue>
#include <memory>
#include <atomic>
#include "lockedQueue.h"
#include "DBOper.h"

namespace goddard
{
		class SqlDelayThread
		{
				using SqlQueue = LockedQueue<std::shared_ptr<SqlOperObj>>;
				public:
				SqlDelayThread();

				virtual void run();
				bool init();

				virtual void stop()
				{
						_running = false;
				}

				virtual bool BeginTransaction()                     // nothing do if DB not support transactions
				{
						return true;
				}
				virtual bool CommitTransaction()                    // nothing do if DB not support transactions
				{
						return true;
				}
				virtual bool RollbackTransaction()                  // can't rollback without transaction support
				{
						return false;
				}

				// must be called before first query in thread (one time for thread using one from existed Database objects)
				virtual void ThreadStart() {}
				// must be called before finish thread run (one time for thread using one from existed Database objects)
				virtual void ThreadEnd() {}
				void delay(std::shared_ptr<SqlOperObj> sql) { _sqlQueue.add(sql); }

				void addResObj(std::shared_ptr<SqlOperObj> res)
				{
						std::lock_guard<std::mutex>lock(_resMtx);
						_sqlRes.push(res);
				}

				void swapRes(std::queue<std::shared_ptr<SqlOperObj>> &resQueue)
				{
						std::lock_guard<std::mutex>lock(_resMtx);
						resQueue.swap(_sqlRes);
				}

				private:
				SqlQueue _sqlQueue;
				std::queue<std::shared_ptr<SqlOperObj>> _sqlRes;
				CDbOper _oper;
				std::atomic<bool> _running;
				std::mutex _resMtx;
		};
}

#endif
