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
#include <memory>
#include <cstdint>
#include <mysql.h>
#include "defmySqlType.h"
#include "SqlOperObj.h"

namespace goddard
{
		class CDbOper
		{
				public:
						CDbOper() : mMySql(NULL) {}
						virtual ~CDbOper() { DisConnect(); }
						bool ConnectDB(const ConnDBConfig& cfg, std::string &errStr);
						void DisConnect();
						bool Execute(std::shared_ptr<SqlOperObj>);

				private:
						void getSqlResult(MYSQL_RES *result, std::shared_ptr<SqlOperObj>);

				private:
						MYSQL *mMySql;
		};

}

#endif
