#!/bin/bash

cd ./6379 && rm dump.rdb appendonly.aof nodes-6379.conf
cd ../7000 && rm dump.rdb appendonly.aof nodes-7000.conf
cd ../7001 && rm dump.rdb appendonly.aof nodes-7001.conf
cd ../7002 && rm dump.rdb appendonly.aof nodes-7002.conf
cd ../7003 && rm dump.rdb appendonly.aof nodes-7003.conf
cd ../7003 && rm dump.rdb appendonly.aof nodes-7004.conf
