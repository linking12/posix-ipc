#!/bin/bash

g++ -Wall -Wno-write-strings -O2 -shared -fPIC -I. -I/usr/jdk1.8.0_60/include/ -I/usr/jdk1.8.0_60/include/linux  org_mingyu_posix_IPC.cpp org_mingyu_posix_Errno.c -o libposix.so   
