#! /bin/sh

#  @(#)identconn.sh  1.1 (2.11BSD) 2025/12/26

PATH=/usr/bin:/usr/sbin:/usr/ucb ; export PATH

netstat -f inet -n | grep ESTAB | itest

