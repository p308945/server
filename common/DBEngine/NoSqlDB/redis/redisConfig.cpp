/*
 * =====================================================================================
 *
 *       Filename:  redisConfig.cpp
 *
 *    Description:  config
 *
 *        Version:  1.0
 *        Created:  03/09/2017 03:57:54 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  huangyun.goddard (), 895175589@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "redisConfig.h"

namespace goddard
{
		static const uint32_t CONFIG_MAX_LINE = 1024;
		bool RedisConfig::init(const std::string &fileName)
		{
				_config.reset();
				if (!loadConfig(fileName))
				{
						std::ostringstream oss;
						oss << "RedisConfig::init failed,loadConfig return false, fileName:" << fileName;
						print_error(oss.str());
						return false;
				}
				return true;
		}


		bool RedisConfig::loadConfig(const std::string &fileName)
		{
				FILE *fp = NULL;
				if (fileName.empty())
				{
						fp = stdin;
				}
				else
				{
						if ((fp = fopen(fileName.c_str(), "r")) == NULL)
						{
								std::ostringstream oss;
								oss << "loadConfig failed fopen return NULL, fileName:" << fileName;
								print_error(oss.str());
								return false;
						}

				}
				char buf[CONFIG_MAX_LINE + 1] = { 0 };
				sds config = sdsempty();
				while(fgets(buf, CONFIG_MAX_LINE, fp) != NULL)
				{
						config = sdscat(config, buf);
						memset(buf, 0x0, sizeof(buf));
				}
				
				if (fp != stdin)
				{
						fclose(fp);
				}
				bool res = false;
				res = loadConfigFromString(config);
				sdsfree(config);
				if (!res)
				{
						std::ostringstream oss;
						oss << "loadConfigFromString failed";
						print_error(oss.str());
						return false;
				}
				return true;
		}

		bool RedisConfig::loadConfigFromString(const char *config)
		{
				int32_t tolines, i;
				sds *lines;
				lines = sdssplitlen(config, strlen(config), "\n", 1, &tolines);
				bool res = false;
				for (i = 0; i < tolines; ++i)
				{
						lines[i] = sdstrim(lines[i], "\t\r\n");
						sdstolower(lines[i]);
						if (!strncasecmp(lines[i], "//", strlen("//")))
						{
								//notes
								continue;
						}
						uint32_t argc = 0;
						sds *argv;
						argv = parseConfigLine(lines[i], &argc);
						if (NULL != argv)
						{
								res = fetchOneConfigArgv(argc, argv);
								sdsfreesplitres(argv, argc);
								if (!res)
								{
										break;
								}
						}
				}
				sdsfreesplitres(lines, tolines);
				if (!res)
				{
						std::ostringstream oss;
						oss << "loadConfigFromString failed";
						print_error(oss.str());
						return false;
				}
				return true;
		}

		sds *RedisConfig::parseConfigLine(const char *line, uint32_t *argc)
		{
				const char *p = line;
				char **vector = NULL;
				char *current = NULL;
				*argc = 0;
				while (1)
				{
						while(*p && isspace(*p)) ++p;
						if (*p)
						{
								bool done = false;
								if (NULL == current)
								{
										current = sdsempty();
								}
								while(!done)
								{
										switch(*p)
										{
												case ' ':
														{
																done = true;
														}
														break;
												case '\0':
														{
																done = true;
														}
														break;
												default:
														{
																current = sdscatlen(current, p, 1);

														}
														break;

										}
										if (*p)
										{
												++p;
										}
								}
								if (0 != sdslen(current))
								{
										vector = (char **)realloc(vector, (*argc + 1) * sizeof(char *));
										vector[*argc] = current;
										++(*argc);
								}
								else
								{
										sdsfree(current);
								}
								current = NULL;
						}
						else
						{
								return vector;
						}
				}
				return NULL;
		}


		bool RedisConfig::fetchOneConfigArgv(uint32_t argc, sds *argv)
		{
				if (0 == argc)
				{
						std::ostringstream oss;
						oss << "fetchOneConfigArgv failed: argc == 0";
						print_error(oss.str());
						return false;
				}
				if (!strcasecmp("ip", argv[0]) && 2 == argc)
				{
						_config._ip = argv[1];
				}
				else if (!strcasecmp("port", argv[0]) && 2 == argc)
				{
						_config._port = ::strtol(argv[1], NULL, 10);
				}
				else if (!strcasecmp("timeout", argv[0]) && 2 == argc)
				{
						long timeout = ::strtol(argv[1], NULL, 10);
						_config._timeout.tv_sec = timeout / 1000;
						_config._timeout.tv_usec = (timeout % 1000) * 1000;
				}
				else if (!strcasecmp("mode", argv[0]) && 2 == argc)
				{
						if (!strcasecmp("cluster", argv[1]))
						{
								_config._mode = eRedisCluster;
						}
						else if (!strcasecmp("single", argv[1]))
						{
								_config._mode = eRedisSingle;
						}
						else
						{
								std::ostringstream oss;
								oss << "fetchOneConfigArgv failed: mode invalid:" << argv[1];
								print_error(oss.str());
								return false;
						}
				}
				else if (!strcasecmp("passwd", argv[0]) && 2 == argc)
				{
						_config._passwd = argv[1];
				}
				else
				{
						std::ostringstream oss;
						oss << "fetchOneConfigArgv failed: para invalid:" << argv[0];
						print_error(oss.str());
						return false;
				}
				return true;
		}

}
