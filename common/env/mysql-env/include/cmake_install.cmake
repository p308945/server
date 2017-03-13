# Install script for directory: /home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local/mysql")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql_com.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql_time.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_list.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_alloc.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/typelib.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql/plugin.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql/plugin_audit.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql/plugin_ftparser.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql/plugin_validate_password.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_dbug.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/m_string.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_sys.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_xml.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql_embed.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_pthread.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/decimal.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/errmsg.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_global.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_net.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_getopt.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/sslopt-longopts.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_dir.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/sslopt-vars.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/sslopt-case.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/sql_common.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/keycache.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/m_ctype.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_attribute.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_compiler.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql_com_server.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_byteorder.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/byte_order_generic.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/byte_order_generic_x86.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/byte_order_generic_x86_64.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/little_endian.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/big_endian.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql_version.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/my_config.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysqld_ername.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysqld_error.h"
    "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/sql_state.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mysql" TYPE DIRECTORY FILES "/home/huangyun/project/server/common/env/mysql-env/mysql-5.6.35/include/mysql/" REGEX "/[^/]*\\.h$" REGEX "/psi\\_abi[^/]*$" EXCLUDE)
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

