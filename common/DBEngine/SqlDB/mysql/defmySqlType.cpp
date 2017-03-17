/*
 * =====================================================================================
 *
 *       Filename:  defmySqlType.cpp
 *
 *    Description:  defmySqlType
 *
 *        Version:  1.0
 *        Created:  03/17/2017 02:41:37 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "defmySqlType.h"

namespace goddard
{
		enum DBDataType SqlResultField::convertType(enum enum_field_types type) const
		{
				DBDataType ret = DBDataType::DB_TYPE_UNKNOWN;
				switch (type)
				{
						case FIELD_TYPE_TIMESTAMP:
						case FIELD_TYPE_DATE:
						case FIELD_TYPE_TIME:
						case FIELD_TYPE_DATETIME:
						case FIELD_TYPE_STRING:
						case FIELD_TYPE_VAR_STRING:
						case FIELD_TYPE_BLOB:
						case FIELD_TYPE_SET:
						case FIELD_TYPE_NULL:
								{
										ret = DBDataType::DB_TYPE_STRING;
								}
								break;
						case FIELD_TYPE_TINY:
						case FIELD_TYPE_SHORT:
						case FIELD_TYPE_LONG:
						case FIELD_TYPE_INT24:
						case FIELD_TYPE_ENUM:
								{
								
										ret = DBDataType::DB_TYPE_INT;
								}
								break;
						case FIELD_TYPE_LONGLONG:
								{

												ret = DBDataType::DB_TYPE_LONGLONG;
								}
								break;
						case FIELD_TYPE_FLOAT:
								{
										ret = DBDataType::DB_TYPE_FLOAT;
								}
								break;
						case FIELD_TYPE_DECIMAL:
						case FIELD_TYPE_DOUBLE:
								{
										ret = DBDataType::DB_TYPE_DOUBLE;
								}
								break;
						default:
								{

								}
								break;
				}

				return ret;
		}
}
