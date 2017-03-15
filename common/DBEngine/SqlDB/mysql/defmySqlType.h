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

namespace goddard
{
		struct ConnDBConfig
		{
				std::string _strHost;
				std::string _strUser;
				std::string _strPasswd;
				std::string _strDbName;
				uint16_t _unPort;
		};

		class SqlOperation
		{

		};

}

#endif
