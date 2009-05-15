#!/bin/bash

LOCAL_PATH=`pwd`

export PKG_CONFIG_PATH=$LOCAL_PATH/local/lib/pkgconfig:$PKG_CONFIG_PATH
export STAGEPATH=$LOCAL_PATH/fasrSource:$LOCAL_PATH/src:$LOCAL_PATH/taskManager
