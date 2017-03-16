/*
 * =====================================================================================
 *
 *       Filename:  SqlDelayThread.cpp
 *
 *    Description:  SqlDelayThread
 *
 *        Version:  1.0
 *        Created:  03/13/2017 11:34:22 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <mysql.h>
#include <unistd.h>
#include "SqlDelayThread.h"

namespace goddard
{
		SqlDelayThread::SqlDelayThread() : _running(true) {}

		bool SqlDelayThread::init()
		{
				return true;
		}

		void SqlDelayThread::run()
		{
				ThreadStart();
				std::shared_ptr<SqlOperObj> s;
				bool ret = false;
				while (_running)
				{
						usleep(10000);
						while (!_sqlQueue.empty())
						{
								ret = _sqlQueue.next(s);
								if (ret)
								{
										_oper.Execute(s);
										addResObj(s);
								}
						}
				}
				ThreadEnd();
		}
}
