#!/bin/bash

process=`ps aux| grep redis-server | grep -v grep | awk '{print $2}'`

for p in $process
do
		echo "killing redis"
		kill $p
done
