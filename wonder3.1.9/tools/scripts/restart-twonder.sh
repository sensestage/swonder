#!/bin/bash

source cluster.conf
source current.mode
source twonder.settings

TWONDER_EXE="$BINPATH/twonder";

echo $TWONDER_EXE

PERC=55
for i in $CLUSTER_NODES; do 
	rsh $i sudo $SCRIPTS_PATH/node/node-kill-twonder.sh;
	rsh $i sudo $SCRIPTS_PATH/node/node-start-twonder.sh $NUM_SOURCES $SPK_PATH $SWONDER_CONF $TWONDER_EXE;
	echo $PERC
	PERC=$[$PERC+3]
done

echo $TWONDER_EXE
echo $SWONDER_CONF
echo $SPK_PATH
