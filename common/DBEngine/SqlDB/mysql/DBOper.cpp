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
				mMySql = mysql_init(NULL);
				if (!mMySql) return false;
				uint32_t timeout = 10;
				mysql_options(mMySql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
				if (!mysql_options(mMySql, MYSQL_SET_CHARSET_NAME, "utf8"))
				{
						goto err;
				}
				if( mysql_real_connect(mMySql, cfg._strHost.c_str(), cfg._strUser.c_str(), cfg._strPasswd.c_str(),
										cfg._strDbName.c_str(), cfg._unPort, NULL, 0) == NULL)
				{
						goto err;
				}
				else
				{
						std::string strSql = "set interactive_timeout = 24 * 3600";
						int nRet = mysql_real_query(mMySql, strSql.c_str(), (unsigned long)strSql.size());
						if(nRet != 0)
						{
								errStr.insert(errStr.length(), mysql_error(mMySql));
						}
						if (!mysql_autocommit(mMySql, 1))
								errStr.insert(errStr.length(), " mysql_autocommit error");
						char value = 1;
						if (!mysql_options(mMySql, MYSQL_OPT_RECONNECT, (char *)&value))
								errStr.insert(errStr.length(), " set MYSQL_OPT_RECONNECT error");
				}
				return true;
err:
				if (mMySql)
				{
						errStr.insert(errStr.length(), mysql_error(mMySql));
						mysql_close(mMySql);
						mMySql = NULL;
				}
				return false;
		}

		void CDbOper::DisConnect()
		{
				if (mMySql)
				{
						mysql_close(mMySql);
						mMySql = NULL;
				}
		}

		bool CDbOper::Execute(std::shared_ptr<SqlOperObj> obj)
		{
				if (!mMySql)
				{
						return false;
				}
				const std::string &sql = obj->getSql();
				if (0 != mysql_real_query(mMySql, sql.c_str(), sql.size()))
				{
						std::string err = mysql_error(mMySql);
						return false;
				}
				MYSQL_RES *result = NULL;
				result = mysql_store_result(mMySql);
				if (result)
				{
						getSqlResult(result, obj);
				}
				obj->setErrCode(DBErrCode::DB_ERR_OK);
				return true;
		}

		void CDbOper::getSqlResult(MYSQL_RES *result, std::shared_ptr<SqlOperObj> obj)
		{
				if (!result)
				{
						return;
				}
				uint32_t num_fields = mysql_num_fields(result);
				if (0 == num_fields)
				{
						return;
				}
				MYSQL_FIELD * fd;
				using fieldPair = std::pair<std::string, enum enum_field_types>;
				std::vector<fieldPair> fieldPairVec;
				fieldPairVec.reserve(num_fields);
				for (uint32_t i = 0; i < num_fields; ++i)
				{
						fd = mysql_fetch_field(result);
						std::string name;
						name.assign(fd->name, fd->name_length);
						obj->addFieldName(name);
						fieldPairVec.emplace_back(name, fd->type);
				}

				MYSQL_ROW row;
				SqlResultFieldPtrVec resVec;

				while(1)
				{
						row = mysql_fetch_row(result);
						if (!row)
						{
								break;
						}
						uint64_t *length = mysql_fetch_lengths(result);
						for (uint32_t i = 0; i < num_fields; ++i)
						{
								resVec.clear();
								char *s = row[i];
								SqlResultFieldPtr res(new SqlResultField());
								res->setValue(s, length[i]);
								res->setType(res->convertType(fieldPairVec.at(i).second));
								resVec.push_back(res);
								obj->addResultFieldVec(resVec);
						}
				}
		}
}
