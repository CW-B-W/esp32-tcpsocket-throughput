#!/bin/bash

head -c $(($1*1024*1024)) < /dev/urandom > dummy.dat
