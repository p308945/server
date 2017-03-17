/*
 * =====================================================================================
 *
 *       Filename:  SqlOperObj.h
 *
 *    Description:  SqlOperObj
 *
 *        Version:  1.0
 *        Created:  03/16/2017 03:49:54 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __SQLOPEROBJ_H__
#define __SQLOPEROBJ_H__

#include <vector>
#include <list>
#include <memory>
#include "defmySqlType.h"

namespace goddard
{
		using SqlResultFieldPtr = std::shared_ptr<SqlResultField>;
		using SqlResultFieldPtrVec = std::vector<SqlResultFieldPtr>;
		using SqlResultFieldPtrVecList = std::list<SqlResultFieldPtrVec>;

		class SqlOperObj
		{
				public:
						SqlOperObj() : mErrCode(DBErrCode::DB_ERR_ERR) {}
						void setSql(const std::string &sql)
						{
								mSql = sql;
						}

						const std::string & getSql() const
						{
								return mSql;
						}

						void addFieldName(const std::string &name)
						{
								mFieldName.push_back(name);
						}

						void addResultFieldVec(const SqlResultFieldPtrVec &vec)
						{
								mResultList.push_back(vec);
						}

						void setErrCode(enum DBErrCode code)
						{
								mErrCode = code;
						}

						enum DBErrCode getErrCode() const
						{
								return mErrCode;
						}

				private:
						std::string mSql;
						DBOperContex mCtx;
						std::vector<std::string> mFieldName;
						SqlResultFieldPtrVecList mResultList;
						enum DBErrCode mErrCode;
		};
}

#endif
