/*
 * =====================================================================================
 *
 *       Filename:  mathUtil.h
 *
 *    Description:  mathUtil
 *
 *        Version:  1.0
 *        Created:  03/10/2017 09:42:23 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __MATHUTIL_H__
#define __MATHUTIL_H__

#include <cstdint>

namespace goddard
{
		inline bool isodd(int64_t num)
		{
				if (num & 1)
						return true;
				else
						return false;
		}

		inline bool iseven(int64_t num)
		{
				return !isodd(num);
		}
}

#endif
