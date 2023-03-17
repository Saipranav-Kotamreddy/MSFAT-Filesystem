#!/bin/bash
cd apps
make
if [ $? != 0 ]
then
    echo "Make failed"
    exit
fi
cd ..
docker build -t test .