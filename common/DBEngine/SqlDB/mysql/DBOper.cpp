/*
 * =====================================================================================
 *
 *       Filename:  DBOper.cpp
 *
 *    Description:  DBOper
 *
 *        Version:  1.0
 *        Created:  03/13/2017 09:31:21 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "DBOper.h"

namespace goddard
{
		bool CDbOper::ConnectDB(const ConnDBConfig& cfg, std::string &errStr)
		{
				DisConnect();
				_mysql = mysql_init(NULL);
				if (!_mysql) return false;
				uint32_t timeout = 10;
				mysql_options(_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
				if (!mysql_options(_mysql, MYSQL_SET_CHARSET_NAME, "utf8"))
				{
						goto err;
				}
				if( mysql_real_connect(_mysql, cfg._strHost.c_str(), cfg._strUser.c_str(), cfg._strPasswd.c_str(),
										cfg._strDbName.c_str(), cfg._unPort, NULL, 0) == NULL)
				{
						goto err;
				}
				else
				{
						std::string strSql = "set interactive_timeout=24*3600";
						int nRet = mysql_real_query(_mysql, strSql.c_str(), (unsigned long)strSql.size());
						if(nRet != 0)
						{
								errStr.insert(errStr.length(), mysql_error(_mysql));
						}
						if (!mysql_autocommit(_mysql, 1))
								errStr.insert(errStr.length(), "mysql_autocommit error");
				}
				return true;
err:
				if (_mysql)
				{
						errStr.insert(errStr.length(), mysql_error(_mysql));
						mysql_close(_mysql);
						_mysql = NULL;
				}
				return false;
		}

		void CDbOper::DisConnect()
		{
				if (_mysql)
				{
						mysql_close(_mysql);
						_mysql = NULL;
				}
		}

		bool CDbOper::Execute(SqlOperation *)
		{
				return true;
		}
}
