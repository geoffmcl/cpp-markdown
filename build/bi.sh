#!/bin/sh
#< bi.sh - special params
BN=`basename $0`

# TMPOPTS="-DCMAKE_PREFIX_PATH=$HOME/projects/install/boost_1_67_0"

TMPOPTS="-DCMAKE_PREFIX_PATH=/media/geoff/backup/projects/install/boost_1_61_0"

./build-me.sh $TMPOPTS $@


