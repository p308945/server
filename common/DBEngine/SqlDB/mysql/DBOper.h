/*
 * =====================================================================================
 *
 *       Filename:  DBOper.h
 *
 *    Description:  DBOper
 *
 *        Version:  1.0
 *        Created:  03/13/2017 09:31:14 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __DBOPER_H__
#define __DBOPER_H__

#include <string>
#include <cstdint>
#include <mysql.h>
#include "defmySqlType.h"

namespace goddard
{
		class CDbOper
		{
				public:
						CDbOper() : _mysql(NULL) {}
						virtual ~CDbOper() { DisConnect(); }
						bool ConnectDB(const ConnDBConfig& cfg, std::string &errStr);
						void DisConnect();
						bool Execute(SqlOperation *);

				private:
						MYSQL *_mysql;
		};

}

#endif
