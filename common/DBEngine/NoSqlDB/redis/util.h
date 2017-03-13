/*
 * =====================================================================================
 *
 *       Filename:  util.h
 *
 *    Description:  util
 *
 *        Version:  1.0
 *        Created:  03/09/2017 03:32:13 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __UTIL_H_
#define __UTIL_H_
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sstream>

namespace goddard
{
		const uint32_t SLOT_MAX_MASK = 16383;
		uint32_t keyHashSlot(const char *key, int keylen);
		bool getNowMs(uint64_t &ms);

#define print_error(err)	\
		{\
				std::cerr << "error:" << " file: " << __FILE__ << ", line:" << __LINE__ << ", err:" << err << std::endl;	\
		}
}

#endif
