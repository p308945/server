/*
 * =====================================================================================
 *
 *       Filename:  ByteBuffer.h
 *
 *    Description:  ByteBuffer
 *
 *        Version:  1.0
 *        Created:  03/16/2017 05:45:02 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BYTEBUFFER_H__
#define __BYTEBUFFER_H__

#include <vector>
#include <cstdint>
#include <string>
#include <list>
#include <map>
#include <cstring>

namespace goddard
{
		class ByteBuffer
		{
				public:
						const static size_t DEFAULT_SIZE = 0x1000;
						ByteBuffer(): _rpos(0), _wpos(0)
				{
						_storge.reserve(DEFAULT_SIZE);
				}

						ByteBuffer(size_t res): _rpos(0), _wpos(0)
				{
						_storge.reserve(res);
				}

						ByteBuffer(const ByteBuffer &buf): _rpos(buf._rpos), _wpos(buf._wpos), _storge(buf._storge)
				{
				}

						void clear()
						{
								_storge.clear();
								_rpos = _wpos = 0;
						}

						template <typename T> void append(T value)
						{
								append((uint8_t*)&value, sizeof(value));
						}

						template <typename T> void put(size_t pos, T value)
						{
								put(pos, (uint8_t*)&value, sizeof(value));
						}

						ByteBuffer &operator<<(bool value)
						{
								append<uint8_t>((uint8_t)value);
								return *this;
						}

						ByteBuffer &operator<<(uint8_t value)
						{
								append<uint8_t>(value);
								return *this;
						}

						ByteBuffer &operator<<(uint16_t value)
						{
								append<uint16_t>(value);
								return *this;
						}

						ByteBuffer &operator<<(uint32_t value)
						{
								append<uint32_t>(value);
								return *this;
						}

						ByteBuffer &operator<<(uint64_t value)
						{
								append<uint64_t>(value);
								return *this;
						}

						ByteBuffer &operator<<(int8_t value)
						{
								append<int8_t>(value);
								return *this;
						}

						ByteBuffer &operator<<(int16_t value)
						{
								append<int16_t>(value);
								return *this;
						}

						ByteBuffer &operator<<(int32_t value)
						{
								append<int32_t>(value);
								return *this;
						}

						ByteBuffer &operator<<(int64_t value)
						{
								append<int64_t>(value);
								return *this;
						}

						ByteBuffer &operator<<(float value)
						{
								append<float>(value);
								return *this;
						}

						ByteBuffer &operator<<(double value)
						{
								append<double>(value);
								return *this;
						}

						ByteBuffer &operator<<(long double value)
						{
								append<long double>(value);
								return *this;
						}

						ByteBuffer &operator<<(const std::string &value)
						{
								append<uint32_t>(value.length());
								append((const uint8_t *)value.c_str(), value.length());
								return *this;
						}

						ByteBuffer &operator<<(const char *str)
						{
								uint32_t len = str ? strlen(str) : 0;
								append<uint32_t>(len);
								append(str, len);
								return *this;
						}

						ByteBuffer &operator>>(bool &value)
						{
								value = read<char>() > 0 ? true : false;
								return *this;
						}

						ByteBuffer &operator>>(uint8_t &value)
						{
								value = read<uint8_t>();
								return *this;
						}

						ByteBuffer &operator>>(uint16_t &value)
						{
								value = read<uint16_t>();
								return *this;
						}

						ByteBuffer &operator>>(uint32_t &value)
						{
								value = read<uint32_t>();
								return *this;
						}

						ByteBuffer &operator>>(uint64_t &value)
						{
								value = read<uint64_t>();
								return *this;
						}

						ByteBuffer &operator>>(int8_t &value)
						{
								value = read<int8_t>();
								return *this;
						}

						ByteBuffer &operator>>(int16_t &value)
						{
								value = read<int16_t>();
								return *this;
						}

						ByteBuffer &operator>>(int32_t &value)
						{
								value = read<int32_t>();
								return *this;
						}

						ByteBuffer &operator>>(int64_t &value)
						{
								value = read<int64_t>();
								return *this;
						}

						ByteBuffer &operator>>(float &value)
						{
								value = read<float>();
								return *this;
						}

						ByteBuffer &operator>>(double &value)
						{
								value = read<double>();
								return *this;
						}

						ByteBuffer &operator>>(long double &value)
						{
								value = read<long double>();
								return *this;
						}

						ByteBuffer &operator>>(std::string &value)
						{
								value.clear();
								uint32_t len = read<uint32_t>();
								if (len > 0)
								{
										value.resize(len);
										read((uint8_t *)&value[0], len);
								}
								return *this;
						}

						ByteBuffer &operator>>(char *value)
						{
								uint32_t len = read<uint32_t>();
								if (len > 0)
								{
										read((uint8_t *)value, len);
								}
								return *this;
						}

						uint8_t operator[](size_t pos) const
						{
								return read<uint8_t>(pos);
						}

						size_t rpos() const
						{
								return _rpos;
						}

						size_t rpos(size_t pos)
						{
								size_t p = _rpos;
								_rpos = pos;
								return p;
						}

						size_t wpos() const
						{
								return _wpos;
						}

						size_t wpos(size_t pos)
						{
								size_t p = _wpos;
								_wpos = pos;
								return p;
						}

						template <typename T> T read()
						{
								T r = read<T>(_rpos);
								_rpos += sizeof(T);
								return r;
						}

						template <typename T> T read(size_t pos) const
						{
								T val = *((const T*)&_storge[pos]);
								return val;
						}

						void read(uint8_t *dest, size_t len)
						{
								memcpy(dest, &_storge[_rpos], len);
								_rpos += len;
						}

						const uint8_t *contents() const { return &_storge[0]; }

						size_t size() const { return _storge.size(); }
						bool empty() const { return _storge.empty(); }

						void resize(size_t newsize) 
						{ 
								_storge.resize(newsize);
								_rpos = 0;
								_wpos = size();
						}

						void reserve(size_t resize)
						{
								if (resize > size()) _storge.reserve(resize);
						}

						void append(const std::string &str)
						{
								append((const uint8_t *)str.c_str(), str.size() + 1);
						}

						void append(const char *src, size_t cnt)
						{
								append((const uint8_t *)src, cnt);
						}

						template <typename T>
						void append(T *src, size_t cnt)
						{
								append((const uint8_t *)src, cnt * sizeof(T));
						}

						void append(const uint8_t *src, size_t cnt)
						{
								if (!src) return;
								if (_storge.size() < _wpos + cnt)
								{
										_storge.resize(_wpos + cnt);
								}
								memcpy(&_storge[_wpos], src, cnt);
								_wpos += cnt;
						}

						void append(const ByteBuffer &buf)
						{
								if (buf.size() > 0) append(buf.contents(), buf.size());
						}

						void put(size_t pos, const uint8_t *src, size_t len)
						{
								memcpy(&_storge[pos], src, len);
						}

				protected:
						size_t _rpos, _wpos;
						std::vector<uint8_t> _storge;
		};

		template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::vector<T> v)
		{
				b << v.size();
				for (auto &ref : v)
				{
						b << ref;
				}
				return b;
		}

		template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
		{
				size_t size;
				b >> size;
				while (size--)
				{
						T t;
						b >> t;
						v.push_back(t);
				}
				return b;
		}

		template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::list<T> v)
		{
				b << v.size();
				for (auto &ref : v)
				{
						b << ref;
				}
				return b;
		}

		template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
		{
				size_t size;
				b >> size;
				while (size--)
				{
						T t;
						b >> t;
						v.push_back(t);
				}
				return b;
		}

		template <typename K, typename V> ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> v)
		{
				b << v.size();
				for (auto &ref : v)
				{
						b << ref.first << ref.second;
				}
				return b;
		}

		template <typename K, typename V> ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &v)
		{
				size_t size;
				b >> size;
				while (size--)
				{
						K k;
						V v;
						b >> k >> v;
						v.insert(std::make_pair(k, v));
				}
				return b;
		}
}

#endif
