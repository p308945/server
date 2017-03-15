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

#include <memory>
#include <atomic>
#include "lockedQueue.h"
#include "DBOper.h"

namespace goddard
{
		class SqlDelayThread
		{
				using SqlQueue = LockedQueue<SqlOperation*>;
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


				void delay(SqlOperation *sql) { _sqlQueue.add(sql); }
				private:
				SqlQueue _sqlQueue;
				CDbOper _oper;
				std::atomic<bool> _running;
		};
}

#endif
