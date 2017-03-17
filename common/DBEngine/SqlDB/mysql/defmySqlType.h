/*
 * =====================================================================================
 *
 *       Filename:  defmySqlType.h
 *
 *    Description:  defmySqlType
 *
 *        Version:  1.0
 *        Created:  03/15/2017 11:05:21 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __DEFMYSQLTYPE_H__
#define __DEFMYSQLTYPE_H__

#include <cstdint>
#include <cstdlib>
#include <string>
#include <mysql.h>

namespace goddard
{
		struct ConnDBConfig
		{
				ConnDBConfig() : _unPort(0) {}
				std::string _strHost;
				std::string _strUser;
				std::string _strPasswd;
				std::string _strDbName;
				uint16_t _unPort;
		};


		struct DBOperContex
		{
				DBOperContex() : _operID(0) {}
				uint64_t _operID;
		};

		enum DBErrCode
		{
				DB_ERR_OK,
				DB_ERR_ERR,
		};

		enum DBDataType
		{
				DB_TYPE_UNKNOWN,
//				DB_TYPE_BOOL,
				DB_TYPE_INT,
				DB_TYPE_FLOAT,
				DB_TYPE_DOUBLE,
//				DB_TYPE_CHAR,
				DB_TYPE_LONGLONG,
				DB_TYPE_STRING,
		};

		class SqlResultField
		{
				public:
						SqlResultField() : mType(DB_TYPE_UNKNOWN) {}
						SqlResultField(const SqlResultField &f) : mType(f.mType), mValue(f.mValue) {}
						SqlResultField(const char *str, uint32_t len, DBDataType t) : mType(t), mValue(str, len) {}
						SqlResultField(const std::string &str, DBDataType t) : mType(t), mValue(str) {}

						enum DBDataType getType()
						{
								return mType;
						}

						void setType(enum DBDataType type)
						{
								mType = type;
						}

						void setValue(const char *str, uint32_t len)
						{
								mValue.assign(str, len);
						}

						void setValue(const std::string str)
						{
								mValue = str;
						}

						const std::string& getValue() const
						{
								return mValue;
						}

						float getFloat() { return mValue.empty() ? 0.0f : static_cast<float>(atof(mValue.c_str())); }

						double getDouble() { return mValue.empty() ? 0.0f : static_cast<double>(atof(mValue.c_str())); }

						bool getBool() { return mValue.empty() ? false : (atoi(mValue.c_str()) > 0); }

						int32_t getInt32() { return mValue.empty() ? 0 : static_cast<int32_t>(atoll(mValue.c_str())); }

						uint32_t getUInt32() { return mValue.empty() ? 0 : static_cast<uint32_t>(atoll(mValue.c_str())); }

						int64_t getInt64() 
						{ 
								if (mValue.empty())
								{
										return 0;
								}
								int64_t i64;
								sscanf(mValue.c_str(), "%ld", &i64);
								return i64;
						}

						uint64_t getUInt64() 
						{ 
								if (mValue.empty())
								{
										return 0;
								}
								uint64_t u64;
								sscanf(mValue.c_str(), "%lu", &u64);
								return u64;
						}

						enum DBDataType convertType(enum enum_field_types type) const;

				private:
						DBDataType mType;
						std::string mValue;
		};
}

#endif
